#ifndef CODELITE_CTAGS_MANAGER_H
#define CODELITE_CTAGS_MANAGER_H

#include "wx/event.h"
#include "wx/process.h"
#include "process.h"
#include "tree.h"
#include "entry.h"
#include "tags_database.h"
#include <wx/thread.h>
#include "singleton.h"
#include "calltip.h"
#include "comment.h"


#ifdef USE_TRACE
#include <wx/stopwatch.h>
#endif

/// Forward declaration
class clProcess;
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
 * \ingroup CodeLite
 * \brief a wrapper around ctags options: --force-language and -I[macros]
 * see http://ctags.sourceforge.net/ctags.html for more details about the ctags options
 *
 * \version 1.0
 * first version
 *
 * \date 05-04-2007
 *
 * \author Eran
 *
 */
class CtagsOptions{
	wxString forceLanguage;
	wxString fileSpec;
	wxString ignoreMacros;

public:
	CtagsOptions() 
		: forceLanguage(wxT("C++"))
		, fileSpec(wxEmptyString)
		, ignoreMacros(wxEmptyString)
	{}

	~CtagsOptions(){}

	/**
	 * Format a single string from this class 
	 * \return CTAGS options
	 */
	wxString ToString() const;

	//----------------------------------
	// setters / getters
	//----------------------------------
	void SetLanguage(const wxString &lang){
		forceLanguage = lang;
		forceLanguage = forceLanguage.Trim();
		forceLanguage = forceLanguage.Trim(false);
	}

	void SetFileSpec(const wxString &spec){
		fileSpec = spec;
		fileSpec = fileSpec.Trim();
		fileSpec = fileSpec.Trim(false);
	}

	void SetIgnoreMacros(const wxString &macros) {
		ignoreMacros = macros;
		ignoreMacros = ignoreMacros.Trim();
		ignoreMacros = ignoreMacros.Trim(false);
	}

	wxString GetLanguage() const { return forceLanguage; }
	wxString GetFileSpec() const { return fileSpec; }
	wxString GetIgnoreMacros() const { return ignoreMacros; }
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
 * clProcess* m_ctags = TagsManagerST::Get()->StartCtagsProcess(TagsGlobal);
 * clProcess* m_localCtags = TagsManagerST::Get()->StartCtagsProcess(TagsLocal);
 * \endcode
 *
 * In the destructor of your main frame it is recommended to call Free() to avoid memory leaks:
 *
 * \code
 * // kill the TagsManager object first, so it will not restart
 * // terminating ctags processes
 * TagsManager::Free();
 * m_localCtags->Terminate();
 * m_ctags->Terminate()
 *
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
	clProcessPtr m_ctags;
	clProcessPtr m_localCtags;
	std::map<int, wxString> m_ctagsCmd;
	
#ifdef USE_TRACE
	wxStopWatch m_watch;
#endif
	bool m_parseComments;
	std::map<wxString, bool> m_validExtensions;
	CtagsOptions m_options;

public:
	/**
	 * Return the CtagsOptions used by the tags manager
	 * \return 
	 */
	const CtagsOptions& GetCtagsOptions() const { return m_options; }

	/**
	 * Set Ctags Options
	 * \param options options to use
	 */
	void SetCtagsOptions(CtagsOptions &options) { m_options = options; }

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
	 * Create an empty project in the tags database
	 * \param projectName the project name to create
	 */	
	void CreateProject(const wxString &projectName);
	
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
	 * By default, TagsManager will try to launch the ctags process using the following command line:
	 * \code
	 * ctags --fields=aKmSsni --filter=yes --filter-terminator="<<EOF>>\n"
	 * \endcode
	 * 
	 * It is possible to add a full path to ctags exectuable by calling the SetCtagsPath() function.
	 */
	clProcessPtr StartCtagsProcess(int kind = TagsGlobal);

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
protected:

	/**
	 * Handler ctags process termination
	 */
	void OnCtagsEnd(wxProcessEvent& event);

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
	void SourceToTags(const wxFileName& source, wxString& tags, clProcessPtr ctags);

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

#endif // CODELITE_CTAGS_MANAGER_H

