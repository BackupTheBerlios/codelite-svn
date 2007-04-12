/*!
 * Manager.cpp
 * 
 * Copyright (c) 2007 by Eran Ifrah <eran.ifrah@gmail.com>
 */
#include "manager.h"
#include "ctags_manager.h"
#include "frame.h"
#include "editor.h"
#include "menumanager.h"
#include "language.h"
#include "editor_config.h"
#include "parse_thread.h"
#include "search_thread.h"
#include "workspace.h"
#include "cpp_symbol_tree.h"
#include "fileview.h"

#define CHECK_MSGBOX(res)									\
if( !res )													\
{															\
	wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
	return;													\
}

Manager::Manager(void)
{
}

Manager::~Manager(void)
{
	UnInitialize();
}

wxFrame *Manager::GetMainFrame()
{
	return Frame::Get();
}

bool Manager::IsWorkspaceOpen() const 
{
	return TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().IsOk();
}

void Manager::OpenFile(const TagEntry &tag)
{
	Frame::Get()->OpenFile(tag);
}

const wxString &Manager::GetInstallPath() const
{
	return Frame::Get()->GetInstallPath();
}

void Manager::SetPageTitle(wxWindow *page, const wxString &name)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if( selection != -1 ){
		Frame::Get()->GetNotebook()->SetPageText(selection, name);
	}
}

const wxString Manager::GetPageTitle(wxWindow *page)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if( selection != -1 ) {
		return Frame::Get()->GetNotebook()->GetPageText(selection);
	} else {
		return wxEmptyString;
	}
}

void Manager::SaveAll()
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = book->GetPageCount();

	for(size_t i=0; i<count; i++)
	{
		LEditor* editor = static_cast<LEditor*>(book->GetPage(i));
		if( !editor )
			continue;
		
		if( editor->GetModify() ) 
		{
			editor->SaveFile();
		}
	}
}

void Manager::UnInitialize()
{
	// Release singleton objects
	TagsManagerST::Free();
	LanguageST::Free();
	EditorConfigST::Free();
	WorkspaceST::Free();

	//-----------------------------------------------------
	// Stop the parser thread and release its resources
	// This is required if you want to avoid memory leaks
	// Stopping the parser thread can take up to several 
	// seconds
	// since we block until the thread complets its current 
	// work on the queue
	//-----------------------------------------------------
	ParseThreadST::Get()->Stop();
	ParseThreadST::Free();

	// Stop the search thread and free its resources
	SearchThreadST::Get()->Stop();
	SearchThreadST::Free();

	wxFlatNotebook::CleanUp();
	MenuManager::Free();
}

void Manager::CreateWorkspace(const wxString &name, const wxString &path)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateWorkspace(name, path, errMsg);
	CHECK_MSGBOX(res);

	// Update the symbol tree
	SymbolTree *tree = Frame::Get()->GetSymbolTree();
	TagTreePtr dummy;
	tree->BuildTree( dummy );
}

void Manager::CreateProject(const wxString &name, const wxString &path, const wxString &type)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateProject(name, path, type, errMsg);
	CHECK_MSGBOX(res);

	TagsManagerST::Get()->CreateProject(name);

	// Update the symbol tree
	SymbolTree *tree = Frame::Get()->GetSymbolTree();
	TagTreePtr dummy;
	tree->BuildTree( dummy );
}

void Manager::OpenWorkspace(const wxString &path)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->OpenWorkspace(path, errMsg);
	CHECK_MSGBOX(res);
	
	// update status bar
	wxString dbfile = WorkspaceST::Get()->GetStringProperty(wxT("Database"), errMsg);
	wxString exDbfile = WorkspaceST::Get()->GetStringProperty(wxT("ExternalDatabase"), errMsg);
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("Workspace DB: '%s'"), dbfile.GetData()), 1);
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("External DB: '%s'"), exDbfile.GetData()), 2);

	// update symbol tree
	TagTreePtr dummy;
	Frame::Get()->GetSymbolTree()->BuildTree( dummy );
	Frame::Get()->GetFileViewTree()->BuildTree();
}

ProjectTreePtr Manager::GetProjectFileViewTree(const wxString &projectName)
{
	if( !IsWorkspaceOpen() ){
		return NULL;
	}

	wxString err_msg;
	ProjectPtr prj = WorkspaceST::Get()->FindProjectByName(projectName, err_msg);
	if( !prj ){
		return NULL;
	}
	return prj->AsTree();
}

void Manager::GetProjectList(wxArrayString &list)
{
	WorkspaceST::Get()->GetProjectList(list);
}
