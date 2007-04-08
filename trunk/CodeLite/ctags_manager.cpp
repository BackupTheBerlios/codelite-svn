#include "precompiled_header.h"

#include "ctags_manager.h"
#include <wx/txtstrm.h>
#include <wx/file.h>
#include <algorithm>
#include "process.h"
#include "language.h"
#include <wx/progdlg.h>
#include "dirtraverser.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

#ifdef USE_TRACE
#define DEBUG_START_TIMER(msg) { wxString logline; logline << _T("Timer started ===> ") << msg; m_watch.Start(); wxLogMessage(logline); }
#define DEBUG_STOP_TIMER()   { wxString msg; msg << _T("Done, total time elapsed: ") << m_watch.Time() << _T(" milliseconds"); wxLogMessage(msg); }
#else
#define DEBUG_START_TIMER(msg)
#define DEBUG_STOP_TIMER()
#endif

/// Descending sorting function
struct SDescendingSort
{
	bool operator()(const TagEntry &rStart, const TagEntry &rEnd)
	{
		return rStart.GetName().CompareTo(rEnd.GetName()) > 0;
	}
};

/// Ascending sorting function
struct SAscendingSort
{
	bool operator()(const TagEntry &rStart, const TagEntry &rEnd)
	{
		return rEnd.GetName().CompareTo(rStart.GetName()) > 0;
	}
};

TagsManager::TagsManager()
: m_ctagsPath(_T("ctags"))
, m_ctags(NULL)
, m_localCtags(NULL)
, m_parseComments(false)
{
	m_pDb = new TagsDatabase();
	m_pExternalDb = new TagsDatabase(true);	// use it as memory database

	// Initialise ctags command pattern
	m_ctagsCmd[TagsGlobal] = _T("  --language-force=C++ --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p --filter=yes --filter-terminator=\"<<EOF>>\" ");
	m_ctagsCmd[TagsLocal] =  _T("  --language-force=C++ --fields=aKmSsnit --c-kinds=+l --C++-kinds=+l --filter=yes --filter-terminator=\"<<EOF>>\" ");
	
	m_validExtensions[wxT("cxx")] = true;
	m_validExtensions[wxT("cpp")] = true;
	m_validExtensions[wxT("h")]   = true;
	m_validExtensions[wxT("hpp")] = true;
	m_validExtensions[wxT("c++")] = true;
	m_validExtensions[wxT("c")]   = true;
}  

TagsManager::~TagsManager()
{
	delete m_pDb;
	delete m_pExternalDb;
}

void TagsManager::OpenDatabase(const wxFileName& fileName)
{
	m_pDb->OpenDatabase(fileName);
}

TagTreePtr TagsManager::ParseTagsFile(const wxFileName& fp, const wxString& project)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);


	tagFileInfo info;
    tagEntry entry;
	wxString tagFileName = fp.GetFullPath();
	const wxCharBuffer fileName = _C(tagFileName);

	tagFile *const file = tagsOpen(fileName.data(), &info);
	if( !file ) 
	{
		DEBUG_MSG(_T("cant open tag file!"));
		return TagTreePtr( NULL );
	}

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(_T("<ROOT>"));
	
	TagTreePtr tree( new TagTree(_T("<ROOT>"), root) );
	while (tagsNext (file, &entry) == TagSuccess)
	{
		TagEntry tag( entry, project );
		tree->AddEntry(tag);
	}
	tagsClose(file);
	return tree;
}

void TagsManager::TagFromLine(const wxString& line, TagEntry& tag, const wxString& project)
{
	// Parse ctags line
	static const char TAB = '\t';
	const wxCharBuffer pline = _C(line);
	char *p = const_cast<char*>(pline.data());
	;
	char *tab = strchr (p, TAB);
	int fieldsPresent = 0;

	// Extract the following fields from the tag
	char *fileName = NULL;
	char *name = NULL;
	int lineNumber = 0;
	char *pattern = NULL;
	char *kind = NULL;
	int fileScope = 0;
	std::map<wxString, wxString> extFields;

	// Parse tag name
	name = p;
	if( tab )
	{
		*tab = 0;
		p = tab + 1;

		// Parse file name
		fileName = const_cast<char*>(p);
		tab = strchr (p, TAB);
		if( tab )
		{
			*tab = 0;
			p = tab + 1;
			if (*p == '/'  ||  *p == '?')
			{
				/* parse pattern */
				int delimiter = *(unsigned char*) p;
				lineNumber = 0;
				pattern = const_cast<char*>(p);
				do
				{
					p = strchr (p + 1, delimiter);
				} while (p != NULL  &&  *(p - 1) == '\\');
				if (p == NULL)
				{
					/* invalid pattern */
				}
				else
					++p;
			}
			else if (isdigit ((int) *(unsigned char*) p))
			{
				/* parse line number */
				pattern = const_cast<char*>(p);
				lineNumber = atol (p);
				while (isdigit ((int) *(unsigned char*) p))
					++p;
			}
			else
			{
				/* invalid pattern */
			}
			fieldsPresent = (strncmp (p, ";\"", 2) == 0);
			*p = '\0';
			if (fieldsPresent)
			{
				p = p + 2;
				while (p != NULL  &&  *p != '\0')
				{
					while (*p == TAB)
						*p++ = '\0';
					if (*p != '\0')
					{
						char *colon;
						char *field = p;
						p = strchr (p, TAB);
						if (p != NULL)
							*p++ = '\0';
						colon = strchr (field, ':');
						if (colon == NULL)
							kind = field;
						else
						{
							char *key = field;
							char *value = colon + 1;
							*colon = '\0';
							if (strcmp (key, "kind") == 0)
								kind = value;
							else if (strcmp (key, "file") == 0)
								fileScope = 1;
							else if (strcmp (key, "line") == 0)
								lineNumber = atol (value);
							else
							{
								extFields[_U(key)] = _U(value);
							}
						}
					}
				}
			}
		}
	}

	tag.Create(fileName, name, lineNumber, pattern, kind, extFields, project);
}

TagTreePtr TagsManager::ParseTags(const wxString& tags, const wxString& project)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(_T("<ROOT>"));

	TagTreePtr tree( new TagTree(_T("<ROOT>"), root) );

	const wxCharBuffer data = _C(tags);
	char* pStart = const_cast<char*>(data.data());
	char* pEnd = strstr(pStart, "\n");
	while( pEnd )
	{
		TagEntry tag;
		int len = (pEnd-pStart);
		char *tagLine = new char[len+1];
		memset(tagLine, 0, len+1);
		memcpy(tagLine, pStart, len);
		wxString line( _U(tagLine) );
		delete [] tagLine;

		// Construct the tag from the line
		TagFromLine(line, tag, project);

		// Add the tag to the tree
		tree->AddEntry(tag);

		// Get next line
		pStart = pEnd + 1;
		pEnd = strstr(pStart, "\n");
	}
	return tree;
}

TagTreePtr TagsManager::ParseSourceFile(const wxFileName& fp, const wxString& project, std::vector<DbRecordPtr> *comments)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	wxString tags;
	
	if(m_ctags == NULL)
	{
		DEBUG_MSG(_T("ctags process, for global scope, is not running, call TagsManager::StartCtagsProcess()"));
		return TagTreePtr( NULL );
	}

	SourceToTags(fp, tags, m_ctags);

	//	return ParseTagsFile(tags, project);
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags, project) );

	if( comments && m_parseComments )
	{
		// parse comments
		LanguageST::Get()->ParseComments( fp, comments );
		
	}
	return ttp;
}

void TagsManager::CreateProject(const wxString &projectName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Create a tag entry of type project and fill the missing 
	// values
	TagEntry proj;
	proj.SetPath(projectName);
	proj.SetName(projectName);
	proj.SetProject(projectName);
	proj.SetKind(_T("project"));

	// Obtain an sql insert statement
	wxSQLite3Statement insertStmt = m_pDb->PrepareStatement(proj.GetInsertOneStatement());

	// store the project into the database
	proj.Store( insertStmt );
}

TagTreePtr TagsManager::ParseSourceFiles(const std::vector<wxFileName> &fpArr, const wxString& project, std::vector<DbRecordPtr> *comments)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	wxString tags;
	
	if(m_ctags == NULL)
	{
		DEBUG_MSG(_T("ctags process, for global scope, is not running, call TagsManager::StartCtagsProcess()"));
		return TagTreePtr(NULL);
	}
		
	size_t i=0;
	for(; i<fpArr.size(); i++)
	{
		wxString fileTags;
		SourceToTags(fpArr[i], fileTags, m_ctags);

		if( comments && m_parseComments )
		{
			// parse comments
			LanguageST::Get()->ParseComments( fpArr[i], comments );
		}
		tags << fileTags;
	}

	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags, project) );
	return ttp;
}

std::vector<TagEntry>* TagsManager::ParseLocals(const wxString& scope)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	wxString tags;

	if(m_localCtags == NULL)
	{
		DEBUG_MSG(_T("ctags process, for local scope, is not running, call TagsManager::StartCtagsProcess()"));
		return NULL;
	}

	// Put the content into temporary file
	wxFileName fp(wxGetCwd(), _T("temp_source_file"));
	
	wxFile file(fp.GetFullPath(), wxFile::write);
	file.Write(scope);
	file.Close();

	// Convert source file to tags
	SourceToTags(fp, tags, m_localCtags);	

	wxRemoveFile(fp.GetFullPath());

	// Create a vector from the tags
	return VectorFromTags(tags);
}

std::vector<TagEntry>* TagsManager::VectorFromTags(const wxString& tags, bool onlyLocals)
{
	std::vector<TagEntry> *vtags = new std::vector<TagEntry>;
	const wxCharBuffer data = _C(tags);
	char* pStart = const_cast<char*>(data.data());
	char* pEnd = strstr(pStart, "\n");
	while( pEnd )
	{
		TagEntry tag;

		// Convert the current pointer to line
		int len = (pEnd-pStart);
		char *tagLine = new char[len+1];
		memset(tagLine, 0, len+1);
		memcpy(tagLine, pStart, len);
		wxString line( _U(tagLine) );
		delete [] tagLine;

		// Construct the tag from the line
		TagFromLine(line, tag, wxEmptyString);

		// Only local tags are collected
		if(onlyLocals && tag.GetKind() == _T("local"))
			vtags->push_back(tag);

		// collect all?
		if(!onlyLocals)
			vtags->push_back(tag);

		// Get next line
		pStart = pEnd + 1;
		pEnd = strstr(pStart, "\n");
	}
	return vtags;
}

//-----------------------------------------------------------
// Database operations
//-----------------------------------------------------------

void TagsManager::Store(TagTreePtr tree, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store(tree, path);
}

void TagsManager::Store(const std::vector<TagEntry> &tags, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store(tags, path);
}

TagTreePtr TagsManager::Load(const wxFileName& path, const wxString& project)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Incase empty file path is provided, use the current file name
	TagTreePtr tree;

	try	
	{
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByProject(project, path);

		// Load the records and build a language tree
		TagEntry root;
		std::vector<TagEntry> rejectedTags;
		root.SetName(_T("<ROOT>"));
		tree.Reset( new TagTree(_T("<ROOT>"), root) );
		while( rs.NextRow() )
		{
			TagEntry entry(rs);
			tree->AddEntry(entry);
		}
		rs.Finalize();
	}
	catch (wxSQLite3Exception& e)
	{
		std::cerr << e.GetErrorCode() << wxT(":") << e.GetMessage().mb_str() << std::endl;
	}
	return tree;
}

void TagsManager::Delete(const wxFileName& path, const wxString& project, const wxString& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Delete(path, project, fileName);
}

//--------------------------------------------------------
// Process Handling of CTAGS
//--------------------------------------------------------

TagsProcess* TagsManager::StartCtagsProcess(wxEvtHandler* evtHandler, int id, int kind)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Run ctags process
	wxString cmd;
	
	// Get ctags flags from the map
	std::map<int, wxString>::iterator iter = m_ctagsCmd.find(kind);
	if( iter == m_ctagsCmd.end() )
	{
		DEBUG_MSG(_T("Faild to start ctags: Invalid ctags kind"));
		return NULL;
	}
	
	// build the command, we surround ctags name with double quatations
	cmd << _T("\"") << m_ctagsPath.GetFullPath() << _T("\"") << iter->second;
	TagsProcess* process;

	if(evtHandler == NULL)
		process = new TagsProcess(wxPROCESS_REDIRECT);
	else
	{
		process = new TagsProcess(evtHandler, id);
		process->Redirect();
	}

	// Launch it!
	long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
	process->SetPid( pid );

	if( process->GetPid() <= 0 )
	{
		DEBUG_MSG(_T("Failed to start ctags process"));
		return NULL;
	}

	kind == TagsGlobal ? m_ctags = process : m_localCtags = process;
	return process;
}

void TagsManager::SetCtagsPath(const wxString& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_ctagsPath = wxFileName(path, _T("ctags"));
}

void TagsManager::SourceToTags(const wxFileName& source, wxString& tags, TagsProcess* ctags)
{
	// Send the command to ctags
	wxOutputStream *out = ctags->GetOutputStream();
	if( out ) 
	{
		wxString cmd(source.GetFullPath());
		cmd += _T("\n");

		const wxCharBuffer pWriteData = _C(cmd);
		out->Write(pWriteData.data(), cmd.Length());
	}
	else
	{
		DEBUG_MSG(_T("Failed to connect to ctags process's standard input"));
		return;
	}
	
	// Read ctags output
	while( true )
	{
		if( ctags->IsInputAvailable() )
		{
			wxTextInputStream in(*ctags->GetInputStream());
			wxString msg;
			msg << in.ReadLine() << _T("\n");
			if( msg.Contains(_T("<<EOF>>")) )
				break;
			tags += msg;
		}
		else
			wxMilliSleep( 1 );
	}
}

TagTreePtr TagsManager::TreeFromTags(const wxString& tags, const wxString& project)
{
	// Load the records and build a language tree
	TagEntry root;
	root.SetName(_T("<ROOT>"));

	TagTreePtr tree( new TagTree(_T("<ROOT>"), root) );

	const wxCharBuffer data = _C(tags);
	char* pStart = const_cast<char*>(data.data());
	char* pEnd = strstr(pStart, "\n");
	while( pEnd )
	{
		TagEntry tag;

		int len = (pEnd-pStart);
		char *tagLine = new char[len+1];
		memset(tagLine, 0, len+1);
		memcpy(tagLine, pStart, len);

		wxString line( _U(tagLine) );
		delete [] tagLine;

		// Construct the tag from the line
		TagFromLine(line, tag, project);

		// Add the tag to the tree, locals are not added to the 
		// tree
		if( tag.GetKind() != _T("local") )
			tree->AddEntry(tag);

		// Get next line
		pStart = pEnd + 1;
		pEnd = strstr(pStart, "\n");
	}
	return tree;
}

bool TagsManager::FunctionByLine(const int lineNo, const wxString& fileName, const wxString& project, TagEntry& tag)
{
	wxString query;
	query << _T("select * from tags where line <=") << lineNo << _T(" AND  project='") << project << _T("' and file='") << fileName << _T("' and kind='function' order by line DESC");

	try	
	{
		wxSQLite3ResultSet rs = m_pDb->Query( query );

		// Load the records and build a language tree
		// We need the last record 
		if( rs.NextRow() )
			tag = TagEntry(rs);
			 
		rs.Finalize();
	}
	catch (wxSQLite3Exception& e)
	{
		std::cerr << e.GetErrorCode() << ":" << e.GetMessage().mb_str() << std::endl;
		return false;
	}
	return true;
}

void TagsManager::GetMembers(const wxString & qualifier, const wxString & name, std::vector<TagEntry> & tags)
{
	wxString query;
	std::map<wxString, TagEntry> tmpMap;
	std::vector<TagEntry> vt;

	// get all tags belongs to this qualifier
	TagsByParent(qualifier, tags, true);

	// filter out all tags that does not match the name
	FilterResults(tags, name, vt, ExactMatch, &tmpMap);

	// Fill up the results from the tmporary map into the result
	// tags vector
	tags.clear();
	std::map<wxString, TagEntry>::iterator iter = tmpMap.begin();
	for(; iter != tmpMap.end(); iter++)
		tags.push_back(iter->second);

	// Sort the results base on their name
	std::sort(tags.begin(), tags.end(), SDescendingSort());	
}

// return list of possibilities for word completion (Ctrl+Space)
void TagsManager::GetTags(const wxString& name, const wxString& scopeName, std::vector<TagEntry>& tags, int flags, const wxString& scope, bool allowDups)
{
	// We need to add a list of entries from the database and add it to the local one
	wxString query, msg;
	wxString escapedName(name);
	std::vector<TagEntry> *localTags(NULL);
	std::map<wxString, TagEntry> tmpMap;

	// If we have local scope, we process it first
	if( scope.IsEmpty() == false )
	{
		// Get a list of tags from the current scope, first remove the non-visible scope from the 
		// row scope
		DEBUG_START_TIMER(_T("Getting local scope ...") );
		wxString visibleScope = LanguageST::Get()->GetScope(scope, wxEmptyString);
		DEBUG_STOP_TIMER()

		DEBUG_START_TIMER(_T("Processing local scope..") );
		localTags = TagsManagerST::Get()->ParseLocals(visibleScope);
		DEBUG_STOP_TIMER()

		// filter all non qualified names from the local scope,
		// consider flags (PartialMatch or ExactMatch)
		FilterResults(*localTags, name, tags, flags, &tmpMap);
		DEBUG_MSG(_T("Matched ") << static_cast<int>(tags.size()) << _T(" in local scope"))

		delete localTags;
	}

	wxString nameMatch;
	// Since '_' (underscore) is a vaild char for C++/C word, 
	// and it is also a wildcard in databases, we need to set it
	// with escape
	escapedName.Replace(_T("_"), _T("^_"));
	nameMatch << _T(" name like '") << escapedName << _T("%%' ESCAPE '^'");

	if( scopeName.IsEmpty() == false && scopeName != _T("<global>"))
	{
		wxString parents;

		parents << _T(" parent in ('<global>', '") << scopeName << _T("'");
		// Incase this parent inhertis from other parent, add it to the possible
		// parents
		TagEntry tag;
		wxString ss(scopeName);
		while( GetClassTagByName(ss, tag) )
		{
			if(tag.GetInherits().IsEmpty())
				break;

			parents << _T(", '") << tag.GetInherits() << _T("'");
			ss = tag.GetInherits();
		}

		parents << _T(") ");

		query << _T("select * from tags where ")
			  << parents 
			  << _T(" and ") << nameMatch
			  << _T(" order by name") ;
	}
	else
	{
		// If no scope is available, search for all classes, namespaces etc and globals
		query << _T("select * from tags  where") 
			  << _T("(")
			  << _T("(kind='struct' or kind='class' or kind='typedef' or kind='namespace' or kind='union' or kind='macro' or kind='variable') OR ")
			  << _T("((kind='function' or kind='prototype') AND parent='<global>') ")
			  << _T(")")
			  << _T(" AND ") << nameMatch
			  << _T(" order by name") ;
	}

	DEBUG_START_TIMER(_T("Quering database with the following sql: ") << query)
	wxSQLite3ResultSet rs = m_pDb->Query(query);
	DEBUG_STOP_TIMER();

	wxSQLite3ResultSet ex_rs;
	if( m_pExternalDb->IsOpen() )
	{
		DEBUG_START_TIMER(_T("Quering external database with the following sql: ") << query);
		ex_rs = m_pExternalDb->Query(query);
		DEBUG_STOP_TIMER();
	}

	// Add the database results
	DEBUG_START_TIMER(_T("Populating map with database results ... "));

	try
	{
		while( rs.NextRow() )
		{
			// Construct a TagEntry from the rescord set
			// if duplicate entries are allowed, add them directly to the tags output vector,
			// else add them to tmpMap (to remove duplicate entries)
			TagEntry tag(rs);

			if( allowDups )
				tags.push_back(tag);
			else
				tmpMap[tag.GetName()] = tag;
		}

		if( m_pExternalDb->IsOpen() )
		{
			// add results from external database to the workspace database
			while( ex_rs.NextRow() )
			{		
				// Construct a TagEntry from the rescord set
				// if duplicate entries are allowed, add them directly to the tags output vector,
				// else add them to tmpMap (to remove duplicate entries)
				TagEntry tag(ex_rs);

				if( allowDups )
					tags.push_back(tag);
				else
					tmpMap[tag.GetName()] = tag;
			}
		}
	}
	catch( wxSQLite3Exception& e)
	{
		DEBUG_MSG(e.GetMessage())
	}

	DEBUG_STOP_TIMER()

	if(false == allowDups)
	{
		// Fill up the results from the tmporary map into the result
		// tags vector
		tags.clear();
		std::map<wxString, TagEntry>::iterator iter = tmpMap.begin();
		for(; iter != tmpMap.end(); iter++)
			tags.push_back(iter->second);
	}

	// Sort the results base on their name
	DEBUG_START_TIMER(_T("Sorting result vector ..."))
	std::sort(tags.begin(), tags.end(), SDescendingSort());	
	DEBUG_STOP_TIMER()
}

void TagsManager::TagsByParent(const wxString& parent, std::vector<TagEntry> &tags, bool inheritedMembers)
{
	wxString sql;
	wxString tmp_parent(parent);

	sql << _T("select * from tags where parent='") << parent << _T("' order by name;");
	GetTagsBySQL(sql, tags, wxEmptyString, _T("~"));	// Skip destructors
	
	if( tags.empty() )
		return;		

	if( inheritedMembers == false )
		return;

	// Get our parent tags
	TagEntry tag;
	while( true )
	{
		if( GetClassTagByName(tmp_parent, tag) )
		{
			// Check to see if our parent has inheritance
			if(tag.GetInherits().IsEmpty() == false)
			{
				sql.Empty();
				sql << _T("select * from tags where parent='") << tag.GetInherits() << _T("';") ;
				GetTagsBySQL(sql, tags, wxEmptyString, _T("~")); // Skip destructors
				tmp_parent = tag.GetInherits();
			}
			else
				break;
		}
		else
			break;
	}

	// Remove duplicate entries
	std::map<wxString, TagEntry> tagmap;
	for(size_t i=0; i<tags.size(); i++)
		tagmap[tags[i].GetName()] = tags[i];

	// copy the map back to the vector
	tags.clear();
	std::map<wxString, TagEntry>::iterator iter = tagmap.begin();
	for(; iter != tagmap.end(); iter++)
		tags.push_back(iter->second);

	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::GetTagsBySQL(const wxString& sql, std::vector<TagEntry> &tags, const wxString &kindToFilter, const wxString& excludePrefix)
{
	wxSQLite3ResultSet rs = m_pDb->Query( sql );

	try{
		while( rs.NextRow() )
		{
			TagEntry entry( rs );
			if(excludePrefix.IsEmpty() == false && entry.GetName().StartsWith(excludePrefix.GetData()))
				continue;

			// Do we need filter out a tag by kind?
			if(entry.GetKind() == kindToFilter)
				continue;

			tags.push_back( entry );
		}
	}
	catch(wxSQLite3Exception &e)
	{
		DEBUG_MSG(e.GetMessage())
	}

	// We have an external database as well, use it
	if( m_pExternalDb->IsOpen() )
	{
		try
		{
			rs = m_pExternalDb->Query( sql );
			while( rs.NextRow() )
			{
				TagEntry entry( rs );
				if(excludePrefix.IsEmpty() == false && entry.GetName().StartsWith(excludePrefix.GetData()))
					continue;

				// Do we need filter out a tag by kind?
				if(entry.GetKind() == kindToFilter)
					continue;

				tags.push_back( entry );
			}
		}
		catch(wxSQLite3Exception &e)
		{
			DEBUG_MSG(e.GetMessage())
		}
	}
}

bool TagsManager::GetClassTagByName(const wxString& name, TagEntry &tag)
{
	// Note that in this function we are assuming that the first match is our 
	// tag, if we have more, then we should have return false

	// Get our parent tags
	wxString sql;
	sql << _T("select * from tags where name='") << name << _T("' and (kind='struct' or kind='class' or kind='union' or kind='typedef') order by name;");

	try
	{
		wxSQLite3ResultSet rs = m_pDb->Query( sql );
		if(rs.NextRow())
		{
			TagEntry entry(rs);
			tag = entry;
			return true;
		}

		// if we are here, it means no match was found in the workspace
		// database, try the external database
		if( m_pExternalDb->IsOpen() )
		{
			rs = m_pExternalDb->Query( sql );
			if(rs.NextRow())
			{
				TagEntry entry(rs);
				tag = entry;
				return true;
			}
		}
	}
	catch(wxSQLite3Exception &e)
	{
		DEBUG_MSG(e.GetMessage())
	}


	return false;
}

bool TagsManager::AutoCompleteCandidates(const wxString& expr, const wxString& scope, const wxString& scopeName, std::vector<TagEntry>& candidates)
{
	candidates.clear();
	wxString path;
	wxString tagName = LanguageST::Get()->ProcessExpression(expr, path, false, scope, scopeName);
	if( tagName.IsEmpty() == false )
		TagsByParent(tagName, candidates, true);
	return candidates.empty() == false;
}

void TagsManager::FindSymbol(const wxString& name, std::vector<TagEntry> &tags)
{
	wxString query;
	query << _T("select * from tags where name='") 
		<< name 
		<< _T("' and kind != 'project';");

	try
	{
		wxSQLite3ResultSet rs = m_pDb->Query(query);

		// Add the database results
		while( rs.NextRow() )
		{
			// Construct a TagEntry from the rescord set
			// if duplicate entries are allowed, add them directly to the tags output vector,
			// else add them to tmpMap (to remove duplicate entries)
			TagEntry tag(rs);
			tags.push_back(tag);
		}

		// if we have an external database open, add its results to the workspace result
		if( m_pExternalDb->IsOpen() )
		{
			rs = m_pExternalDb->Query( query );
			while( rs.NextRow() )
			{
				TagEntry tag(rs);
				tags.push_back(tag);
			}
		}
	}
	catch(wxSQLite3Exception &e)
	{
		DEBUG_MSG(e.GetMessage())
	}
}

void TagsManager::DeleteProject(const wxString& projectName)
{
	wxString query;
	query << _T("delete from tags where project='") << projectName << _T("'");
	m_pDb->Begin();
	m_pDb->ExecuteUpdate(query);
	m_pDb->Commit();
}

void TagsManager::BuildExternalDatabase(const wxFileName& rootDir, 
										const wxFileName &dbName, 
										const wxString& WXUNUSED(language), 
										wxWindow* updateDlgParent/** null */)
{
	// check that root dir exist
	if(!wxDir::Exists(rootDir.GetFullPath()))
		return;

	// Find all files under this directory
	DirTraverser traverser;
	wxString tags;
	wxProgressDialog* prgDlg = NULL;

	wxDir dir(rootDir.GetFullPath());
	dir.Traverse(traverser);

	// Create a progress dialog
	if( updateDlgParent )
	{
		prgDlg = new wxProgressDialog (_("Building tags database ..."), wxEmptyString, static_cast<int>(traverser.GetFiles().size())+10, updateDlgParent, wxPD_APP_MODAL |	wxPD_SMOOTH | wxPD_AUTO_HIDE);
		prgDlg->SetSize(wxSize(800, -1));
		prgDlg->Layout();
		prgDlg->Centre();
	}
	
	int maxVal = static_cast<int>(traverser.GetFiles().size());
	int i = 0;
	for(i=0; i<maxVal; i++)
	{
		// Parse all source file, and concatenate them into one big string
		wxString fileTags;
		wxFileName curFile((traverser.GetFiles())[i]);
		wxString ext = curFile.GetExt();

		// we parse only valid C/C++ files
		if(m_validExtensions.find(ext.MakeLower()) == m_validExtensions.end())
			continue;		

		// update the progress bar
		if( prgDlg )
		{
			wxString msg;
			msg << wxT("File:\n") << curFile.GetFullPath();
			prgDlg->Update(i, msg);
		}

		SourceToTags(curFile, tags, m_ctags);
		tags << fileTags;
	}

	std::vector<TagEntry>* vec = VectorFromTags(tags, false);
	
	if( prgDlg )
	{
		wxString msg;
		msg << _("Updating database this can take a while ...");
		prgDlg->Update(maxVal, msg);
	}

	// create a trasaction and insert all info into the database
	// we dont use m_pExternalDb since it is on memory only, so 
	// we use a temproary database
	TagsDatabase db;
	db.Store(*vec, dbName);
	delete vec;

	if( prgDlg )
	{
		wxString msg;
		msg << _("Done");
		prgDlg->Update(maxVal+10, msg);
		prgDlg->Destroy();
	}
}

void TagsManager::OpenExternalDatabase(const wxFileName &dbName)
{
	// check that the database exist
	if(!wxFile::Exists(dbName.GetFullPath()))
		return;

	// load it to memory
	m_pExternalDb->LoadToMemory(dbName);
}

void TagsManager::GetHoverTip(const wxString & token, const wxString & scope, const wxString & scopeName, bool isFunc, std::vector<wxString> &tips)
{
	LanguageST::Get()->GetHoverTip(token, scope, scopeName, isFunc, tips);
}

CallTipPtr TagsManager::GetFunctionTip(const wxString &expr, const wxString & scope, const wxString & scopeName )
{
	// display call tip with function prototype
	std::vector<wxString> tips;
	wxString expression( expr );

	wxString parent; ///< contains the parent of the function
	if( LanguageST::Get()->ProcessExpression(expression, parent, true, scope, scopeName).IsEmpty() == false )
	{
		// Get the function(s) signatures from database
		// Trim whitespace from right and left
		static wxString trimString(_T("({};\r\n\t\v "));

		expression.erase(0, expression.find_first_not_of(trimString)); 
		expression.erase(expression.find_last_not_of(trimString)+1);

		wxArrayString dl;
		dl.Add(_T("::"));
		dl.Add(_T("->"));
		dl.Add(_T("."));
		StringTokenizer tok( expression, dl );
		wxString funcName = tok.Last();

		wxString sql;
		std::vector<TagEntry> tags;
		if( parent.IsEmpty() )
			sql << _T("SELECT * FROM TAGS WHERE name='") << funcName << _T("' AND (parent='<global>' OR parent='") << scopeName << _T("') AND (kind='function' OR kind='prototype')");
		else
			sql << _T("SELECT * FROM TAGS WHERE name='") << funcName << _T("' AND parent='") << parent << _T("' AND (kind='function' OR kind='prototype')");
		TagsManagerST::Get()->GetTagsBySQL( sql, tags );

		if( tags.size() > 0 )
		{
			// we have a match
			for(size_t i=0; i<tags.size(); i++)
			{
				wxString tip;
				tip << tags[i].GetName() << _T(" ") << tags[i].GetSignature();
				tips.push_back( tip );
			}
		}
	}
	CallTipPtr ct( new CallTip(tips) );
	return ct;
}

void TagsManager::FilterResults(const std::vector<TagEntry> & src, const wxString & name, std::vector<TagEntry> & target, int flags, std::map<wxString, TagEntry>* tmpMap)
{
	size_t i=0;
	for(i=0; i<src.size(); i++)
	{
		TagEntry e(src[i]);
		if( flags & PartialMatch )
		{
			if( name.IsEmpty() == false && e.GetName().StartsWith(name.GetData()) )
			{
				target.push_back( e );
				if( tmpMap )
					(*tmpMap) [ e.GetName() ] = e;
			}
		}
		else
		{
			if( e.GetName() == name )
			{
				target.push_back( e );
				if( tmpMap )
					(*tmpMap) [ e.GetName() ] = e;
			}
		}
	}
}

void TagsManager::ParseComments(const bool parse)
{
	wxCriticalSectionLocker locker( m_cs );
	m_parseComments = parse;
}

wxString TagsManager::GetComment(const wxString &file, const int line)
{
	wxString sql;
	// the comments is searched one line above the requested line
	sql << _T("SELECT * FROM COMMENTS WHERE file='") 
		<< file << _T("' AND line=") << line - 1;

	try
	{
		wxSQLite3ResultSet rs = m_pDb->Query( sql );
		if( rs.NextRow() )
		{
			Comment comment( rs );
			return comment.GetComment();
		}

		// try the external database
		if( m_pExternalDb->IsOpen() )
		{
			wxSQLite3ResultSet rs = m_pExternalDb->Query( sql );
			if( rs.NextRow() )
			{
				Comment comment( rs );
				return comment.GetComment();
			}
		}
	}
	catch( wxSQLite3Exception &e )
	{
		DEBUG_MSG(e.GetMessage())
	}

	return wxEmptyString;
}

void TagsManager::StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store( comments, path );
}

