
#ifndef CODELITE_PARSE_THREAD_H
#define CODELITE_PARSE_THREAD_H

#include "entry.h"
#include "singleton.h"
#include <map>
#include <vector>
#include <memory>
#include <wx/stopwatch.h>
#ifndef WXDLLIMPEXP_SYM_TREE
    #ifdef WXMAKINGDLL_SYM
    #    define WXDLLIMPEXP_SYM_TREE WXEXPORT
    #elif defined(WXUSINGDLL_SYM)
    #    define WXDLLIMPEXP_SYM_TREE WXIMPORT
    #else /* not making nor using FNB as DLL */
    #    define WXDLLIMPEXP_SYM_TREE
    #endif // WXMAKINGDLL_FNB
#endif

class TagsDatabase;

class ParseThread : public wxThread
{
	friend class Singleton<ParseThread>;
	std::auto_ptr<TagsDatabase> m_pDb;
	std::map<std::pair<wxString, wxString>, wxString> m_requests;
	wxCriticalSection m_cs;
	wxEvtHandler *m_notifiedWindow;
	wxStopWatch m_watch;

private:
	/**
	 * Default constructor.
	 */
	ParseThread();

	/**
	 * Destructor.
	 */
	virtual ~ParseThread();

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
	 * Add a request to the parser thread to parse files.
	 * \param file File name
	 * \param project Project which this file belongs too
	 * \param dbfile Database file name
	 */
	void Add(const wxString& file, const wxString& project, const wxString& dbfile);

	/**
	 * Set the window to be notified when a change was done
	 * between current source file tree and the actual tree.
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
	 * Get next request from queue.
	 * \param file [output] source file that was updated
	 * \param project [output] project name where file belongs to
	 * \param dbfile [output] dataabase file name
	 * \return true if there is a request to process
	 */
	bool GetRequest(wxString& file, wxString& project, wxString& dbfile);

	/**
	 * Process request from the editor.
	 * \param file Source file that was updated
	 * \param project Project name where file belongs to
	 * \param dbfile Database file name
	 */
	void ProcessRequest(wxString& file, wxString& project, wxString& dbfile);

	/**
	 * Send an event to the window with an array of items that where changed.
	 * \param evtType Event type to send, one of:
	 * - wxEVT_CMD_DEL_SYMBOL_TREE_ITEMS
	 * - wxEVT_CMD_ADD_SYMBOL_TREE_ITEMS
	 * - wxEVT_CMD_UPD_SYMBOL_TREE_ITEMS
	 * \param items Vector of items that were modified/deleted/added
	 */
	void SendEvent(int evtType, std::vector<std::pair<wxString, TagEntry> >  &items);
};

typedef Singleton<ParseThread> ParseThreadST;

/**
 * Holds information about events associated with SymbolTree object.
 */
class SymbolTreeEvent : public wxNotifyEvent
{
	DECLARE_DYNAMIC_CLASS(SymbolTreeEvent)
	std::vector<std::pair<wxString, TagEntry> >  m_items;
	wxString m_project;

public:
	/**
	 * Constructor
	 * \param commandType Event type
	 * \param winid Window ID
	 * \param key Item key
	 * \param data Item data
	 */
	SymbolTreeEvent(std::vector<std::pair<wxString, TagEntry> >  &items, wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
		, m_items(items)
	{
	}

	/**
	 * Construct event with project name
	 * \param project project name
	 * \param commandType Event type
	 * \param winid Window ID
	 */
	SymbolTreeEvent(const wxString& project, wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
		, m_project(project)
	{
	}

	/**
	 * Copy constructor
	 * \param rhs Right hand side
	 */
	SymbolTreeEvent(const SymbolTreeEvent& rhs)
		: wxNotifyEvent(rhs.GetEventType(), rhs.GetId())
		, m_items(rhs.m_items)
		, m_project(rhs.m_project)
	{
	}

	/**
	 * Clone method to allow the event to be posted between threads.
	 * \return
	 */
	wxEvent *Clone(void) const { return new SymbolTreeEvent(*this); }

	SymbolTreeEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
		: wxNotifyEvent(commandType, winid)
	{}

	std::vector<std::pair<wxString, TagEntry> >& GetItems() { return m_items; }
	const wxString& GetProject() const { return m_project; }
};


BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SYM_TREE, wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, 50300)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SYM_TREE, wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, 50301)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SYM_TREE, wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, 50302)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_SYM_TREE, wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT, 50303)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*SymbolTreeEventFunction)(SymbolTreeEvent&);

#define SymbolTreeEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(SymbolTreeEventFunction, &func)

#define EVT_SYMBOLTREE_ADD_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_DELETE_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_UPDATE_ITEM(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, winid, SymbolTreeEventHandler(fn))

#define EVT_SYMBOLTREE_DELETE_PROJECT(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT, winid, SymbolTreeEventHandler(fn))

#endif // CODELITE_PARSE_THREAD_H
