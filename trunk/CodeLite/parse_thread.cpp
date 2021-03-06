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


IMPLEMENT_DYNAMIC_CLASS(SymbolTreeEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT)

ParseThread::ParseThread()
: WorkerThread()
, m_pDb( new TagsDatabase() )
{
}

ParseThread::~ParseThread()
{
}

void ParseThread::ProcessRequest(ThreadRequest * request)
{
	ParseRequest *req = (ParseRequest*)request;
	wxString dbfile  = req->dbfile;
	wxString file  = req->file;
	
	//----------------------------------------------
	// Build a tree from project/file/project 
	// from the value which are set in the database
	//----------------------------------------------
	TagTreePtr oldTree;
	try
	{
		wxSQLite3ResultSet rs = m_pDb->SelectTagsByFile(file, wxFileName(dbfile));
		
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
		wxUnusedVar(e);
	}

	// Build second tree from the updated file
	TagTreePtr newTree;
	std::vector<DbRecordPtr> comments;
	if( TagsManagerST::Get()->GetParseComments() )
	{
		newTree = TagsManagerST::Get()->ParseSourceFile(wxFileName(file), &comments);
	}
	else
	{
		newTree = TagsManagerST::Get()->ParseSourceFile(wxFileName(file));
	}
	//-------------------------------------------------------------------
	// Now what is left to be done here, is to update the GUI tree
	// The GUI tree needs to be updated item by item, to avoid total tree
	// Collapsing
	//-------------------------------------------------------------------

	// Compare old tree vs new tree
	std::vector<std::pair<wxString, TagEntry> >  deletedItems;
	std::vector<std::pair<wxString, TagEntry> >  newItems;
	std::vector<std::pair<wxString, TagEntry> >  goodNewItems;
	std::vector<std::pair<wxString, TagEntry> >  modifiedItems;

	oldTree->Compare(newTree.Get(), deletedItems, modifiedItems, newItems);
	// Delete old entries
	size_t i=0;
	m_pDb->Begin();
	
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
			sql << wxT("DELETE FROM COMMENTS WHERE FILE='") << file << wxT("'");
			m_pDb->ExecuteUpdate( sql );
			m_pDb->Store( comments, wxFileName(), false );
		}
		catch( wxSQLite3Exception & e)
		{
			wxUnusedVar(e);
		}
	}

	for(i=0; i<deletedItems.size(); i++)
		deletedItems[i].second.Delete(deleteStmt);

	for(i=0; i<newItems.size(); i++)
	{
		if(newItems[i].second.Store(insertStmt) == TagOk){
			goodNewItems.push_back(newItems[i]);
		}
	}

	for(i=0; i<modifiedItems.size(); i++)
		modifiedItems[i].second.Update(updateStmt);

	m_pDb->Commit();

	insertStmt.Finalize();
	updateStmt.Finalize();
	deleteStmt.Finalize();

	// If there is no event handler set to handle this comaprison 
	// results, then nothing more to be done
	if(m_notifiedWindow == NULL)
		return;

	// Send an event for each operation type
	if( !deletedItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_DELETE_ITEM, deletedItems);

	if( !newItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_ADD_ITEM, goodNewItems);

	if( !modifiedItems.empty() )
		SendEvent(wxEVT_COMMAND_SYMBOL_TREE_UPDATE_ITEM, modifiedItems);
}


void ParseThread::SendEvent(int evtType, std::vector<std::pair<wxString, TagEntry> >  &items)
{
	SymbolTreeEvent event(items, evtType);
	wxPostEvent(m_notifiedWindow, event);
}

