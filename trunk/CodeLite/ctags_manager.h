#ifndef CODELITE_CTAGS_MANAGER_H
#define CODELITE_CTAGS_MANAGER_H

#include "wx/event.h"
#include "wx/process.h"
#include "cl_process.h"
#include "tree.h"
#include "entry.h"
#include "tags_database.h"
#include <wx/thread.h>
#include "singleton.h"
#include "calltip.h"
#include "comment.h"
#include "language.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE


#ifdef USE_TRACE
#include <wx/stopwatch.h>
#endif

/// Forward declaration
class clProcess;
class DirTraverser;
class Language;

#define TagsGlobal 0
#define TagsLocal  1

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
class WXDLLIMPEXP_CL CtagsOptions{
	wxString forceLanguage;
	wxString fileSpec;
	wxString ignoreMacros;
	bool parseComments;

public:
	CtagsOptions() 
		: forceLanguage(wxT("C++"))
		, fileSpec(wxT("*.cpp;*.c;*.cxx;*.cc;*.h;*.hpp"))
		, ignoreMacros(wxEmptyString)
		, parseComments(false)
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

	void SetParseComments(bool parse){
		parseComments = parse;
	}

	wxString GetLanguage() const { return forceLanguage; }
	wxString GetFileSpec() const { return fileSpec; }
	wxString GetIgnoreMacros() const { return ignoreMacros; }
	bool GetParseComments() const { return parseComments; }
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
 * TagsManagerST::Get()->StartCtagsProcess(TagsGlobal);
 * TagsManagerST::Get()->StartCtagsProcess(TagsLocal);
 * \endcode
 *
 * In the destructor of your main frame it is recommended to call Free() to avoid memory leaks:
 *
 * \code
 * // kill the TagsManager object first it will do the process termination and cleanup
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
class WXDLLIMPEXP_CL TagsManager : public wxEvtHandler 
{
	// Members
	friend class Singleton<TagsManager>;
	friend class DirTraverser;
	friend class Language;

	TagsDatabase *m_pDb;
	TagsDatabase *m_pExternalDb;

	wxCriticalSection m_cs;

	wxFileName m_ctagsPath;
	clProcess* m_ctags;
	clProcess* m_localCtags;
	std::map<int, wxString> m_ctagsCmd;
	
	wxStopWatch m_watch;
	bool m_parseComments;
	CtagsOptions m_options;
	std::map<int, clProcess*> m_processes;

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
	void SetCtagsOptions(const CtagsOptions &options) { 
		m_options = options; 
		RestartCtagsProcess(TagsLocal);
		RestartCtagsProcess(TagsGlobal);
	}

	/**
	 * Locate symbol by name in database
	 * \param name name to search
	 * \param tags [output] result vector
	 */
	void FindSymbol(const wxString& name, std::vector<TagEntryPtr> &tags);

	/**
	 * ParseTagsFile CTAGS file and construct a TagTree. 
	 * User should use ParseSourceFile, which calls to this function internally.
	 * This function throws a std::exception*.
	 * \param fp CTAGS tags file name
	 * \return tag tree
	 */
	TagTreePtr ParseTagsFile(const wxFileName& fp);

	/**
	 * Parse a source file and construct a TagTree.
	 * This function throws a std::exception*.
	 * \param fp Source file name
	 * \param comments if not null, comments will be parsed as well, and will be returned as vector
	 * \return tag tree
	 */
	TagTreePtr ParseSourceFile(const wxFileName& fp, std::vector<DbRecordPtr> *comments = NULL);

	/**
	 * Parse an array of source files and construct a TagTree.
	 * This function throws a std::exception*.
	 * \param fpArr An array of source files
	 * \param comments if not null, comments will be parsed as well, and will be returned as vector
	 * \return tag tree
	 */
	TagTreePtr ParseSourceFiles(const std::vector<wxFileName> &fpArr, std::vector<DbRecordPtr> *comments = NULL);
	
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
	 * \brief Store vector of comments into database
	 * \param comments comments vector to store
	 * \param path Database file name
	 */
	void StoreComments(const std::vector<DbRecordPtr> &comments, const wxFileName& path = wxFileName());

	/**
	 * load all symbols of fileName from the database and return them 
	 * to user as tree
	 * \param path file's symbols
	 * \return tag tree
	 */
	TagTreePtr Load(const wxFileName& fileName);

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
	 * Delete all entries from database that are related to file name.
	 * \param path Database name
	 * \param fileName File name
	 */
	void Delete(const wxFileName& path, const wxString& fileName);

	/**
	 * Start a ctags process on a filter mode. 
	 * By default, TagsManager will try to launch the ctags process using the following command line:
	 * \code
	 * ctags --fields=aKmSsni --filter=yes --filter-terminator="<<EOF>>\n"
	 * \endcode
	 * 
	 * It is possible to add a full path to ctags exectuable by calling the SetCtagsPath() function.
	 */
	clProcess *StartCtagsProcess(int kind);

	/**
	 * Restart ctags process.
	 * \param kind 
	 * \return 
	 */
	void RestartCtagsProcess(int kind);

	/**
	 * Set the event handler to handle notifications of tree changes. 
	 * This is usually the SymbolTree class (or its derived classes).
	 * \param evtHandler 
	 */
	void SetEventHandler(wxEvtHandler* evtHandler);

	/**
	 * Test if filename matches the current ctags file spec.
	 * \param filename file name to test
	 * \return true if the file name extension matches the current running ctags file spec
	 */
	bool IsValidCtagsFile(const wxFileName &filename) const;

	/** 
	 * Find symbols by name and scope. 
	 * \param name symbol name
	 * \param scope full path to symbol. if set to wxEmptyString, the search is performed against the global
	 * \param tags [output] a vector of the results tags
	 * \return true on success false otherwise
	 */
	void FindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags);

	/**
	 * Get tags related to a scope.
	 * \param scope scope to search for members
	 * \param tags [output] vector of tags
	 */
	void TagsByScope(const wxString& scope, std::vector<TagEntryPtr> &tags);

	/**
	 *	Get tags related to a scope and name (name can be partial name
	 * \param scope scope to search for members
	 * \param name partial tag name 
	 * \param tags [output] vector of tags
	 */
	void TagsByScopeAndName(const wxString& scope, const wxString &name, std::vector<TagEntryPtr> &tags);

	/**
	 * Return autocompletion candidates based on parsing an expression and retrieving its member from the database.
	 * \param expr Expression to evaluate, can be complex one, such as ((MyClass&)cls).GetName().GetData() ... )
	 * \param text Scope where the expression is located
	 * \param candidates [output] list of TagEntries that can be displayed in Autucompletion box
	 * \return true if candidates.size() is greater than 0
	 */
	bool AutoCompleteCandidates(const wxString& expr, const wxString& text, std::vector<TagEntryPtr> &candidates);

	/**
	 * Return a word completion candidates. this function is used when user hit Ctrl+Space.
	 * \param expr Expression to evaluate, can be complex one, such as ((MyClass&)cls).GetName().GetData() ... )
	 * \param text Scope where the expression is located
	 * \param &word the partial word entered by user
	 * \param &candidates [output] list of TagEntries that can be displayed in Autucompletion box
	 * \return true if candidates.size() is greater than 0
	 */
	bool WordCompletionCandidates(const wxString& expr, const wxString& text, const wxString &word, std::vector<TagEntryPtr> &candidates);

	/**
	 * Delete all tags related to project
	 * \param projectName project name
	 */
	void DeleteProject(const std::vector<wxFileName> &projectFiles);

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
	 * Get a hover tip. This function is a wrapper around the Language::GetHoverTip.
	 * \param expr the current expression
	 * \param word the token under the cursor
	 * \param scope scope where token was found
	 * \param scopeName scope name
	 * \param isFunc is token is a function
	 * \param tips array of tip strings
	 */
	void GetHoverTip(const wxString & expr, const wxString &word, const wxString & text, std::vector<wxString> & tips);

	/**
	 * Return a function call tip object
	 * \param expression expression where the function was found
	 * \param text local scope
	 * \param word function name
	 * \return call tip object
	 */
	clCallTipPtr GetFunctionTip(const wxString &expression, const wxString &text, const wxString &word);

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
	void SourceToTags(const wxFileName& source, wxString& tags, clProcess *ctags);

	/**
	 * Construct a tag object from line.
	 * \param line Line 
	 * \param tag Tag from the line (output)
	 */
	void TagFromLine(const wxString& line, TagEntry& tag);

	/**
	 * Parse tags from memory and constructs a TagTree. 
	 * This function throws a std::exception*.
	 * \param tags wxString containing the tags to parse
	 * \return tag tree, must be freed by caller
	 */
	TagTreePtr TreeFromTags(const wxString& tags);

	/**
	 *
	 * \param &path
	 * \param &derivationList 
	 * \return 
	 */
	bool GetDerivationList(const wxString &path, std::vector<wxString> &derivationList);


protected:
	void DoFindByNameAndScope(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags);
	void DoExecuteQueury(const wxString &sql, std::vector<TagEntryPtr> &tags);
	void RemoveDuplicates(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
	void RemoveDuplicatesTips(std::vector<TagEntryPtr>& src, std::vector<TagEntryPtr>& target);
	void GetGlobalTags(const wxString &name, std::vector<TagEntryPtr> &tags, SearchFlags flags = PartialMatch);
	void GetLocalTags(const wxString &name, const wxString &scope, std::vector<TagEntryPtr> &tags, SearchFlags flags = PartialMatch);
	void TipsFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips);
	void GetFunctionTipFromTags(const std::vector<TagEntryPtr> &tags, const wxString &word, std::vector<wxString> &tips);
};

/// create the singleton typedef
typedef Singleton<TagsManager> TagsManagerST;

#endif // CODELITE_CTAGS_MANAGER_H

