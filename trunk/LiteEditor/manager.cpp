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
#include "wx/arrstr.h"
#include "context_manager.h"
#include "wx/tokenzr.h"

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

void Manager::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position)
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
	if( position != wxNOT_FOUND ){
		editor->SetCaretAt( position );
	} else {
		editor->GotoLine( lineno );
	}

	editor->SetProject( projectName );
	editor->SetFocus ();
	editor->SetSCIFocus (true);
}

/// Open file and set the cursor to be on the line
void Manager::OpenFile(const TagEntry& tag)
{
	OpenFile(tag.GetFile(), tag.GetProject(), tag.GetLine() - 1, tag.GetPosition());
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
	ContextManager::Free();

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

void Manager::CreateEnvironmentVars(const wxString &path)
{
	//initialize some environment variable to be available for this workspace
	EnvironmentVarieblesPtr env = GetEnvironmentVariables();
	wxFileName filepath(path);
	env->SetEnv(wxT("WorkspaceName"), WorkspaceST::Get()->GetName());
	env->SetEnv(wxT("WorkspacePath"), filepath.GetPath());
	SetEnvironmentVariables(env);
}

void Manager::CreateWorkspace(const wxString &name, const wxString &path, const CtagsOptions &options)
{
	// make sure that the workspace pane is visible
	ShowWorkspacePane(WorkspacePane::FILE_VIEW);

	wxString errMsg;
	TagsManagerST::Get()->SetCtagsOptions(options);
	bool res = WorkspaceST::Get()->CreateWorkspace(name, path, options, errMsg);
	CHECK_MSGBOX(res);

	//initialize some environment variable to be available for this workspace
	CreateEnvironmentVars(path);

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
	// make sure that the workspace pane is visible
	ShowWorkspacePane(WorkspacePane::FILE_VIEW);

	wxString errMsg;
	bool res = WorkspaceST::Get()->OpenWorkspace(path, errMsg);
	CHECK_MSGBOX(res);
	
	// update status bar
	wxString dbfile = WorkspaceST::Get()->GetStringProperty(wxT("Database"), errMsg);
	wxString exDbfile = WorkspaceST::Get()->GetStringProperty(wxT("ExternalDatabase"), errMsg);
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("Workspace DB: '%s'"), dbfile.GetData()), 1);
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("External DB: '%s'"), exDbfile.GetData()), 2);

	// load ctags options
	CtagsOptions options = WorkspaceST::Get()->LoadCtagsOptions();
	TagsManagerST::Get()->SetCtagsOptions( options );
	TagsManagerST::Get()->ParseComments(options.GetParseComments());
	
	//initialize some environment variable to be available for this workspace
	CreateEnvironmentVars(path);

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

	ProjectPtr p = WorkspaceST::Get()->FindProjectByName(fn.GetName(), errMsg);
	if( !p ){
		return;
	}

	// Get list of all files from the project and parse them
	std::vector<wxFileName> vList;
	p->GetFiles(vList);

	// Parse & Store
	TagTreePtr ttp;
	std::vector<DbRecordPtr> comments;
	if (TagsManagerST::Get()->GetParseComments()) {
		ttp = TagsManagerST::Get()->ParseSourceFiles(vList, p->GetName(), &comments);
		TagsManagerST::Get()->StoreComments(comments);
	} else {
		ttp = TagsManagerST::Get()->ParseSourceFiles(vList, p->GetName());
	}

	TagsManagerST::Get()->Store(ttp);
	
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

	// Notify the symbol tree to update the gui
	SymbolTreeEvent evt(name, wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT);
	wxPostEvent(Frame::Get()->GetWorkspacePane()->GetSymbolTree(), evt);
	Frame::Get()->GetWorkspacePane()->GetFileViewTree()->BuildTree();
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
	wxString project = virtualDirFullPath.BeforeFirst(wxT(':'));
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if( !p ){
		return;
	}

	wxString vdPath = virtualDirFullPath.AfterFirst(wxT(':'));
	wxArrayString files;
	p->GetFilesByVirtualDir(vdPath, files);
	for(size_t i=0; i<files.Count(); i++){
		TagsManagerST::Get()->Delete(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), project, files.Item(i));
	}

	// Update 
	TagTreePtr dummy;
	Frame::Get()->GetWorkspacePane()->GetSymbolTree()->BuildTree(dummy );

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

	TagTreePtr ttp;
	if( project.IsEmpty() == false ){
		std::vector<DbRecordPtr> comments;
		if(TagsManagerST::Get()->GetParseComments()){
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName, project, &comments);
			TagsManagerST::Get()->StoreComments(comments);
		}else{
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName, project);
		}
		TagsManagerST::Get()->Store(ttp);

		// Update 
		TagTreePtr dummy;
		Frame::Get()->GetWorkspacePane()->GetSymbolTree()->BuildTree(dummy );
	}
}

bool Manager::RemoveFile(const wxString &fileName, const wxString &vdFullPath)
{
	// First, close any open tab with this file
	wxFlatNotebook* nb = dynamic_cast<wxFlatNotebook*>(Frame::Get()->GetNotebook());
	if( !nb ){
		return false;	
	}

	wxString project = vdFullPath.BeforeFirst(wxT(':'));
	int count = nb->GetPageCount();
	for(int i=0; i<count; i++){
		LEditor *editor = dynamic_cast<LEditor*>(nb->GetPage(static_cast<size_t>(i)));
		if( editor ){
			if( editor->GetFileName().GetFullPath() == fileName && editor->GetProject() == project){
				nb->DeletePage(static_cast<size_t>(i));
				break;
			}
		}
	}

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile(vdFullPath, fileName, errMsg);
	CHECK_MSGBOX_BOOL(res);

	if( project.IsEmpty() == false ){
		// Remove the file from the tags database as well
		TagsManagerST::Get()->Delete(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), project, fileName);
		
		// Update 
		TagTreePtr dummy;
		Frame::Get()->GetWorkspacePane()->GetSymbolTree()->BuildTree(dummy );
	}
	return true;
}

wxString Manager::GetProjectCwd(const wxString &project) const 
{
	wxString errMsg;
	ProjectPtr p = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	if ( !p ){
		return wxGetCwd();
	}

	wxFileName projectFileName(p->GetFileName());
	projectFileName.MakeAbsolute();
	return projectFileName.GetPath();
}

void Manager::ShowOutputPane(wxString focusWin){
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
	if( info.IsOk() ){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

	// set the selection to focus win
	OutputPane *pane = Frame::Get()->GetOutputPane();
	int index = pane->CaptionToIndex(focusWin);
	if( index != wxNOT_FOUND ){
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}

void Manager::ShowWorkspacePane(wxString focusWin){
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
	if( info.IsOk() ){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

	// set the selection to focus win
	WorkspacePane *pane = Frame::Get()->GetWorkspacePane();
	int index = pane->CaptionToIndex(focusWin);
	if( index != wxNOT_FOUND ){
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}

void Manager::HideOutputPane()
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
	if( info.IsOk() ){
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}
}

void Manager::HideWorkspacePane()
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
	if( info.IsOk() ){
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}	
}

void Manager::SetWorkspaceCtagsOptions(const CtagsOptions &options)
{
	WorkspaceST::Get()->SaveCtagsOptions(options);
}


CtagsOptions Manager::GetWorkspaceCtagsOptions() const
{
	return WorkspaceST::Get()->LoadCtagsOptions();
}

ContextBasePtr Manager::NewContextByFileName(const wxFileName &fileName, LEditor *parent) const
{
	EditorConfigCookie cookie;

	LexerConfPtr lexer = EditorConfigST::Get()->GetFirstLexer(cookie);
	while( lexer ) {
		wxString lexExt = lexer->GetFileSpec();

		wxStringTokenizer tkz(lexExt, wxT(";"));
		while(tkz.HasMoreTokens()){
			if(wxMatchWild(tkz.NextToken(), fileName.GetFullName())){
				return ContextManager::Get()->NewContext(parent, lexer->GetName());
			}
		}
		
		lexer = EditorConfigST::Get()->GetNextLexer(cookie);
	}

	// return the default context
	return ContextManager::Get()->NewContext(parent, wxT("Default"));
}

void Manager::ApplySettingsChanges()
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = (size_t)book->GetPageCount();
	for(size_t i=0; i<count; i++){
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if(editor){
			// the open page is of type LEditor
			editor->SetSyntaxHighlight(editor->GetContext()->GetName());
		}
	}
}

ProjectSettingsPtr Manager::GetProjectSettings(const wxString &projectName) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if( !proj ){
		wxMessageBox(errMsg, wxT("Lite Editor"), wxOK | wxICON_HAND);
		return NULL;
	}
	
	return proj->GetSettings();
}

void Manager::SetProjectSettings(const wxString &projectName, ProjectSettingsPtr settings)
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
	if( !proj ){
		wxMessageBox(errMsg, wxT("Lite Editor"), wxOK | wxICON_HAND);
		return;
	}
	
	proj->SetSettings(settings);
}

BuildMatrixPtr Manager::GetWorkspaceBuildMatrix() const
{
	return WorkspaceST::Get()->GetBuildMatrix();
}

void Manager::SetWorkspaceBuildMatrix(BuildMatrixPtr matrix)
{
	WorkspaceST::Get()->SetBuildMatrix(matrix);
}

void Manager::TogglePanes()
{
	static bool workspaceShown = false;
	static bool outputShown = false;
	static bool toggled = false;

	if(!toggled){
		wxAuiPaneInfo info;
		info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
		if( info.IsOk() ){
			outputShown = info.IsShown();
		}
		info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
		if( info.IsOk() ){
			workspaceShown = info.IsShown();
		}
		HideOutputPane();
		HideWorkspacePane();
		toggled = true;
	}else{
		if(outputShown){
			ShowOutputPane();
		}
		if(workspaceShown){
			ShowWorkspacePane();
		}
		toggled = false;
	}
}

void Manager::SetEnvironmentVariables(EnvironmentVarieblesPtr env)
{
	WorkspaceST::Get()->SetEnvironmentVariables(env);
}

EnvironmentVarieblesPtr Manager::GetEnvironmentVariables() const
{
	return WorkspaceST::Get()->GetEnvironmentVariables();
}
