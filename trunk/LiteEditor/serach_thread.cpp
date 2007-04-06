#include "search_thread.h"
#include "wx/event.h"
#include "dirtraverser.h"
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <iostream>
#include <wx/tokenzr.h>

#define ADJUST_LINE_AND_CONT(modLine, pos, findString)	\
{														\
	if( !AdjustLine(modLine, pos, findString) ){		\
		break;											\
	} else {											\
		continue;										\
	}													\
}

DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_MATCHFOUND)
DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHEND)
DEFINE_EVENT_TYPE(wxEVT_SEARCH_THREAD_SEARCHCANCELED)

//----------------------------------------------------------------
// SearchData
//----------------------------------------------------------------

std::map<wxString, bool> SearchData::GetExtensions() const 
{
	std::map<wxString, bool> exts;
	wxStringTokenizer tok(m_validExt, wxT(";"));
	
	// return empty map to indicate search in all...
	if(m_validExt == wxT("*.*") || m_validExt == wxT("*")){
		return exts;
	}

	while( tok.HasMoreTokens() ) {
		wxString token = tok.GetNextToken();
		token = token.AfterFirst(wxT('.'));
		exts[token] = true;
	}
	return exts;
}

//----------------------------------------------------------------
// SearchThread
//----------------------------------------------------------------

SearchThread::SearchThread()
: wxThread(wxTHREAD_JOINABLE)
, m_notifiedWindow( NULL )
, m_wordChars(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"))
{
	IndexWordChars();
}

SearchThread::~SearchThread()
{
	if( !m_queue.empty() ){
		std::deque<SearchData*>::iterator iter = m_queue.begin();
		for(; iter != m_queue.end(); iter++){
			delete (*iter);
		}
		m_queue.clear();
	}
}

void SearchThread::IndexWordChars()
{
	m_wordCharsMap.clear();
	for(size_t i=0; i<m_wordChars.Length(); i++){
		m_wordCharsMap[m_wordChars.GetChar(i)] = true;
	}
}

void SearchThread::SetWordChars(const wxString &chars)
{
	m_wordChars = chars;
	IndexWordChars();
}

void* SearchThread::Entry()
{
	SearchData *data;
	while( true )
	{
		// Did we get a request to terminate?
		if(TestDestroy())
			break;

		// Clean the previous search data
		data = NULL;
		if( TestRequest( &data ) ){

			m_summary = SearchSummary();
			DoSearchFiles(data);
			delete data;

			// Send search end event 
			SendEvent(wxEVT_SEARCH_THREAD_SEARCHEND);
		}
		
		// Sleep for 100 seconds, and then try again
		wxThread::Sleep(100);
	}
	return NULL;
}

void SearchThread::PerformSearch(const SearchData &data)
{
	wxCriticalSectionLocker locker(m_cs);
	m_queue.push_back( new SearchData(data) );
}

void SearchThread::DoSearchFiles(const SearchData *data)
{
	wxUnusedVar(data);
	DirTraverser traverser;

	// Get all files
	traverser.SetExtensions(data->GetExtensions());
	if( data->GetRootDir().IsEmpty())
		return;

	if( data->GetFindString().IsEmpty() )
		return;

	StopSearch(false);

	wxDir dir(data->GetRootDir());
	dir.Traverse(traverser);

	std::vector<wxString> files = traverser.GetFiles();
	for(size_t i=0; i<files.size(); i++){
		
		m_summary.SetNumFileScanned((int)i+1);

		// give user chance to cancel the search ...
		if( TestStopSearch() ) {
			// Send cancel event
			SendEvent(wxEVT_SEARCH_THREAD_SEARCHCANCELED);
			StopSearch(false);
			break;
		}

		DoSearchFile(files[i], data);
	}
}

bool SearchThread::TestStopSearch()
{
	bool stop = false;
	{
		wxCriticalSectionLocker locker(m_cs);
		stop = m_stopSearch;
	}
	return stop;
}

void SearchThread::StopSearch(bool stop)
{
	wxCriticalSectionLocker locker(m_cs);
	m_stopSearch = stop;
}

void SearchThread::DoSearchFile(const wxString &fileName, const SearchData *data)
{
	wxFileInputStream input(fileName);
	if(!input.IsOk())
		return;

	wxTextInputStream text( input );

	// Process single lines
	int lineNumber = 1;
	

	if( data->IsRegularExpression() ){
		// regular expression search
	} else {
		// simple search
		while( !input.Eof() ){

			// Read the next line
			wxString line = text.ReadLine();
			DoSearchLine(line, lineNumber, fileName, data);
			lineNumber++;
		}
	}

	if( m_results.empty() == false )
		SendEvent(wxEVT_SEARCH_THREAD_MATCHFOUND);
}

void SearchThread::DoSearchLine(const wxString &line, const int lineNum, const wxString &fileName, const SearchData *data)
{
	wxString findString = data->GetFindString();
	wxString modLine = line;

	if( !data->IsMatchCase() ){
		modLine = modLine.MakeLower();
		findString = findString.MakeLower();
	}

	int pos = 0;
	while( pos != wxNOT_FOUND ){
		pos = modLine.Find(findString);
		if(pos != wxNOT_FOUND){
			// we have a match
			if( data->IsMatchWholeWord() ){

				// make sure that the word before is not in the wordChars map
				if((pos > 0) && (m_wordCharsMap.find(modLine.GetChar(pos-1)) != m_wordCharsMap.end()) )
					ADJUST_LINE_AND_CONT(modLine, pos, findString);

				// if we have more characters to the right, make sure that the first char does not match any
				// in the wordCharsMap
				if(pos + findString.Length() <= modLine.Length()){
					wxChar nextCh = modLine.GetChar(pos+findString.Length());
					if(m_wordCharsMap.find(nextCh) != m_wordCharsMap.end())
						ADJUST_LINE_AND_CONT(modLine, pos, findString);
				}		
			} 

			m_summary.SetNumMatchesFound(m_summary.GetNumMatchesFound() + 1);

			// Notify our match
			SearchResult result;
			result.SetColumn(pos);
			result.SetLineNumber(lineNum);
			result.SetPattern(line);
			result.SetFileName(fileName);
			m_results.push_back(result);

			ADJUST_LINE_AND_CONT(modLine, pos, findString);
		}
	}
}

bool SearchThread::AdjustLine(wxString &line, int pos, wxString &findString)
{
	// adjust the current line
	if( line.Length() - (pos + findString.Length()) >= findString.Length()){
		line = line.Right(line.Length() - (pos + findString.Length()));
		return true;
	} else {
		return false;
	}
}

void SearchThread::Stop()
{
	// Notify the thread to stop
	// and wait for its termination
	if( IsAlive() )
		Delete();
	
	while( IsAlive() )
	{
		wxThread::Sleep( 10 );
	}
}

void SearchThread::Start()
{
	Create();
	Run();	 
}

// Pop a request from the queue
bool SearchThread::TestRequest(SearchData **data)
{
	wxCriticalSectionLocker locker( m_cs );
	if( m_queue.empty() )
		return false;

	*data = m_queue.front();
	
	m_queue.pop_front();
	return true;
}

void SearchThread::SendEvent(wxEventType type)
{
	if( !m_notifiedWindow )
		return;

	wxCommandEvent event(type, GetId());

	if(type == wxEVT_SEARCH_THREAD_MATCHFOUND)
	{
		event.SetClientData( new SearchResultList(m_results) );
		m_results.clear();
	}
	else if(type == wxEVT_SEARCH_THREAD_SEARCHEND)
	{
		// Nothing to do
		event.SetClientData( new SearchSummary(m_summary) );
	}
	else if(type == wxEVT_SEARCH_THREAD_SEARCHCANCELED)
	{
		event.SetString(wxT("Search cancelled by user"));
	}

	if( m_notifiedWindow ){
		::wxPostEvent(m_notifiedWindow, event);
	}
	wxThread::Sleep(10);
}
