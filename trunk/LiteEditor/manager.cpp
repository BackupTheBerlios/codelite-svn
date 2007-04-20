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
#include "art_manager.h"
#include <wx/file.h>

#define CHECK_MSGBOX(res)									\
if( !res )													\
{															\
	wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
	return;													\
}

#define CHECK_MSGBOX_BOOL(res)								\
if( !res )													\
{															\
	wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);	\
	return res;												\
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

void Manager::OpenFile(const wxString &file_name, const wxString &projectName, const int lineno)
{
	wxFileName fileName(file_name);
	wxFlatNotebook *notebook = Frame::Get()->GetNotebook();
	// Search to see if this file is already opened
	// in the notebook
	LEditor* editor = NULL;
	size_t nCount = 0;
	for(; nCount < (size_t)notebook ->GetPageCount(); nCount++)
	{
		editor = dynamic_cast<LEditor*>(notebook ->GetPage(nCount));
		if( editor ){
			if( editor->GetFileName() == fileName.GetFullPath() )
			{
				notebook ->SetSelection( nCount );
				break;
			}
			editor = NULL;
		}
	}

	if( !editor )
	{
		/// Open the file and read the text
		if(fileName.IsOk() == false)
			return;
		
		// make sure that the file exist
		if( !fileName.FileExists() )
			return;

		// Create new editor and add it to the notebook
		notebook ->Freeze();
		editor = new LEditor(Frame::Get(), wxID_ANY, wxSize(1, 1), fileName.GetFullPath(), projectName);
		notebook ->AddPage(editor, fileName.GetFullName(), true);
		notebook ->Thaw();
	}

	// Go to tag line number and gives scintilla the focus
	editor->GotoLine( lineno );
	editor->SetProject( projectName );
	editor->SetFocus ();
	editor->SetSCIFocus (true);

}

/// Open file and set the cursor to be on the line
void Manager::OpenFile(const TagEntry& tag)
{
	OpenFile(tag.GetFile(), tag.GetProject(), tag.GetLine() - 1);
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
		LEditor* editor = dynamic_cast<LEditor*>(book->GetPage(i));
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
	wxArtManagerST::Free();
}

void Manager::CreateWorkspace(const wxString &name, const wxString &path)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateWorkspace(name, path, errMsg);
	CHECK_MSGBOX(res);

	DoUpdateGUITrees();
}

void Manager::CreateProject(const wxString &name, const wxString &path, const wxString &type)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateProject(name, path, type, errMsg);
	CHECK_MSGBOX(res);

	TagsManagerST::Get()->CreateProject(name);

	DoUpdateGUITrees();
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

	DoUpdateGUITrees();
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

void Manager::AddProject(const wxString & path)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddProject(path, errMsg);
	CHECK_MSGBOX(res);

	// Create an entry in the CodeLite database
	wxFileName fn(path);
	TagsManagerST::Get()->CreateProject(fn.GetName());

	// Update the trees
	DoUpdateGUITrees();
}

void Manager::DoUpdateGUITrees()
{
	// update symbol tree
	WorkspacePane *wp = Frame::Get()->GetWorkspacePane();
	TagTreePtr dummy;
	wp->BuildSymbolTree( dummy );
	wp->BuildFileTree();
}

bool Manager::RemoveProject(const wxString &name)
{
	if( name.IsEmpty() ){
		return false;
	}

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveProject(name, errMsg);
	CHECK_MSGBOX_BOOL(res);

	//  Update the database
	TagsManagerST::Get()->DeleteProject(name);

	// update gui trees
	DoUpdateGUITrees();
	return true;
}

wxString Manager::GetActiveProjectName()
{
	return WorkspaceST::Get()->GetActiveProjectName();
}

void Manager::SetActiveProject(const wxString &name)
{
	WorkspaceST::Get()->SetActiveProject(WorkspaceST::Get()->GetActiveProjectName(), false);
	WorkspaceST::Get()->SetActiveProject(name, true);
}


void Manager::AddVirtualDirectory(const wxString &virtualDirFullPath)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateVirtualDirectory(virtualDirFullPath, errMsg);
	CHECK_MSGBOX(res);
}

void Manager::RemoveVirtualDirectory(const wxString &virtualDirFullPath)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveVirtualDirectory(virtualDirFullPath, errMsg);
	CHECK_MSGBOX(res);
}

void Manager::SaveWorkspace()
{
	WorkspaceST::Get()->Save();
}

void Manager::AddNewFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxFile file;
	if (!file.Create(fileName.GetData(), true))
		return;

	if(file.IsOpened()){
		file.Close();
	}

	AddFileToProject(fileName, vdFullPath, openIt);
}

void Manager::AddFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxString project;
	project = vdFullPath.BeforeFirst(wxT(':'));

	if( openIt ){
		OpenFile(fileName, project);
	}

	// Add the file to the project
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddNewFile(vdFullPath, fileName, errMsg);
	CHECK_MSGBOX(res);
}
