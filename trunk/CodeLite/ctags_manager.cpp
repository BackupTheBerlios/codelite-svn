#include "precompiled_header.h"
#include "ctags_manager.h"
#include <wx/txtstrm.h>
#include <wx/file.h>
#include <algorithm>
#include "language.h"
#include <wx/progdlg.h>
#include "dirtraverser.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Descending sorting function
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

//------------------------------------------------------------------------------
// CtagsOptions
//------------------------------------------------------------------------------
wxString CtagsOptions::ToString() const
{
	wxString options(wxEmptyString);
	if(GetLanguage().IsEmpty() == false){
		options += wxT(" --language-force=");
		options += GetLanguage();
		options += wxT(" ");
	}

	if(GetIgnoreMacros().IsEmpty() == false){
		bool first = true;
		options += wxT("-I");
		wxStringTokenizer tkz(GetIgnoreMacros(), wxT(","));
		while( tkz.HasMoreTokens() ){
			wxString token = tkz.NextToken();
			token = token.Trim();
			token = token.Trim(false);

			if(token.Find(wxT("=")) != wxNOT_FOUND){
				// we found a MACRO=VALUE
				wxString name = token.BeforeFirst(wxT('='));
				wxString value = token.AfterFirst(wxT('='));

				// construct new entry
				if(!first) { 
					options += wxT(","); 
				}else{
					first = false;
				}

				options += name;
				options += wxT("=");
				options += value;
			} else if(token.Find(wxT(" "))!= wxNOT_FOUND){
				// Skip macro with a space in its name
				continue;
			} else {
				// add the entry as is
				if(!first) { 
					options += wxT(","); 
				}else{
					first = false;
				}

				options += token;
			}
		}
	}

	return options;
}

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------

TagsManager::TagsManager() : wxEvtHandler()
, m_ctagsPath(wxT("ctags-le"))
, m_ctags(NULL)
, m_localCtags(NULL)
, m_parseComments(false)
{
	m_pDb = new TagsDatabase();
	m_pExternalDb = new TagsDatabase(true);	// use it as memory database

	// Initialise ctags command pattern
	m_ctagsCmd[TagsLocal] =  wxT("  --fields=aKmSsnit --c-kinds=+l --C++-kinds=+l  --filter=yes --filter-terminator=\"<<EOF>>\" ");
	m_ctagsCmd[TagsGlobal] =  wxT("  --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p --filter=yes --filter-terminator=\"<<EOF>>\"  ");
}  

TagsManager::~TagsManager()
{
	delete m_pDb;
	delete m_pExternalDb;

	if(m_ctags)	m_ctags->Disconnect(m_ctags->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	if(m_localCtags) m_localCtags->Disconnect(m_localCtags->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);

	// terminate ctags processes
	if(m_localCtags) m_localCtags->Terminate();
	if(m_ctags) m_ctags->Terminate();
}

void TagsManager::OpenDatabase(const wxFileName& fileName){
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
		return TagTreePtr( NULL );
	}

	// Load the records and build a language tree
	TagEntry root;
	root.SetName(wxT("<ROOT>"));

	TagTreePtr tree( new TagTree(wxT("<ROOT>"), root) );
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
	wxString pattern, kind;
	wxString strLine = line;
	long lineNumber = wxNOT_FOUND;
	std::map<wxString, wxString> extFields;

	//get the token name
	wxString name = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	//get the file name
	wxString fileName = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	//here we can get two options:
	//pattern followed by ;"
	//or
	//line number followed by ;"
	int end = strLine.Find(wxT(";\""));
	if(end == wxNOT_FOUND){
		//invalid pattern found
		return;
	}

	if(strLine.StartsWith(wxT("/^"))){
		//regular expression pattern found
		pattern = strLine.Mid(0, end);
		strLine	= strLine.Right(strLine.Length() - (end + 2));
	}
	else
	{
		//line number pattern found, this is usually the case when
		//dealing with macros in C++
		pattern = strLine.Mid(0, end);
		strLine	= strLine.Right(strLine.Length() - (end + 2));

		pattern = pattern.Trim();
		pattern = pattern.Trim(false);
		pattern.ToLong(&lineNumber);
	}

	//next is the kind of the token
	if(strLine.StartsWith(wxT("\t"))){
		strLine	= strLine.AfterFirst(wxT('\t'));
	}

	kind = strLine.BeforeFirst(wxT('\t'));
	strLine	= strLine.AfterFirst(wxT('\t'));

	if(strLine.IsEmpty() == false)
	{
		wxStringTokenizer tkz(strLine, wxT('\t'));
		while(tkz.HasMoreTokens())
		{
			wxString token = tkz.NextToken();
			wxString key = token.BeforeFirst(wxT(':'));
			wxString val = token.AfterFirst(wxT(':'));

			if(key == wxT("line") && !val.IsEmpty())
			{
				val.ToLong(&lineNumber);
			}
			else
			{
				extFields[key] = val;
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
	root.SetName(wxT("<ROOT>"));

	TagTreePtr tree( new TagTree(wxT("<ROOT>"), root) );

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
	wxString tags;
	if( !m_ctags ){
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
	proj.SetKind(wxT("project"));

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
	
	wxString tmpFileName = wxFileName::CreateTempFileName(wxT("CtagsInputFile"));
	wxFileName fn(tmpFileName);
	fn.MakeAbsolute();
	size_t i=0;

	//create a file with list of files for ctags to process
	wxFileOutputStream out(fn.GetFullPath());
	wxTextOutputStream text( out );

	for(; i<fpArr.size(); i++){
		wxFileName file(fpArr[i]);
		file.MakeAbsolute();
		text << file.GetFullPath() << wxT("\n");
	}
	out.Close();

	//start ctags and tell him to parse files from the temporary file
	//get ctags execution line
	wxString cmd;
	
	// Get ctags flags from the map
	wxString ctagsCmd;
	ctagsCmd << m_options.ToString() << wxT(" --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ");
	
	// build the command, we surround ctags name with double quatations
	cmd << wxT("\"") << m_ctagsPath.GetFullPath() << wxT("\"") << ctagsCmd 
		<< wxT(" -f -") // send output to stdout
		<< wxT(" -L") 
#ifdef __WXMSW__
		<< wxT("\"") << fn.GetFullPath() << wxT("\"");	//read input files from temp file
#else
		<< fn.GetFullPath();							//read input files from temp file
#endif

	//run ctags in sync mode
	wxArrayString stdoutArr, stderrArr;
	wxExecute(cmd, stdoutArr, stderrArr);

	//print to stderr the errors received from ctags
	wxFFileOutputStream output( stderr );
	wxTextOutputStream cout( output );
	
	for(i=0; i<stderrArr.GetCount(); i++){
		cout << stderrArr.Item(i);
	}

	for(i=0; i<stdoutArr.GetCount(); i++){
		tags += stdoutArr.Item(i);
		tags += wxT("\n");
	}

	for(i=0; i<fpArr.size(); i++){
		if( comments && m_parseComments ){
			// parse comments
			LanguageST::Get()->ParseComments( fpArr[i], comments );
		}
	}
	
	return TagTreePtr( TreeFromTags(tags, project) );
}

std::vector<TagEntry>* TagsManager::ParseLocals(const wxString& scope)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	wxString tags;

	if( !m_localCtags )
	{
		return NULL;
	}

	// Put the content into temporary file
	wxFileName fp(wxGetCwd(), wxT("temp_source_file"));
	
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
		if(onlyLocals && tag.GetKind() == wxT("local"))
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
		wxLogMessage(wxT("Loading symbols..."));
		
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByProject(project, path);

		// Load the records and build a language tree
		TagEntry root;
		std::vector<TagEntry> rejectedTags;
		root.SetName(wxT("<ROOT>"));
		tree.Reset( new TagTree(wxT("<ROOT>"), root) );
		while( rs.NextRow() )
		{
			TagEntry entry(rs);
			tree->AddEntry(entry);
		}
		rs.Finalize();
	}
	catch (wxSQLite3Exception& e)
	{
		wxLogMessage(wxT("Caught an exception:") + e.GetMessage());
		wxUnusedVar(e);
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

clProcess *TagsManager::StartCtagsProcess(int kind)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Run ctags process
	wxString cmd;
	
	// Get ctags flags from the map
	std::map<int, wxString>::iterator iter = m_ctagsCmd.find(kind);
	if( iter == m_ctagsCmd.end() ){
		return NULL;
	}
	
	wxString ctagsCmd;
	ctagsCmd << m_options.ToString() << wxT(" ") << iter->second;
	
	// build the command, we surround ctags name with double quatations
	cmd << wxT("\"") << m_ctagsPath.GetFullPath() << wxT("\"") << ctagsCmd;
	clProcess* process;

	process = new clProcess(wxNewId(), cmd);

	// Launch it!
	process->Start();
	process->SetType(kind);
	
	m_processes[process->GetPid()] = process;

	if( process->GetPid() <= 0 ){
		kind == TagsGlobal ? m_ctags = NULL : m_localCtags = NULL;
		return NULL;
	}

	// attach the termination event to the tags manager class
	process->Connect(process->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	kind == TagsGlobal ? m_ctags = process : m_localCtags = process;
	return process;
}

void TagsManager::RestartCtagsProcess(int kind)
{
	clProcess *oldProc(NULL);
	if(kind == TagsLocal){
		oldProc = m_localCtags;
	} else if(kind == TagsGlobal){
		oldProc = m_ctags;
	}

	if( !oldProc ){
		return ;
	}

	// no need to call StartCtagsProcess(), since it will be called automatically
	// by the termination handler OnCtagsEnd()
	oldProc->Terminate();
}

void TagsManager::SetCtagsPath(const wxString& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_ctagsPath = wxFileName(path, wxT("ctags-le"));
}

void TagsManager::OnCtagsEnd(wxProcessEvent& event)
{
	//-----------------------------------------------------------
	// This event handler is a must if you wish to delete 
	// the process and prevent memory leaks
	// In addition, I implemented here some kind of a watchdog
	// mechanism: if ctags process terminated abnormally, it will
	// be restarted automatically by this function (unless the 
	// termination of it was from OnClose() function, then we 
	// choose to ignore the restart)
	//-----------------------------------------------------------

	// Which ctags process died?
	std::map<int, clProcess*>::iterator iter = m_processes.find(event.GetPid());
	if( iter != m_processes.end()){
		clProcess *proc = iter->second;
		proc->Disconnect(proc->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
		// start new process
		StartCtagsProcess((int)proc->GetType());
		// delete the one that just terminated
		delete proc;
		// remove it from the map
		m_processes.erase(iter);
	}
}

//---------------------------------------------------------------------
// Parsing
//---------------------------------------------------------------------
void TagsManager::SourceToTags(const wxFileName& source, wxString& tags, clProcess *ctags)
{
	// Send the command to ctags
	wxOutputStream *out = ctags->GetOutputStream();
	if( out ) 
	{
		wxString cmd(source.GetFullPath());
		cmd += wxT("\n");

		const wxCharBuffer pWriteData = _C(cmd);
		out->Write(pWriteData.data(), cmd.Length());
	}
	else
	{
		return;
	}
	
	// Read ctags output
	while( true )
	{
		if( ctags->IsInputAvailable() )
		{
			wxTextInputStream in(*ctags->GetInputStream());
			wxString msg;
			msg << in.ReadLine() << wxT("\n");
			if( msg.Contains(wxT("<<EOF>>")) )
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
	root.SetName(wxT("<ROOT>"));

	TagTreePtr tree( new TagTree(wxT("<ROOT>"), root) );

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
		if( tag.GetKind() != wxT("local") )
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
	query << wxT("select * from tags where line <=") << lineNo << wxT(" AND  project='") << project << wxT("' and file='") << fileName << wxT("' and kind='function' order by line DESC");

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
		wxUnusedVar(e);
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
		wxString visibleScope = LanguageST::Get()->GetScope(scope, wxEmptyString);
		localTags = TagsManagerST::Get()->ParseLocals(visibleScope);
		if( !localTags )
		{
			return;
		}

		// filter all non qualified names from the local scope,
		// consider flags (PartialMatch or ExactMatch)
		FilterResults(*localTags, name, tags, flags, &tmpMap);
		
		delete localTags;
	}

	wxString nameMatch;
	// Since '_' (underscore) is a vaild char for C++/C word, 
	// and it is also a wildcard in databases, we need to set it
	// with escape
	escapedName.Replace(wxT("_"), wxT("^_"));
	nameMatch << wxT(" name like '") << escapedName << wxT("%%' ESCAPE '^'");

	if( scopeName.IsEmpty() == false && scopeName != wxT("<global>"))
	{
		wxString parents;

		parents << wxT(" parent in ('<global>', '") << scopeName << wxT("'");
		// Incase this parent inhertis from other parent, add it to the possible
		// parents
		TagEntry tag;
		wxString ss(scopeName);
		while( GetClassTagByName(ss, tag) )
		{
			if(tag.GetInherits().IsEmpty())
				break;

			parents << wxT(", '") << tag.GetInherits() << wxT("'");
			ss = tag.GetInherits();
		}

		parents << wxT(") ");

		query << wxT("select * from tags where ")
			  << parents 
			  << wxT(" and ") << nameMatch
			  << wxT(" order by name") ;
	}
	else
	{
		// If no scope is available, search for all classes, namespaces etc and globals
		query << wxT("select * from tags  where") 
			  << wxT("(")
			  << wxT("(kind='struct' or kind='class' or kind='typedef' or kind='namespace' or kind='union' or kind='macro' or kind='variable') OR ")
			  << wxT("((kind='function' or kind='prototype') AND parent='<global>') ")
			  << wxT(")")
			  << wxT(" AND ") << nameMatch
			  << wxT(" order by name") ;
	}

	wxSQLite3ResultSet rs = m_pDb->Query(query);
	
	wxSQLite3ResultSet ex_rs;
	if( m_pExternalDb->IsOpen() )
	{
		ex_rs = m_pExternalDb->Query(query);
	}

	// Add the database results
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
		wxUnusedVar(e);
	}

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
	std::sort(tags.begin(), tags.end(), SDescendingSort());	
}

void TagsManager::TagsByParent(const wxString& parent, std::vector<TagEntry> &tags, bool inheritedMembers)
{
	wxString sql;
	wxString tmp_parent(parent);

	sql << wxT("select * from tags where parent='") << parent << wxT("' order by name;");
	GetTagsBySQL(sql, tags, wxEmptyString, wxT("~"));	// Skip destructors
	
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
				sql << wxT("select * from tags where parent='") << tag.GetInherits() << wxT("';") ;
				GetTagsBySQL(sql, tags, wxEmptyString, wxT("~")); // Skip destructors
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
		wxUnusedVar(e);
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
			wxUnusedVar(e);
		}
	}
}

bool TagsManager::GetClassTagByName(const wxString& name, TagEntry &tag)
{
	// Note that in this function we are assuming that the first match is our 
	// tag, if we have more, then we should have return false

	// Get our parent tags
	wxString sql;
	sql << wxT("select * from tags where name='") << name << wxT("' and (kind='struct' or kind='class' or kind='union' or kind='typedef') order by name;");

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
		wxUnusedVar(e);
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
	query << wxT("select * from tags where name='") 
		<< name 
		<< wxT("' and kind != 'project';");

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
		wxUnusedVar(e);
	}
}

void TagsManager::DeleteProject(const wxString& projectName)
{
	wxString query;
	query << wxT("delete from tags where project='") << projectName << wxT("'");
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
	DirTraverser traverser(m_options.GetFileSpec());
	
	wxString tags;
	wxProgressDialog* prgDlg = NULL;

	wxDir dir(rootDir.GetFullPath());
	dir.Traverse(traverser);

	// Create a progress dialog
	if( updateDlgParent )
	{
		prgDlg = new wxProgressDialog (wxT("Building tags database ..."), wxEmptyString, static_cast<int>(traverser.GetFiles().size())+10, updateDlgParent, wxPD_APP_MODAL |	wxPD_SMOOTH | wxPD_AUTO_HIDE);
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
		
		// update the progress bar
		if( prgDlg ){
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
		msg << wxT("Updating database this can take a while ...");
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
		msg << wxT("Done");
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

clCallTipPtr TagsManager::GetFunctionTip(const wxString &expr, const wxString & scope, const wxString & scopeName )
{
	// display call tip with function prototype
	std::vector<wxString> tips;
	wxString expression( expr );

	wxString parent; ///< contains the parent of the function
	if( LanguageST::Get()->ProcessExpression(expression, parent, true, scope, scopeName).IsEmpty() == false )
	{
		// Get the function(s) signatures from database
		// Trim whitespace from right and left
		static wxString trimString(wxT("({};\r\n\t\v "));

		expression.erase(0, expression.find_first_not_of(trimString)); 
		expression.erase(expression.find_last_not_of(trimString)+1);

		wxArrayString dl;
		dl.Add(wxT("::"));
		dl.Add(wxT("->"));
		dl.Add(wxT("."));
		StringTokenizer tok( expression, dl );
		wxString funcName = tok.Last();

		wxString sql;
		std::vector<TagEntry> tags;
		if( parent.IsEmpty() )
			sql << wxT("SELECT * FROM TAGS WHERE name='") << funcName << wxT("' AND (parent='<global>' OR parent='") << scopeName << wxT("') AND (kind='function' OR kind='prototype')");
		else
			sql << wxT("SELECT * FROM TAGS WHERE name='") << funcName << wxT("' AND parent='") << parent << wxT("' AND (kind='function' OR kind='prototype')");
		TagsManagerST::Get()->GetTagsBySQL( sql, tags );

		if( tags.size() > 0 )
		{
			// we have a match
			for(size_t i=0; i<tags.size(); i++)
			{
				wxString tip;
				tip << tags[i].GetName() << wxT(" ") << tags[i].GetSignature();
				tips.push_back( tip );
			}
		}
	}
	clCallTipPtr ct( new clCallTip(tips) );
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
	sql << wxT("SELECT * FROM COMMENTS WHERE file='") 
		<< file << wxT("' AND line=") << line - 1;

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
		wxUnusedVar(e);
	}

	return wxEmptyString;
}

void TagsManager::StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store( comments, path );
}

