#include "search_thread.h"
#include "wx/event.h"

SearchThread::SearchThread()
: wxThread(wxTHREAD_JOINABLE)
, m_notifiedWindow( NULL )
{
}

SearchThread::~SearchThread()
{
}

void* SearchThread::Entry()
{
	SearchData data;
	while( true )
	{
		// Did we get a request to terminate?
		if(TestDestroy())
			break;

		// Clean the previous search data
		data = SearchData();
		if( TestRequest( data ) ){
			// TODO:: Process the request here
		}

		// Sleep for 100 seconds, and then try again
		wxThread::Sleep(100);
	}
	return NULL;
}

void SearchThread::PerformSearch(const SearchData &data)
{
	wxCriticalSectionLocker locker(m_cs);
	m_queue.push_back( data );
}

void SearchThread::Stop()
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

void SearchThread::Start()
{
	Create();
	Run();	 
}

// Pop a request from the queue
bool SearchThread::TestRequest(SearchData &data)
{
	wxCriticalSectionLocker locker( m_cs );
	if( m_queue.empty() )
		return false;

	data = m_queue.front();
	m_queue.pop_front();
	return true;
}
