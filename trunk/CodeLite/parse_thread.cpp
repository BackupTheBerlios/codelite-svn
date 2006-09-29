#include "precompiled_header.h"

#include "parse_thread.h"
#include "ctags_manager.h"
#include "tags_database.h"
#include <wx/stopwatch.h>

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

namespace codelite
{

#ifdef USE_TRACE
#define DEBUG_START_TIMER(msg) { wxString logmsg; m_watch.Start(); wxLogMessage(logmsg << wxT("Timer started ===> ") << msg); }
#define DEBUG_STOP_TIMER()   { wxString msg; msg << wxT("Done, total time elapsed: ") << m_watch.Time() << wxT(" milliseconds"); wxLogMessage(msg); }
#else
#define DEBUG_START_TIMER(msg)
#define DEBUG_STOP_TIMER()
#endif

IMPLEMENT_DYNAMIC_CLASS(SymbolTreeEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT)

ParseThread::ParseThread()
: wxThread(wxTHREAD_JOINABLE)
, m_pDb( new TagsDatabase() )
, m_notifiedWindow( NULL )
{
}

ParseThread::~ParseThread()
{
}

void* ParseThread::Entry()
{
	while( true )
	{
		// Did we get a request to terminate?
		if(TestDestroy())
			break;

		wxString sourceFile;
		wxString project;
		wxString dbfile;
		
		if( GetRequest( sourceFile, project, dbfile ) )
		{
			DEBUG_MSG( wxT("Processing request for Project:") << project << wxT(" File: ") << sourceFile << wxT(" DB: ") << dbfile)
			ProcessRequest( sourceFile, project, dbfile );
			wxThread::Sleep(10);  // Allow other threads to work as well
			continue; // to avoid the sleep
		}

		// Sleep for 1 seconds, and then try again
		wxThread::Sleep(1000);
	}
	return NULL;
}

void ParseThread::Add(const wxString& file, const wxString& project, const wxString& dbfile)
{
	std::pair<wxString, wxString> p;
	p.first = project;
	p.second = file;

	{
		wxCriticalSectionLocker locker(m_cs);
		m_requests[p] = dbfile;
	}
}

bool ParseThread::GetRequest(wxString& file, wxString& project, wxString& dbfile)
{
	wxCriticalSectionLocker locker(m_cs);
	std::map<std::pair<wxString, wxString>, wxString>::iterator iter = m_requests.begin();
	if( iter != m_requests.end() )
	{
		project = iter->first.first;
		file    = iter->first.second;
		dbfile  = iter->second;
		m_requests.erase( iter );
		return true;
	}
	return false;
}

void ParseThread::ProcessRequest(wxString& file, wxString& project, wxString& dbfile)
{
	//----------------------------------------------
	// Build a tree from project/file/project 
	// from the value which are set in the database
	//----------------------------------------------
	DEBUG_START_TIMER(wxT("Creating old tree from database"));
	TagTreePtr oldTree;
	try
	{
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByProjectAndFile(project, file, wxFileName(dbfile));
		
		// Load the records and build a language tree
		TagEntry root;
		root.SetName(wxT("<ROOT>"));
		oldTree.Reset( new TagTree(wxT("<ROOT>"), root) );
		while( rs.NextRow() )
		{
			TagEntry entry(rs);
			oldTree->AddEntry(entry);
		}
		rs.Finalize();
	}
	catch (wxSQLite3Exception& e)
	{
		std::cerr << e.GetErrorCode() << ":" << e.GetMessage().mb_str() << std::endl;
	}

	DEBUG_STOP_TIMER()
	
	// Build second tree from the updated file
	TagTreePtr newTree;
	std::vector<DbRecordPtr> comments;
	if( TagsManagerST::Get()->GetParseComments() )
	{
		DEBUG_START_TIMER(wxT("Building new syntax tree from source file (comments inclded)"));
		newTree = TagsManagerST::Get()->ParseSourceFile(wxFileName(file), project, &comments);
	}
	else
	{
		DEBUG_START_TIMER(wxT("Building new syntax tree from source file"))
		newTree = TagsManagerST::Get()->ParseSourceFile(wxFileName(file), project);
	}
	DEBUG_STOP_TIMER()
	
	//-------------------------------------------------------------------
	// Now what is left to be done here, is to update the GUI tree
	// The GUI tree needs to be updated item by item, to avoid total tree
	// Collapsing
	//-------------------------------------------------------------------

	// Compare old tree vs new tree
	std::vector<std::pair<wxString, TagEntry> >  deletedItems;
	std::vector<std::pair<wxString, TagEntry> >  newItems;
	std::vector<std::pair<wxString, TagEntry> >  modifiedItems;

	DEBUG_START_TIMER(wxT("Comparing trees ..."))
	oldTree->Compare(newTree.Get(), deletedItems, modifiedItems, newItems);
	DEBUG_STOP_TIMER()

	// Delete old entries
	size_t i=0;
	m_pDb->Begin();
	
	DEBUG_START_TIMER(wxT("Updating database ..."))
	
	// Prepare sql statements
	TagEntry dummy;

	wxSQLite3Statement insertStmt = m_pDb->PrepareStatement( dummy.GetInsertOneStatement() );
	wxSQLite3Statement updateStmt = m_pDb->PrepareStatement( dummy.GetUpdateOneStatement() );
	wxSQLite3Statement deleteStmt = m_pDb->PrepareStatement( dummy.GetDeleteOneStatement() );

	if( TagsManagerST::Get()->GetParseComments() )
	{
		// drop all old entries from this file
		try
		{
			wxString sql;
			sql << wxT("DELETE FROM COMMENTS WHERE file='") << file << wxT("'");
			m_pDb->ExecuteUpdate( sql );
			m_pDb->Store( comments, wxFileName(), false );
		}
		catch( wxSQLite3Exception & e)
		{
			DEBUG_MSG(e.GetMessage());
		}
	}

	for(i=0; i<deletedItems.size(); i++)
		deletedItems[i].second.Delete(deleteStmt);

	for(i=0; i<newItems.size(); i++)
		newItems[i].second.Store(insertStmt);

	for(i=0; i<modifiedItems.size(); i++)
		modifiedItems[i].second.Update(updateStmt);

	m_pDb->Commit();

	insertStmt.Finalize();
	updateStmt.Finalize();
	deleteStmt.Finalize();
	DEBUG_STOP_TIMER()

	DEBUG_MSG( (int)newItems.size() << wxT(" Added, ") << (int)deletedItems.size() << wxT(" Deleted, ") << (int)modifiedItems.size() << wxT(" Updated.") );

	// If there is no event handler set to handle this comaprison 
	// results, then nothing more to be done
	if(m_notifiedWindow == NULL)
		return;

	// Send an event for each operation type
	DEBUG_START_TIMER(wxT("Sending events to Symbol Tree"));

	if( !deletedItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, deletedItems);

	if( !newItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, newItems);

	if( !modifiedItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, modifiedItems);
}

void ParseThread::Stop()
{
	// Notify the thread to stop
	// and wait for its termination
	if( IsAlive() )
		Delete();
	
	while( IsAlive() )
	{
		wxThread::Sleep( 1 );
	}
}

void ParseThread::Start()
{
	Create();
	Run();	 
}

void ParseThread::SendEvent(int evtType, std::vector<std::pair<wxString, TagEntry> >  &items)
{
	SymbolTreeEvent event(items, evtType);
	wxPostEvent(m_notifiedWindow, event);
}

} // codelite
