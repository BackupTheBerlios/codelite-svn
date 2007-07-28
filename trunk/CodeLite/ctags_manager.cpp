#include "precompiled_header.h"
#include "ctags_manager.h"
#include <wx/txtstrm.h>
#include <wx/file.h>
#include <algorithm>
#include <wx/progdlg.h>
#include "dirtraverser.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include "cpp_comment_creator.h"
#include "tags_options_data.h"
#include <wx/busyinfo.h>

#define PRINT_START_MESSAGE(msg)\
	{\
		m_watch = wxStopWatch();\
		wxString logmsg;\
		logmsg << msg;\
		wxLogMessage(logmsg);\
	}

#define PRINT_END_MESSAGE(msg)\
	{\
		wxString logmsg;\
		logmsg << msg << wxT(" Time elapsed: ") << m_watch.Time();\
		wxLogMessage(logmsg);\
	}

// Descending sorting function
struct SDescendingSort
{
	bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd)
	{
		return rStart->GetName().Cmp(rEnd->GetName()) > 0;
	}
};

/// Ascending sorting function
struct SAscendingSort
{
	bool operator()(const TagEntryPtr &rStart, const TagEntryPtr &rEnd)
	{
		return rEnd->GetName().Cmp(rStart->GetName()) > 0;
	}
};

struct tagParseResult {
	TagTreePtr tree;
	std::vector<DbRecordPtr> *comments;
};

//------------------------------------------------------------------------------
// CTAGS Manager
//------------------------------------------------------------------------------

TagsManager::TagsManager() : wxEvtHandler()
, m_ctagsPath(wxT("ctags-le"))
, m_ctags(NULL)
{
	m_pDb = new TagsDatabase();
	m_pExternalDb = new TagsDatabase();
	m_ctagsCmd[TagsGlobal] =  wxT("  --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p --filter=yes --filter-terminator=\"<<EOF>>\"  ");
}  

TagsManager::~TagsManager()
{
	delete m_pDb;
	delete m_pExternalDb;

	if(m_ctags)	m_ctags->Disconnect(m_ctags->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	// terminate ctags process
	if(m_ctags) m_ctags->Terminate();
}

void TagsManager::OpenDatabase(const wxFileName& fileName){
	m_pDb->OpenDatabase(fileName);
}

// Currently not in use, maybe in the future??
TagTreePtr TagsManager::ParseTagsFile(const wxFileName& fp)
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
		TagEntry tag( entry );
		tree->AddEntry(tag);
	}
	tagsClose(file);
	return tree;
}

void TagsManager::TagFromLine(const wxString& line, TagEntry& tag)
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

	tag.Create(fileName, name, lineNumber, pattern, kind, extFields);
}

TagTreePtr TagsManager::ParseSourceFile(const wxFileName& fp, std::vector<DbRecordPtr> *comments)
{
	wxString tags;
	if( !m_ctags ){
		return TagTreePtr( NULL );
	}

	SourceToTags(fp, tags, m_ctags);

	//	return ParseTagsFile(tags, project);
	TagTreePtr ttp = TagTreePtr( TreeFromTags(tags) );

	if( comments && GetParseComments() )
	{
		// parse comments
		LanguageST::Get()->ParseComments( fp, comments );
		
	}
	return ttp;
}

TagTreePtr TagsManager::ParseSourceFiles(const std::vector<wxFileName> &fpArr, std::vector<DbRecordPtr> *comments)
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
		if( comments && GetParseComments() ){
			// parse comments
			LanguageST::Get()->ParseComments( fpArr[i], comments );
		}
	}
	
	return TagTreePtr( TreeFromTags(tags) );
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

TagTreePtr TagsManager::Load(const wxFileName& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	// Incase empty file path is provided, use the current file name
	TagTreePtr tree;
	try	
	{
		wxLogMessage(wxT("Loading symbols..."));
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByFile(fileName.GetFullPath());

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

void TagsManager::Delete(const wxFileName& path, const wxString& fileName)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->DeleteByFileName(path, fileName);
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
		m_ctags = NULL;
		return NULL;
	}

	// attach the termination event to the tags manager class
	process->Connect(process->GetUid(), wxEVT_END_PROCESS, wxProcessEventHandler(TagsManager::OnCtagsEnd), NULL, this);
	m_ctags = process;
	return process;
}

void TagsManager::RestartCtagsProcess(int kind)
{
	clProcess *oldProc(NULL);
	if(kind == TagsGlobal){
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

TagTreePtr TagsManager::TreeFromTags(const wxString& tags)
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
		TagFromLine(line, tag);

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

bool TagsManager::IsValidCtagsFile(const wxFileName &filename) const
{
	bool is_ok(false);
	// Put a request on the parsing thread to update the GUI tree & the database
	wxString filespec = GetCtagsOptions().GetFileSpec();

	//if the file spec matches the current file, notify ctags
	wxStringTokenizer tkz(filespec, wxT(";"));
	while(tkz.HasMoreTokens())
	{
		wxString spec = tkz.NextToken();
		if(wxMatchWild(spec, filename.GetFullName()))
		{
			is_ok = true;
			break;
		}
	} // while(tkz.HasMoreTokens())
	return is_ok;
}

//-----------------------------------------------------------------------------
// >>>>>>>>>>>>>>>>>>>>> Code Completion API START
//-----------------------------------------------------------------------------

void TagsManager::TagsByScopeAndName(const wxString& scope, const wxString &name, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scope);
	GetDerivationList(scope, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);
	wxString tmpName(name);

	for(size_t i=0; i<derivationList.size(); i++)
	{
		PRINT_START_MESSAGE(wxT("TagsByScopeAndName::Iterating"));
		sql.Empty();
		tmpName.Replace(wxT("_"), wxT("^_"));
		sql << wxT("select * from tags where scope='") << derivationList.at(i) << wxT("' and name like '") << tmpName << wxT("%%' ESCAPE '^'");
		DoExecuteQueury(sql, tags);
		PRINT_END_MESSAGE(wxT("TagsByScopeAndName::Iterating ended"));
	} // for(size_t i=0; i<derivationList.size(); i++)
	// and finally sort the results

	PRINT_START_MESSAGE(wxT("TagsByScopeAndName::Sorting"));
	std::sort(tags.begin(), tags.end(), SAscendingSort());
	PRINT_END_MESSAGE(wxT("TagsByScopeAndName::Sorting ended"));
}

void TagsManager::TagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scope);
	GetDerivationList(scope, derivationList);

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);

	for(size_t i=0; i<derivationList.size(); i++)
	{
		PRINT_START_MESSAGE(wxT("TagsByScope::Iterating"));
		sql.Empty();
		wxString tmpScope(derivationList.at(i));
		sql << wxT("select * from tags where scope='") << tmpScope << wxT("'");
		DoExecuteQueury(sql, tags);
		PRINT_END_MESSAGE(wxT("TagsByScope::Iterating ended"));
	}
	// and finally sort the results

	PRINT_START_MESSAGE(wxT("TagsByScope::Sorting"));
	std::sort(tags.begin(), tags.end(), SAscendingSort());
	PRINT_END_MESSAGE(wxT("TagsByScope::Sorting ended"));

}

bool TagsManager::WordCompletionCandidates(const wxString& expr, const wxString& text, const wxString &word, std::vector<TagEntryPtr> &candidates)
{
	candidates.clear();
	wxString path, tmp;
	wxString typeName, typeScope;

	//remove the word from the expression
	wxString expression(expr);
	
	// Trim whitespace from right and left
	static wxString trimString(wxT("!<>=(){};\r\n\t\v "));

	expression.erase(0, expression.find_first_not_of(trimString)); 
	expression.erase(expression.find_last_not_of(trimString)+1);
	tmp = expression;
	expression.EndsWith(word, &tmp);
	expression = tmp;
	
	wxString funcSig;
	wxString scope = LanguageST::Get()->GetScope(text);
	wxString scopeName = LanguageST::Get()->GetScopeName(scope, NULL, &funcSig);
	if(expression.IsEmpty()){
		//collect all the tags from the current scope, and 
		//from the global scope
		std::vector<TagEntryPtr> tmpCandidates;
		GetGlobalTags(word, tmpCandidates);
		GetLocalTags(word, scope, tmpCandidates);
		GetLocalTags(word, funcSig, tmpCandidates);
		TagsByScopeAndName(scopeName, word, tmpCandidates);
		RemoveDuplicates(tmpCandidates, candidates);
	}else{
		wxString typeName, typeScope;
		bool res = LanguageST::Get()->ProcessExpression(expression, text, typeName, typeScope);
		if(!res){
			return false;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if(typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScope(scope, tmpCandidates);
		RemoveDuplicates(tmpCandidates, candidates);
	}
	return true;
}

bool TagsManager::AutoCompleteCandidates(const wxString& expr, const wxString& text, std::vector<TagEntryPtr>& candidates)
{
	candidates.clear();
	wxString path;
	wxString typeName, typeScope;

	wxString expression(expr);
	static wxString trimString(wxT("(){};\r\n\t\v "));
	expression.erase(0, expression.find_first_not_of(trimString)); 
	expression.erase(expression.find_last_not_of(trimString)+1);
	
	PRINT_START_MESSAGE(wxT("ProcessExpression started..."));
	bool res = LanguageST::Get()->ProcessExpression(expression, text, typeName, typeScope);
	if(!res){
		return false;
	}

	PRINT_END_MESSAGE(wxT("Done"));
	//load all tags from the database that matches typeName & typeScope
	wxString scope;
	if(typeScope == wxT("<global>"))
		scope << typeName;
	else
		scope << typeScope << wxT("::") << typeName;

	//this function will retrieve the ineherited tags as well
	std::vector<TagEntryPtr> tmpCandidates;
	PRINT_START_MESSAGE(wxT("TagsByScope started..."));
	TagsByScope(scope, tmpCandidates);
	PRINT_END_MESSAGE(wxT("TagsByScope ended"));
	RemoveDuplicates(tmpCandidates, candidates);
	return candidates.empty() == false;
}

void TagsManager::RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	for(size_t i=0; i<src.size(); i++)
	{
		if(i == 0){
			target.push_back(src.at(0));
		}else{
			if(src.at(i)->GetName() != target.at(target.size()-1)->GetName()){
				target.push_back(src.at(i));
			}
		}
	}
}

void TagsManager::RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target)
{
	for(size_t i=0; i<src.size(); i++)
	{
		if(i == 0){
			target.push_back(src.at(0));
		}else{
			if(src.at(i)->GetSignature() != target.at(target.size()-1)->GetSignature()){
				target.push_back(src.at(i));
			}
		}
	}
}

void TagsManager::GetGlobalTags(const wxString &name, std::vector<TagEntryPtr> &tags, SearchFlags flags)
{
	wxString sql, tmpName;
	
	//make enough room for max of 500 elements in the vector
	tags.reserve(500);
	tmpName = name;
	if(flags == PartialMatch){
		tmpName.Replace(wxT("_"), wxT("^_"));
		sql << wxT("select * from tags where parent='<global>' and name like '") << tmpName << wxT("%%' ESCAPE '^'");
	}else{
		sql << wxT("select * from tags where parent='<global>' and name ='") << tmpName << wxT("'");
	}
	DoExecuteQueury(sql, tags);
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

void TagsManager::GetLocalTags(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags, SearchFlags flags)
{
	//collect tags from the current scope text
	LanguageST::Get()->GetLocalVariables(scope, tags, name, flags);
}

void TagsManager::GetHoverTip(const wxString & expr, const wxString &word, const wxString & text, std::vector<wxString> & tips)
{
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<TagEntryPtr> tmpCandidates, candidates;

	//remove the word from the expression
	wxString expression(expr);
	
	// Trim whitespace from right and left
	static wxString trimString(wxT("(){};\r\n\t\v "));

	expression.erase(0, expression.find_first_not_of(trimString)); 
	expression.erase(expression.find_last_not_of(trimString)+1);
	tmp = expression;
	expression.EndsWith(word, &tmp);
	expression = tmp;

	wxString scope = LanguageST::Get()->GetScope(text);
	wxString scopeName = LanguageST::Get()->GetScopeName(scope);
	if(expression.IsEmpty()){
		//collect all the tags from the current scope, and 
		//from the global scope
		
		GetGlobalTags(word, tmpCandidates, ExactMatch);
		GetLocalTags(word, scope, tmpCandidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, tmpCandidates);
		RemoveDuplicates(tmpCandidates, candidates);

		// we now have a list of tags that matches our word
		TipsFromTags(candidates, word, tips);
	}else{
		wxString typeName, typeScope;
		bool res = LanguageST::Get()->ProcessExpression(expression, text, typeName, typeScope);
		if(!res){
			return;
		}

		//get all symbols realted to this scope
		scope = wxT("");
		if(typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		std::vector<TagEntryPtr> tmpCandidates;
		TagsByScopeAndName(scope, word, tmpCandidates);
		RemoveDuplicatesTips(tmpCandidates, candidates);

		// we now have a list of tags that matches our word
		TipsFromTags(candidates, word, tips);
	}
}

clCallTipPtr TagsManager::GetFunctionTip(const wxString &expr, const wxString &text, const wxString &word)
{
	std::vector<TagEntryPtr> candidates;
	wxString path;
	wxString typeName, typeScope, tmp;
	std::vector<wxString> tips;

	// Trim whitespace from right and left
	static wxString trimString(wxT("(){};\r\n\t\v "));

	wxString expression(expr);
	expression.erase(0, expression.find_first_not_of(trimString)); 
	expression.erase(expression.find_last_not_of(trimString)+1);

	//remove the last token from the expression 
	expression.EndsWith(word, &tmp);
	expression = tmp;
	if(word.IsEmpty()){
		return NULL;
	}

	if(expression.IsEmpty())
	{
		//we are probably examining a global function, or a scope function
		PRINT_START_MESSAGE(wxT("GetFunctionTip is called for global or scoped function"));
		wxString scopeName = LanguageST::Get()->GetScopeName(text);
		GetGlobalTags(word, candidates, ExactMatch);
		TagsByScopeAndName(scopeName, word, candidates);
		GetFunctionTipFromTags(candidates, word, tips);
	}
	else
	{
		PRINT_START_MESSAGE(wxT("ProcessExpression started..."));
		bool res = LanguageST::Get()->ProcessExpression(expression, text, typeName, typeScope);
		if(!res){
			return false;
		}

		PRINT_END_MESSAGE(wxT("Done"));
		//load all tags from the database that matches typeName & typeScope
		wxString scope;
		if(typeScope == wxT("<global>"))
			scope << typeName;
		else
			scope << typeScope << wxT("::") << typeName;

		//this function will retrieve the ineherited tags as well
		std::vector<TagEntryPtr> tmpCandidates;
		PRINT_START_MESSAGE(wxT("TagsByScopeAndName started..."));
		TagsByScope(scope, tmpCandidates);
		PRINT_END_MESSAGE(wxT("TagsByScopeAndName ended"));
		GetFunctionTipFromTags(tmpCandidates, word, tips);
	}

	// display call tip with function prototype
	clCallTipPtr ct( new clCallTip(tips) );
	return ct;
}

//-----------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<< Code Completion API END
//-----------------------------------------------------------------------------
void TagsManager::OpenType(std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	sql << wxT("select * from tags where kind in ('class', 'namespace', 'struct', 'union', 'enum', 'typedef') order by name DESC");
	DoExecuteQueury(sql, tags);
}

void TagsManager::FindSymbol(const wxString& name, std::vector<TagEntryPtr> &tags)
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
			TagEntryPtr tag(new TagEntry(rs));
			tags.push_back(tag);
		}

		// if we have an external database open, add its results to the workspace result
		if( m_pExternalDb->IsOpen() )
		{
			rs = m_pExternalDb->Query( query );
			while( rs.NextRow() )
			{
				TagEntryPtr tag(new TagEntry(rs));
				tags.push_back(tag);
			}
		}
	}
	catch(wxSQLite3Exception &e)
	{
		wxLogMessage(e.GetMessage());
	}
}

void TagsManager::DeleteProject(const std::vector<wxFileName> &projectFiles)
{
	wxString query;
	wxString filelist;
	query << wxT("delete from tags where file in (");
	for(size_t i=0; i<projectFiles.size(); i++)
	{
		filelist << wxT("'") << projectFiles.at(i).GetFullPath() << wxT("'") << wxT(",");
	}
	filelist = filelist.BeforeLast(wxT(','));
	query << filelist << wxT(")");

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

	std::list<tagParseResult> trees;
	for(i=0; i<maxVal; i++)
	{
		// Parse all source file, and concatenate them into one big string
		wxString fileTags;
		wxFileName curFile((traverser.GetFiles())[i]);
		
		// update the progress bar
		if( prgDlg ){
			wxString msg;
			msg << wxT("Parsing file:\n") << curFile.GetFullPath();
			prgDlg->Update(i, msg);
		}

		tags.Clear();
		tagParseResult result;
		if(GetParseComments())
		{
			result.comments = new std::vector<DbRecordPtr>();
			result.tree = ParseSourceFile(curFile, result.comments);
		}
		else
		{
			result.tree = ParseSourceFile(curFile);
		}
		trees.push_back(result);
	}

	if( prgDlg ){
		prgDlg->Update(maxVal, wxT("Saving symbols to database..."));
	}

	TagsDatabase db;
	unsigned int cur = 1;
	for(std::list<tagParseResult>::iterator iter = trees.begin(); iter != trees.end(); iter++)
	{
		if(prgDlg){
			wxString msg;
			msg << wxT("Saving file status: ") << cur << wxT("/") << (unsigned int)trees.size();
			prgDlg->Update(maxVal, msg);
		}
		db.Store((*iter).tree, dbName, true);
		if(GetParseComments())
		{
			// drop all old entries from this file
			try
			{
				db.Store(*(*iter).comments, dbName, true);
			}
			catch( wxSQLite3Exception & e)
			{
				wxLogMessage(e.GetMessage());
			}

			//free the vector
			delete (*iter).comments;
		}
		cur++;
	} // for(std::list<TagTreePtr>::iterator iter = trees.begin(); iter != trees.end(); iter++)
	if(prgDlg)
		prgDlg->Destroy();
}

void TagsManager::OpenExternalDatabase(const wxFileName &dbName)
{
	// check that the database exist
	if(!wxFile::Exists(dbName.GetFullPath()))
		return;
	
	wxString message;
				
	// load it to memory
	if(GetCtagsOptions().GetFlags() & CC_LOAD_EXT_DB_TO_MEMORY){
		message << wxT("Attaching symbols database '") << dbName.GetFullName() << wxT("' to memory ...");
		wxBusyInfo wait(message);
		wxWindowDisabler disableAll;
		wxBusyCursor cursor;
		m_pExternalDb->LoadToMemory(dbName);
	}else{
		m_pExternalDb->OpenDatabase(dbName);
	}
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
		wxLogMessage(e.GetMessage());
	}

	return wxEmptyString;
}

void TagsManager::StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path)
{
	// Make this call threadsafe
	wxCriticalSectionLocker locker(m_cs);

	m_pDb->Store( comments, path );
}

void TagsManager::FindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	DoFindByNameAndScope(name, scope, tags);

	// Sort the results base on their name
	std::sort(tags.begin(), tags.end(), SDescendingSort());	
}

void TagsManager::DoFindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags)
{
	wxString sql;
	if(scope == wxT("<global>"))
	{
		sql << wxT("select * from tags where name='") << name << wxT("' and parent='<global>'");
		DoExecuteQueury(sql, tags);
	}
	else
	{
		std::vector<wxString> derivationList;
		derivationList.push_back(scope);
		GetDerivationList(scope, derivationList);

		for(size_t i=0; i<derivationList.size(); i++)
		{
			sql.Empty();
			wxString path_;
			path_ << derivationList.at(i) << wxT("::") << name ;

			sql << wxT("select * from tags where path='") << path_ << wxT("'");
			DoExecuteQueury(sql, tags);
		}
	}
}

void TagsManager::DoExecuteQueury(const wxString &sql, std::vector<TagEntryPtr> &tags)
{
	wxSQLite3ResultSet rs = m_pDb->Query(sql);

	// Add the database results
	try
	{
		while( rs.NextRow() )
		{
			// Construct a TagEntry from the rescord set
			TagEntryPtr tag(new TagEntry(rs));
			tags.push_back(tag);
		}

		if( m_pExternalDb->IsOpen() )
		{
			wxSQLite3ResultSet ex_rs;
			ex_rs = m_pExternalDb->Query(sql);

			// add results from external database to the workspace database
			while( ex_rs.NextRow() )
			{		
				// Construct a TagEntry from the rescord set
				TagEntryPtr tag(new TagEntry(ex_rs));
				tags.push_back(tag);
			}
		}
	}
	catch( wxSQLite3Exception& e)
	{
		wxLogMessage(e.GetMessage());
	}
}

bool TagsManager::GetDerivationList(const wxString &path, std::vector<wxString> &derivationList)
{
	wxString sql;
	sql << wxT("select * from tags where path='") << path << wxT("' and kind in ('struct', 'class', 'interface')");

	std::vector<TagEntryPtr> tags;
	TagEntryPtr tag;
	DoExecuteQueury(sql, tags);
	if(tags.size() == 1){
		tag = tags.at(0);
	}else{
		return false;
	}

	if(tag && tag->IsOk())
	{
		wxString ineheritsList = tag->GetInherits();
		wxStringTokenizer tok(ineheritsList, wxT(','));
		while(tok.HasMoreTokens())
		{
			wxString inherits = tok.GetNextToken();
			if(tag->GetScopeName() != wxT("<global>"))
			{
				inherits = tag->GetScopeName() + wxT("::") + inherits;
			}
			derivationList.push_back(inherits);
			GetDerivationList(inherits, derivationList);
		}
	}
	return true;
}

void TagsManager::TipsFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips)
{
	for(size_t i=0; i<tags.size(); i++)
	{
		if(tags.at(i)->GetName() != word)
			continue;

		wxString tip = tags.at(i)->GetPattern();
		wxString comment;

		//handle comments
		if(GetCtagsOptions().GetFlags() & CC_DISP_COMMENTS){
			int      lineno   = tags.at(i)->GetLine();
			wxString filename = tags.at(i)->GetFile();
			if(lineno != wxNOT_FOUND && filename.IsEmpty() == false){
				comment = GetComment(filename, lineno);
				if(comment.IsEmpty() == false){
					comment << wxT("\n");
				}
			}
		}

		//remove the pattern perfix and suffix
		tip = tip.AfterFirst(wxT('^'));
		tip = tip.BeforeLast(wxT('$'));
		
		// Trim whitespace from right and left
		static wxString trimString(wxT("{};\r\n\t\v "));

		tip.erase(0, tip.find_first_not_of(trimString)); 
		tip.erase(tip.find_last_not_of(trimString)+1);

		tip.Prepend(comment);
		tips.push_back(tip);
	}
}

void TagsManager::GetFunctionTipFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips)
{
	std::map<wxString, wxString> tipsMap;

	for(size_t i=0; i<tags.size(); i++)
	{
		if(tags.at(i)->GetName() != word)
			continue;

		if(tags.at(i)->GetKind() != wxT("function") && tags.at(i)->GetKind() != wxT("prototype"))
			continue;

		wxString tip = word;
		tip << tags.at(i)->GetSignature();
		//collect each signature only once, we do this by using
		//map
		tipsMap[tags.at(i)->GetSignature()] = tip;
	}

	for(std::map<wxString, wxString>::iterator iter = tipsMap.begin(); iter != tipsMap.end(); iter++)
	{
		tips.push_back(iter->second);
	}
}

void TagsManager::CloseExternalDatabase()
{
	//close the database by simply deleting it and creating new
	//empty one
	if(m_pExternalDb){
		delete m_pExternalDb;
		m_pExternalDb = new TagsDatabase();
	}
}

wxString TagsManager::GenerateDoxygenComment(const wxString &file, const int line)
{
	if(m_pDb->IsOpen())
	{
		wxString sql;
		sql << wxT("select * from tags where file='") << file << wxT("' and line=") << line + 1;
		std::vector<TagEntryPtr> tags;
		DoExecuteQueury(sql, tags);
		if(tags.empty() || tags.size() > 1)
			return wxEmptyString;

		//create a doxygen comment from the tag
		return DoCreateDoxygenComment(tags.at(0));
	}
	return wxEmptyString;
}

wxString TagsManager::DoCreateDoxygenComment(TagEntryPtr tag)
{
	CppCommentCreator commentCreator(tag);
	return commentCreator.CreateComment();
}

bool TagsManager::GetParseComments()
{
	//wxCriticalSectionLocker lock(m_cs);
	return m_parseComments;
}

void TagsManager::SetCtagsOptions(const TagsOptionsData &options)
{
	m_options = options; 
	RestartCtagsProcess(TagsGlobal);
	
	wxCriticalSectionLocker locker(m_cs);
	m_parseComments = m_options.GetFlags() & CC_PARSE_COMMENTS ? true : false;
}

void TagsManager::GenerateSettersGetters(const wxString &scope, const SettersGettersData &data, const std::vector<TagEntryPtr> &tags, wxString &impl, wxString *decl)
{
	wxUnusedVar(scope);
	wxUnusedVar(data);
	wxUnusedVar(tags);
	wxUnusedVar(impl);
	wxUnusedVar(decl);
}

void TagsManager::TagsByScope(const wxString &scopeName, const wxString &kind, std::vector<TagEntryPtr> &tags, bool includeInherits)
{
	wxString sql;
	std::vector<wxString> derivationList;
	//add this scope as well to the derivation list
	derivationList.push_back(scopeName);

	if(includeInherits){
		GetDerivationList(scopeName, derivationList);
	}

	//make enough room for max of 500 elements in the vector
	tags.reserve(500);

	for(size_t i=0; i<derivationList.size(); i++)
	{
		sql.Empty();
		wxString tmpScope(derivationList.at(i));
		sql << wxT("select * from tags where scope='") << tmpScope << wxT("' and kind='") << kind << wxT("'");;
		DoExecuteQueury(sql, tags);
	}
	// and finally sort the results
	std::sort(tags.begin(), tags.end(), SAscendingSort());
}

wxString TagsManager::GetScopeName(const wxString &scope)
{
	Language *lang = LanguageST::Get();
	return lang->GetScopeName(scope);
}
