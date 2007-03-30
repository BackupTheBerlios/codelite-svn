#ifndef CODELITE_CTAGS_MANAGER_H
#define CODELITE_CTAGS_MANAGER_H

#include "tree.h"
#include "entry.h"
#include "tags_database.h"
#include <wx/thread.h>
#include "singleton.h"
#include <wx/dir.h>
#include "calltip.h"
#include "comment.h"

#ifdef USE_TRACE
#include <wx/stopwatch.h>
#endif

/// Forward declaration
class TagsProcess;
class DirTraverser;
class Language;

enum CtagsType
{
	TagsGlobal = 0,
	TagsLocal
};

enum SearchFlags
{
	PartialMatch = 1,
	ExactMatch = 2
};

/**
 * This class is the interface to ctags and SQLite database. 
 * It contains various APIs that allows the caller to parse source file(s), 
 * store it into the database and return a symbol tree.
 * TagsManager is also responsible for starting ctags processes. 
 *
 * Before you use TagsManager, usually you would like to start ctags, 
 * this is easily done by writing something like this: 
 *
 * \code
 * // Create ctags processes, one for local scope and one for global scope.
 * // 'this' is a pointer to the main frame or any other window that wishes to be notified 
 * // if ctags process died
 * TagsProcess* m_ctags = TagsManagerST::Get()->StartCtagsProcess(this, 10500, TagsGlobal);
 * TagsProcess* m_localCtags = TagsManagerST::Get()->StartCtagsProcess(this, 10501, TagsLocal);
 * \endcode
 *
 * In the destructor of your main frame it is recommended to call Free() to avoid memory leaks:
 *
 * \code
 * // First make sure no ctags processes exist
 * wxKill(m_localCtags->GetPid(), wxSIGKILL);
 * wxKill(m_ctags->GetPid(), wxSIGKILL);
 *
 * // Now it is safe to free tagsmanager
 * TagsManager::Free();
 * \endcode
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 09-01-2006
 *
 * \author Eran
 *
 */
class TagsManager 
{
	// Members
	friend class Singleton<TagsManager>;
	friend class DirTraverser;
	friend class Language;

	TagsDatabase *m_pDb;
	TagsDatabase *m_pExternalDb;

	wxCriticalSection m_cs;

	wxFileName m_ctagsPath;
	TagsProcess* m_ctags;
	TagsProcess* m_localCtags;
	std::map<int, wxString> m_ctagsCmd;
#ifdef USE_TRACE
	wxStopWatch m_watch;
#endif
	bool m_parseComments;
	std::map<wxString, bool> m_validExtensions;

public:
	//--------------------------------
	// Operations
	//--------------------------------
	/**
	 * Locate symbol by name in database
	 * \param name name to search
	 * \param tags [output] result vector
	 */
	void FindSymbol(const wxString& name, std::vector<TagEntry> &tags);

	/**
	 * ParseTagsFile CTAGS file and construct a TagTree. 
	 * User should use ParseSourceFile, which calls to this function internally.
	 * This function throws a std::exception*.
	 * \param fp CTAGS tags file name
	 * \param project Project to associate to this file
	 * \return tag tree
	 */
	TagTreePtr ParseTagsFile(const wxFileName& fp, const wxString& project = wxEmptyString);

	/**
	 * Parse a source file and construct a TagTree.
	 * This function throws a std::exception*.
	 * \param fp Source file name
	 * \param project Project to associate to this file
	 * \param comments if not null, comments will be parsed as well, and will be returned as vector
	 * \return tag tree
	 */
	TagTreePtr ParseSourceFile(const wxFileName& fp, const wxString& project = wxEmptyString, std::vector<DbRecordPtr> *comments = NULL);

	/**
	 * Parse an array of source files and construct a TagTree.
	 * This function throws a std::exception*.
	 * \param fpArr An array of source files
	 * \param project Project to associate to this file
	 * \param comments if not null, comments will be parsed as well, and will be returned as vector
	 * \return tag tree
	 */
	TagTreePtr ParseSourceFiles(const std::vector<wxFileName> &fpArr, const wxString& project = wxEmptyString, std::vector<DbRecordPtr> *comments = NULL);

	/**
	 * Parse a source file for local variables only construct a TagTree.
	 * This function throws a std::exception*.
	 * \param scope string containing the scope to parse
	 * \return a sorted TagEntry vector which must be freed by caller
	 */	
	std::vector<TagEntry>* ParseLocals(const wxString& scope);

	/**
	 * \brief Set the full path to ctags executable, else TagsManager will use relative path ctags.
	 * So, if for example, ctags is located at: $/home/eran/bin$, you simply call this function
	 * with SetCtagsPath(_T("/home/eran/bin"));
	 * \param path ctags 
	 */
	void SetCtagsPath(const wxString& path);

	/**
	 * \brief Store tree of tags into db.
	 * \param tree Tags tree to store
	 * \param path Database file name
	 */
	void Store(TagTreePtr tree, const wxFileName& path = wxFileName());

	/**
	 * \brief Store vector of tags into db.
	 * \param tags Tags vector to store
	 * \param path Database file name
	 */
	void Store(const std::vector<TagEntry> &tags, const wxFileName& path = wxFileName());

	/**
	 * \brief Store vector of comments into database
	 * \param comments comments vector to store
	 * \param path Database file name
	 */
	void StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path = wxFileName());

	/**
	 * Construct tree from database. 
	 * Returned tree should be freed by caller.
	 * \param path Database file name
	 * \param project Project name
	 * \return tag tree, must be freed by caller
	 */
	TagTreePtr Load(const wxFileName& path = wxFileName(), const wxString& project = wxEmptyString);

	/**
	 * Open sqlite database.
	 * \param fileName Database file name
	 */
	void OpenDatabase(const wxFileName& fileName);
	
	/**
	 * Return a pointer to the underlying database object.
	 * \return tags database
	 */
	const TagsDatabase* GetDatabase() const { return m_pDb; }

	/**
	 * Delete all entries from database that are related to project and file name.
	 * \param path Database name
	 * \param project Project name
	 * \param fileName File name
	 */
	void Delete(const wxFileName& path, const wxString& project, const wxString& fileName);

	/**
	 * Start a ctags process on a filter mode. 
	 * TagsManager will try to launch the ctags process using the following command line:
	 * \code
	 * ctags.exe --fields=aKmSsni --filter=yes --filter-terminator="<<EOF>>\n"
	 * \endcode
	 * 
	 * It is possible to add a full path to ctags exectuable by calling the SetCtagsPath() function.
	 * This function throws a std::exception*.
	 *
	 * \param evtHandler Event handler that will receive the wxEVT_PROCESS_END event in case the process terminates
	 * \param id id of an event
	 * \param kind TagsManager can start two kind of ctags processes, 
	 * - one for processing local variables: TagsLocal (for WordCompletion & CodeCompletion) 
	 * - and the second: TagsGlobal - processes globals (classes, namespaces, structs, functions, etc)
	 * \return A pointer to TagsProcess (derived from wxProcess) allocated on the heap.
	 * It is under the user reponsibility to delete it
	 */
	TagsProcess* StartCtagsProcess(wxEvtHandler* evtHandler = NULL, int id = wxID_ANY, int kind = TagsGlobal);

	/**
	 * Set the event handler to handle notifications of tree changes. 
	 * This is usually the SymbolTree class (or its derived classes).
	 * \param evtHandler 
	 */
	void SetEventHandler(wxEvtHandler* evtHandler);

	/**
	 * Return the closest functions' line number from lineNo, the search direction is UP.
	 * \param lineNo Line number to start from
	 * \param project Project name
	 * \param file Source file to search in
	 * \param tag TagEntry containing the function
	 * \return true if function was found, else false
	 */
	bool FunctionByLine(const int lineNo, const wxString& fileName, const wxString& project, TagEntry& tag);

	/**
	 * Get vector of tags from database by name (or part of it) and scope name.
	 * this function is mainly used to get list of possibilities for populating the auto completion box
	 * when user press ctrl+space
	 * \note Scope name does not include the project name in it.
	 * \param name Word name (or part of it)
	 * \param scopeName Scope name to search in
	 * \param tags [output] resulted tags
	 * \param scope Add an additional scope to search for, the results from this scope will be added to the 
	 * results from the database will be converted into TagEntry of kind member with public access
	 * \param allowDups Allow duplicates in the array of tags. In some cases, such as the code browsing, duplicate tags
	 * are important, while in cases like auto-completion or word-completion, it is preferred not to have duplicate tags
	 * default is set to false
	 */
	void GetTags(const wxString& name, const wxString& scopeName, std::vector<TagEntry>& tags, int flags = PartialMatch, const wxString& scope = wxEmptyString, bool allowDups = false);

	/**
	 * Return list of tags for the code completion (user hit '.'  '->' '::' )
	 * \param qualifier parent of the memebers
	 * \param tags [output] list of tags belongs to parent
	 */
	void GetMembers(const wxString & qualifier, const wxString & name, std::vector<TagEntry>& tags);

	/**
	 * Get tags related to parent.
	 * It's a wrapper for the sql statement: $select * from tags where parent='parent';$
	 * \param parent Parent 
	 * \param tags [output] vector of tags
	 * \param inheritedMembers set to true when you want to retrieve member of base classes
	 */
	void TagsByParent(const wxString& parent, std::vector<TagEntry> &tags, bool inheritedMembers = false);

	/**
	 * Return autocompletion candidates based on parsing an expression and retrieving its member from the database.
	 * \param expr Expression to evaluate, can be complex one, such as ((MyClass&)cls).GetName().GetData() ... )
	 * \param scope Scope where the expression is located
	 * \param scopeName Scope name if any (for example, if the scope starts with void Buffer::Add(){ ... expr , then
	 * the scope name is 'Buffer'
	 * \param candidates [output] list of TagEntries that can be displayed in Autucompletion box
	 * \return true if candidates.size() is greater than 0
	 */
	bool AutoCompleteCandidates(const wxString& expr, const wxString& scope, const wxString& scopeName, std::vector<TagEntry> &candidates);

	/**
	 * Delete all tags related to project
	 * \param projectName project name
	 */
	void DeleteProject(const wxString& projectName);

	/**
	 * Build a secondary database that will be used for searching (e.g. database containing C/C++ header files)
	 * if dbName is already existed, it will be updated, else it will be created.
	 * \param rootDir root directory to start processing files
	 * \param dbName database to store the data
	 * \param FFU
	 * \param updateDlgParent when set to non-null, TagsManager will popup a modal dialog to report its progress
	 */
	void BuildExternalDatabase(const wxFileName & rootDir, const wxFileName &dbName, const wxString& WXUNUSED(language), wxWindow* updateDlgParent = NULL);

	/**
	 * Open a an existing external database that will be used for searching (e.g. database containing C/C++ header files)
	 * tags in this database can not be updated. This is a read-only database
	 * \param dbName external database file name
	 */
	void OpenExternalDatabase(const wxFileName &dbName);

	/**
	 * Get Tags by user sql 
	 * \param sql sql query
	 * \param [output] tags 
	 * \param kindToFilter exclude tag of this kind
	 * \param excludePrefix if a tag name is starting with this prefix, it will excluded
	 */
	void GetTagsBySQL(const wxString& sql, std::vector<TagEntry> &tags, const wxString &kindToFilter = wxEmptyString, const wxString &excludePrefix = wxEmptyString);

	/**
	 * Get tag by name, if more than one tag exist, return the first one
	 * \param name tag name
	 * \param &tag [output] tag
	 * \return true if match was found, else false
	 */
	bool GetClassTagByName(const wxString& name, TagEntry &tag);

	/**
	 * Get a hover tip. This function is a wrapper around the Language::GetHoverTip.
	 * \param token the token under the cursor
	 * \param scope scope where token was found
	 * \param scopeName scope name
	 * \param isFunc is token is a function
	 * \param tips array of tip strings
	 */
	void GetHoverTip(const wxString & token, const wxString & scope, const wxString & scopeName, bool isFunc, std::vector<wxString> & tips);

	/**
	 * Return a function call tip object
	 * \param expression expression where the function was found
	 * \param scope local scope
	 * \param scopeName scope name
	 * \return call tip object
	 */
	CallTipPtr GetFunctionTip(const wxString &expression, const wxString &scope, const wxString &scopeName);

	/**
	 * When parsing files, parse comments as well and store them into database
	 * \param parse set to true to enable comments parsing
	 */
	void ParseComments(const bool parse);

	/**
	 * Return true if comment parsing is enabled, false otherwise
	 */
	bool GetParseComments() const { return m_parseComments; }

	/**
	 * Load comment from database by line and file
	 * \param line line number
	 * \param file file name
	 */
	wxString GetComment(const wxString &file, const int line);

private: 
	/**
	 * Construct a TagsManager object, for internal use
	 */
	TagsManager();

	/**
	 * Destructor
	 */
	virtual ~TagsManager();

	/**
	 * Pass a source file to ctags process, wait for it to process it and return the output.
	 * This function throws a std::exception*.
	 * \param source Source file name
	 * \param tags String containing the ctags output
	 * \param ctags Ctags process to use for the parsing of the source file - ctags manager holds two 
	 * ctags processes, one for parsing local variables and one for global scope
	 */
	void SourceToTags(const wxFileName& source, wxString& tags, TagsProcess* ctags);

	/**
	 * Parse tags from memory construct a TagTree. 
	 * User should use ParseSourceFile, which calls to this function internally.
	 * This function throws a std::exception*.
	 * \param tags wxString containing the tags to parse
	 * \param project Project to associate to this file
	 * \return tag tree
	 */
	TagTreePtr ParseTags(const wxString& tags, const wxString& project = wxEmptyString);
	
	/**
	 * Create a vector of tags.
	 * This function throws a std::exception*.
	 * \param tags String containing the ctags 
	 * \param onlyLocals set to true if you wish to receive only local tags
	 * \return vector of TagEntrys, even if there are no tags vector is returned but with size() == 0
	 */
	std::vector<TagEntry>* VectorFromTags(const wxString& tags, bool onlyLocals = true);

	/**
	 * Construct a tag object from line.
	 * \param line Line 
	 * \param tag Tag from the line (output)
	 * \param project Tag project name
	 */
	void TagFromLine(const wxString& line, TagEntry& tag, const wxString& project);

	/**
	 * Parse tags from memory and constructs a TagTree. 
	 * This function throws a std::exception*.
	 * \param tags wxString containing the tags to parse
	 * \param project Project to associate to this file
	 * \return tag tree, must be freed by caller
	 */
	TagTreePtr TreeFromTags(const wxString& tags, const wxString& project);

	/**
	 * Filter out entries from src vector. The filter is done base on the name & flags
	 * \param src source vector
	 * \param name name to search (or partial name of PartialMatch is set)
	 * \param target [output] output vector
	 * \param flags comaprison criterion
	 * \param tmpMap set this to non null if you wishe to have a result map without duplicate entries
	 */
	void FilterResults(const std::vector<TagEntry> & src, const wxString & name, std::vector<TagEntry> & target, int flags = PartialMatch, std::map<wxString, TagEntry>* tmpMap = NULL);
};

/// create the singleton typedef
typedef Singleton<TagsManager> TagsManagerST;

/**
 * A helper class that help us recurse into sub-directories.
 * For each found directory, sink.OnDir() is called and sink.OnFile() is called for every file. 
 * Depending on the return value, the enumeration may continue or stop of one of the files, the 
 * recurse may stop.
 * \ingroup CodeLite
 * \version 1.0
 * first version
 * \date 11-08-2006
 * \author Eran
 */
class DirTraverser : public wxDirTraverser
{
	std::vector<wxFileName> m_files;
public:
	/**
	 * This function is called once a file is found. The traverse of the directories 
	 * can be stopped based on the return value from this function:
	 * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
	 * - wxDIR_STOP,             // stop traversing
	 * - wxDIR_CONTINUE          // continue into this directory
	 * \param filename name of the file that was found
	 * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
	 */
	virtual wxDirTraverseResult  OnFile(const wxString& filename)
	{
		// add the file to our array
		wxFileName file(filename);
		m_files.push_back(file);
		return wxDIR_CONTINUE;
	}

	/**
	 * This function is called once a directory is found. The traverse of the directories 
	 * can be stopped based on the return value from this function:
	 * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
	 * - wxDIR_STOP,             // stop traversing
	 * - wxDIR_CONTINUE          // continue into this directory
	 * \param dirname name of the directory that was found
	 * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
	 */
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
	{
		// do nothing
		return wxDIR_CONTINUE;
	}

	/**
	 * Return list of files found 
	 * \return Return list of files found 
	 */
	std::vector<wxFileName>& GetFiles()
	{
		return m_files;
	}
};

#endif // CODELITE_CTAGS_MANAGER_H

