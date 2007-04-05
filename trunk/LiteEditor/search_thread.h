#ifndef SEARCH_THREAD_H
#define SEARCH_THREAD_H

#include <deque>
#include <wx/string.h>
#include <map>
#include <wx/thread.h>
#include "singleton.h"

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
	std::map<wxString, bool> m_validExt;
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

	SearchData(const wxString &rootDir, const wxString &findString, const int flags, std::map<wxString, bool> exts)
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
	void SetRootDir(const wxString &rootDir) { m_rootDir = rootDir; }
};

//----------------------------------------------------------
// The search thread 
//----------------------------------------------------------

class SearchThread : public wxThread
{
	friend class Singleton<SearchThread>;
	wxCriticalSection m_cs;
	wxEvtHandler *m_notifiedWindow;
	std::deque<SearchData> m_queue;

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
	 * Start the thread as joinable thread.
	 * \note This call must be called from the context of other thread (e.g. main thread)
	 */
	void Start();

private:

	/**
	 * Try to get a request from the ipc queue
	 * \param data output, return the searchdata from the queue
	 * \return true if test succeeded, false otherwise
	 */
	bool TestRequest(SearchData &data);
};

typedef Singleton<SearchThread> SearchThreadST;

#endif // SEARCH_THREAD_H
