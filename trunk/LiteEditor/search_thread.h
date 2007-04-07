#ifndef SEARCH_THREAD_H
#define SEARCH_THREAD_H

#include <deque>
#include <list>
#include <wx/string.h>
#include <map>
#include <wx/thread.h>
#include "singleton.h"
#include "wx/event.h"
#include "wx/filename.h"
#include <wx/regex.h>

// Possible search data options:
enum {
	wxSD_MATCHCASE         = 1,
	wxSD_MATCHWHOLEWORD    = 2,
	wxSD_REGULAREXPRESSION = 4	
};

class wxEvtHandler;

//----------------------------------------------------------
// The searched data class to be passed to the search thread
//----------------------------------------------------------

class SearchData
{
	wxString m_rootDir;
	wxString m_findString;
	wxString m_validExt;
	int m_flags;

private:
	// An internal helper function that set/remove an option bit
	void SetOption(int option, bool set){
		if(set){
			m_flags |= option;
		} else {
			m_flags &= ~(option);
		}
	}

public:
	// Ctor-Dtor
	SearchData() 
		: m_rootDir(wxEmptyString)
		, m_findString(wxEmptyString) 
		, m_flags(0)
	{}

	SearchData(const wxString &rootDir, const wxString &findString, const int flags, const wxString &exts)
		: m_rootDir(rootDir)
		, m_findString(findString)
		, m_flags(flags)
		, m_validExt(exts)
	{}

	SearchData(const SearchData &rhs){
		*this = rhs;
	}

	virtual ~SearchData()
	{}

	SearchData& operator=(const SearchData &rhs){
		if(this == &rhs)
			return *this;

		m_findString = rhs.m_findString;
		m_flags = rhs.m_flags;
		m_validExt = rhs.m_validExt;
		m_rootDir = rhs.m_rootDir;
		return *this;
	}
	
public:
	//------------------------------------------
	// Setters / Getters
	//------------------------------------------
	bool IsMatchCase() const { return m_flags & wxSD_MATCHCASE ? true : false; }
	bool IsMatchWholeWord() const { return m_flags & wxSD_MATCHWHOLEWORD ? true : false; }
	bool IsRegularExpression() const { return m_flags & wxSD_REGULAREXPRESSION ? true : false; }
	const wxString &GetRootDir() const { return m_rootDir; }
	void SetMatchCase(bool matchCase){ SetOption(wxSD_MATCHCASE, matchCase); }
	void SetMatchWholeWord(bool matchWholeWord){ SetOption(wxSD_MATCHWHOLEWORD, matchWholeWord); }
	void SetRegularExpression(bool re){ SetOption(wxSD_REGULAREXPRESSION, re); }
	void SetExtensions(const wxString &exts) { m_validExt = exts; }
	void SetRootDir(const wxString &rootDir) { m_rootDir = rootDir; }
	std::map<wxString, bool> GetExtensions() const;
	const wxString &GetFindString() const { return m_findString; }
	void SetFindString(const wxString &findString){ m_findString = findString; }
};

//------------------------------------------
// class containing the search result
//------------------------------------------
class SearchResult : public wxObject {
	wxString m_pattern;
	int m_lineNumber;
	int m_column;
	wxString m_fileName;

public:
	//ctor-dtor, copy constructor and assignment operator
	SearchResult(){}

	virtual ~SearchResult(){}

	SearchResult(const SearchResult &rhs) { *this = rhs; }

	SearchResult& operator=(const SearchResult &rhs){
		if(this == &rhs)
			return *this;
		m_column = rhs.m_column;
		m_lineNumber = rhs.m_lineNumber;
		m_pattern = rhs.m_pattern;
		m_fileName = rhs.m_fileName;
		return *this;
	}
	
	//------------------------------------------------------
	// Setters/getters
	void SetPattern(const wxString &pat){ m_pattern = pat;}
	void SetLineNumber(const int &line){ m_lineNumber = line; }
	void SetColumn(const int &col){ m_column = col;}
	void SetFileName(const wxString &fileName) { m_fileName = fileName; }

	const int& GetLineNumber() const { return m_lineNumber; }
	const int& GetColumn() const { return m_column; }
	const wxString &GetPattern() const { return m_pattern; }
	const wxString &GetFileName() const { return m_fileName; }

	// return a foramtted message
	wxString GetMessage() const 
	{
		wxString msg;
		msg << GetFileName()
			<< wxT("(")
			<< GetLineNumber() 
			<< wxT(",")
			<< GetColumn()
			<< wxT("): ")
			<< GetPattern();
		return msg;
	}
};

typedef std::list<SearchResult> SearchResultList;


class SearchSummary : public wxObject {
	int m_fileScanned;
	int m_matchesFound;

public:
	SearchSummary() : m_fileScanned(0), m_matchesFound(0) {}
	virtual ~SearchSummary(){};

	SearchSummary(const SearchSummary& rhs){
		*this = rhs;
	}

	SearchSummary& operator=(const SearchSummary &rhs){
		if(this == &rhs)
			return *this;

		m_fileScanned = rhs.m_fileScanned;
		m_matchesFound = rhs.m_matchesFound;
		return *this;
	}

	int GetNumFileScanned() const { return m_fileScanned; }
	int GetNumMatchesFound() const { return m_matchesFound; }
	
	void SetNumFileScanned(const int &num) { m_fileScanned = num; }
	void SetNumMatchesFound(const int &num) { m_matchesFound = num; }

	wxString GetMessage() const { 
		wxString msg(wxT("Number of files scanned: "));
		msg << m_fileScanned;
		msg << wxT(" Matches found: ");
		msg << m_matchesFound; 
		return msg;
	}

};

//----------------------------------------------------------
// The search thread 
//----------------------------------------------------------

class SearchThread : public wxThread
{
	friend class Singleton<SearchThread>;
	wxCriticalSection m_cs;
	wxEvtHandler *m_notifiedWindow;
	std::deque<SearchData*> m_queue;
	wxString m_wordChars;
	std::map<wxChar, bool> m_wordCharsMap; //< Internal
	SearchResultList m_results;
	bool m_stopSearch;
	SearchSummary m_summary;
	wxString m_reExpr;
	wxRegEx m_regex;
	bool m_matchCase;

private:
	/**
	 * Default constructor.
	 */
	SearchThread();

	/**
	 * Destructor.
	 */
	virtual ~SearchThread();

public:
	/**
	 * Thread execution point.
	 */
	void* Entry();

	/**
	 * Called when the thread exits
	 * whether it terminates normally or is stopped with Delete() (but not when it is Kill()'ed!)
	 */
	virtual void OnExit(){};

	/**
	 * Add a request to the search thread to start
	 * \param data SearchData class 
	 */
	void PerformSearch(const SearchData &data);

	/**
	 * Set the window to be notified about the progress
	 * \param evtHandler
	 */
	void SetNotifyWindow( wxEvtHandler* evtHandler ) { m_notifiedWindow  = evtHandler; }

	/**
	 * Stops the thread
	 * This function returns only when the thread is terminated.
	 * \note This call must be called from the context of other thread (e.g. main thread)
	 */
	void Stop();

	/**
	 * Stops the current search operation
	 * \note This call must be called from the context of other thread (e.g. main thread)
	 */
	void StopSearch(bool stop = true);

	/**
	 * Start the thread as joinable thread.
	 * \note This call must be called from the context of other thread (e.g. main thread)
	 */
	void Start();

	/**
	 *  The search thread has several functions that operate on words, 
	 *  which are defined to be contiguous sequences of characters from a particular set of characters. 
	 *  Defines which characters are members of that set. The default is set to:
	 * "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
	 * \param chars sequence of characters that are considered part of a word
	 */
	void SetWordChars(const wxString &chars);

private:
	/**
	 * Index the word chars from the array into a map
	 */
	void IndexWordChars();

	/**
	 * Try to get a request from the ipc queue
	 * \param data output, return the searchdata from the queue
	 * \return true if test succeeded, false otherwise
	 */
	bool TestRequest(SearchData **data);

	// Test to see if user asked to cancel the search
	bool TestStopSearch();

	/**
	 * Do the actual search operation
	 * \param data inpunt contains information about the search
	 */
	void DoSearchFiles(const SearchData *data);

	// Perform search on a single file
	void DoSearchFile(const wxString &fileName, const SearchData *data);
	
	// Perform search on a line
	void DoSearchLine(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data);

	// Perform search on a line using regular expression
	void DoSearchLineRE(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data);

	// Send an event to the notified window
	void SendEvent(wxEventType type);

	// return a compiled regex object for the expression
	wxRegEx &GetRegex(const wxString &expr, bool matchCase);

	// Internal function 
	bool AdjustLine(wxString &line, int &pos, wxString &findString);

};

typedef Singleton<SearchThread> SearchThreadST;

DECLARE_EVENT_TYPE(wxEVT_SEARCH_THREAD_MATCHFOUND, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHEND, wxID_ANY)
DECLARE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxID_ANY)

#endif // SEARCH_THREAD_H
