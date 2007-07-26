/*!
 * Manager.cpp
 * 
 * Copyright (c) 2007 by Eran Ifrah <eran.ifrah@gmail.com>
 */
#include "precompiled_header.h"
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
#include <wx/file.h>
#include "wx/arrstr.h"
#include "context_manager.h"
#include "wx/tokenzr.h"
#include "buildmanager.h"
#include "macros.h"
#include "dirtraverser.h"
#include "depends_dlg.h"
#include "build_settings_config.h"
#include "dirsaver.h"
#include "editor_creator.h"
#include "algorithm"
#include "async_executable_cmd.h"
#include "fileutils.h"
#include "VariableLexer.h"
#include "MakefileParser.h"
#include "TargetLexer.h"
#include "workspace_pane.h"

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
: m_cleanRequest(NULL)
, m_compileRequest(NULL)
, m_asyncExeCmd(NULL)
{
}

Manager::~Manager(void)
{
	UnInitialize();
	if(m_cleanRequest){
		delete m_cleanRequest;
		m_cleanRequest = NULL;
	}
	if(m_compileRequest){
		delete m_compileRequest;
		m_compileRequest = NULL;
	}
}

wxFrame *Manager::GetMainFrame()
{
	return Frame::Get();
}

bool Manager::IsWorkspaceOpen() const 
{
	return WorkspaceST::Get()->GetName().IsEmpty() == false;
}

void Manager::OpenFile(const wxString &file_name, const wxString &projectName, int lineno, long position)
{
	wxFileName fileName(file_name);
	wxString projName(projectName);
	wxFlatNotebook *notebook = Frame::Get()->GetNotebook();
	bool updTree = false;
	bool fileWasOpenedInNB(false);

	//make sure that the notebook is visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Editor"));
	if( info.IsOk() && !info.IsShown()){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

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
				fileWasOpenedInNB = true;
				break;
			}
			editor = NULL;
		}
	}

	//even in cases were a porject name is empty, we try 
	//to match a project name to the actual file. otherwise, CC may not work
	//since it depends on a valid project name in the editor
	if(projectName.IsEmpty()){
		projName = GetProjectNameByFile(fileName.GetFullPath());
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
		// create new instance from pool
		editor = EditorCreatorST::Get()->NewInstance();
		editor->Create(fileName, projName);

		notebook ->AddPage(editor, fileName.GetFullName(), true);
		notebook ->Thaw();
		updTree = true;
	}

	// Go to tag line number and gives scintilla the focus
	if( position != wxNOT_FOUND ){
		int lineno = editor->LineFromPosition(position);
		editor->GotoLine( lineno );
		editor->SetCaretAt( position );
	} else {
		if(!(fileWasOpenedInNB && lineno == wxNOT_FOUND)){
			editor->GotoLine( lineno );
		}
	}

	//update the symbol tree
	if(updTree && !editor->GetProject().IsEmpty())
		Frame::Get()->GetWorkspacePane()->BuildSymbolTree(fileName);

	//update the 'Recent file' history
	AddToRecentlyOpenedFiles(fileName.GetFullPath());

	editor->SetProject( projName );
	editor->SetActive();
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

void Manager::SaveAll(bool includeUntitled)
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = book->GetPageCount();

	for(size_t i=0; i<count; i++)
	{
		LEditor* editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if( !editor )
			continue;
		
		//if 'includeUntitled' is not true, dont save new documents that have
		//not been saved to disk yet
		if(!includeUntitled && editor->GetFileName().GetFullPath().StartsWith(wxT("Untitled"))){
			continue;
		}
		
		if(editor->GetModify()) 
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
	BuildManagerST::Free();
	BuildSettingsConfigST::Free();

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
	
	//stop the creator thread
	EditorCreatorST::Free();

	wxFlatNotebook::CleanUp();
	MenuManager::Free();
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

void Manager::CreateWorkspace(const wxString &name, const wxString &path)
{
	// make sure that the workspace pane is visible
	ShowWorkspacePane(WorkspacePane::FILE_VIEW);

	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateWorkspace(name, path, errMsg);
	CHECK_MSGBOX(res);
	OpenWorkspace(path + PATH_SEP + name + wxT(".workspace"));
}

void Manager::CreateProject(ProjectData &data)
{
	wxString errMsg;
	bool res = WorkspaceST::Get()->CreateProject(data.m_name, 
												 data.m_path, 
												 data.m_srcProject->GetSettings()->GetProjectType(), 
												 errMsg);
	CHECK_MSGBOX(res);
	//set the compiler type
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(data.m_name, errMsg);

	//copy the project settings to the new one
	ProjectSettingsPtr settings = proj->GetSettings();
	proj->SetSettings(data.m_srcProject->GetSettings());
	RebuildFileView();
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
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("Workspace DB: '%s'"), dbfile.GetData()), 1);
	
	// load ctags options
	wxBusyCursor cursor;
	
	//initialize some environment variable to be available for this workspace
	CreateEnvironmentVars(path);

	TagTreePtr dummy;
	Frame::Get()->GetWorkspacePane()->BuildFileTree();

	//Update the configuration choice on the toolbar
	DoUpdateConfigChoiceControl();

	//update the 'Recent Workspace' history
	AddToRecentlyOpenedWorkspaces(path);

	//close all open files
	Frame::Get()->GetNotebook()->DeleteAllPages();
	Frame::Get()->GetNotebook()->Refresh();
}

void Manager::DoUpdateConfigChoiceControl()
{
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxComboBox *choice = Frame::Get()->GetConfigChoice();
	
	choice->Enable(true);
	choice->Freeze();
	
	std::list<WorkspaceConfigurationPtr> confs = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin();
	choice->Clear();
	for(; iter != confs.end(); iter++){
		choice->Append((*iter)->GetName());
	}

	if(choice->GetCount() > 0){
		int where = choice->FindString(matrix->GetSelectedConfigurationName());
		if(where != wxNOT_FOUND){
			choice->SetSelection(where);
		}else{
			choice->SetSelection(0);
		}
	}

	choice->Thaw();
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
	//in order to add files properly to the project
	//the application must set its working directory
	//to the project path
	DirSaver ds;

	wxString new_cwd = p->GetFileName().GetPath();
	::wxSetWorkingDirectory(new_cwd);

	wxBusyCursor cursor;
	if (TagsManagerST::Get()->GetParseComments()) {
		ttp = TagsManagerST::Get()->ParseSourceFiles(vList, &comments);
		TagsManagerST::Get()->StoreComments(comments);
	} else {
		ttp = TagsManagerST::Get()->ParseSourceFiles(vList);
	}

	TagsManagerST::Get()->Store(ttp);
	RebuildFileView();
}

void Manager::RebuildFileView()
{
	// update symbol tree
	WorkspacePane *wp = Frame::Get()->GetWorkspacePane();
	wp->BuildFileTree();
}

bool Manager::RemoveProject(const wxString &name)
{
	if( name.IsEmpty() ){
		return false;
	}

	ProjectPtr proj = GetProject(name);

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveProject(name, errMsg);
	CHECK_MSGBOX_BOOL(res);

	if(proj)
	{
		//remove symbols from the database
		std::vector<wxFileName> projectFiles;
		proj->GetFiles(projectFiles, true);
		TagsManagerST::Get()->DeleteProject(projectFiles);
		RemoveProjectNameFromOpenFiles(projectFiles);
	} // if(proj)
	
	Frame::Get()->GetWorkspacePane()->GetFileViewTree()->BuildTree();
	return true;
}

void Manager::RemoveProjectNameFromOpenFiles(const std::vector<wxFileName> &project_files)
{
	//go over all open tabs, if a file belongs to the closed
	//project, remove the project name from the container editor,
	//this will pervent the parser thread to parse this file after 
	//its parent project was removed
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	for(size_t i=0; i<(size_t)book->GetPageCount(); i++)
	{
		LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(i));
		if(editor)
		{
			wxString openFileFP = editor->GetFileName().GetFullPath();
			for(size_t j=0; j<project_files.size(); j++)
			{
				if(openFileFP == project_files.at(j).GetFullPath())
				{
					editor->SetProject(wxEmptyString);
					break;
				} // if(openFileFP == project_files.at(j).GetFullPath())
			} // for(size_t j=0; j<project_files.size(); j++)
		}
	}
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

	// Update symbol tree and database
	wxString vdPath = virtualDirFullPath.AfterFirst(wxT(':'));
	wxArrayString files;
	p->GetFilesByVirtualDir(vdPath, files);
	for(size_t i=0; i<files.Count(); i++){
		RemoveFileFromSymbolTree(files.Item(i), p->GetName());
	}

	//and finally, remove the virtual dir from the workspace
	bool res = WorkspaceST::Get()->RemoveVirtualDirectory(virtualDirFullPath, errMsg);
	CHECK_MSGBOX(res);
}

void Manager::SaveWorkspace()
{
	WorkspaceST::Get()->Save();
}

bool Manager::AddNewFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxFile file;
	if (!file.Create(fileName.GetData(), true))
		return false;

	if(file.IsOpened()){
		file.Close();
	}

	return AddFileToProject(fileName, vdFullPath, openIt);
}

bool Manager::AddFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt )
{
	wxString project;
	project = vdFullPath.BeforeFirst(wxT(':'));

	// Add the file to the project
	wxString errMsg;
	bool res = WorkspaceST::Get()->AddNewFile(vdFullPath, fileName, errMsg);
	if(!res){
		//file or virtual dir does not exist
		return false;
	}

	if( openIt ){
		OpenFile(fileName, project);
	}

	TagTreePtr ttp;
	if( project.IsEmpty() == false ){
		std::vector<DbRecordPtr> comments;
		if(TagsManagerST::Get()->GetParseComments()){
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName, &comments);
			TagsManagerST::Get()->StoreComments(comments);
		}else{
			ttp = TagsManagerST::Get()->ParseSourceFile(fileName);
		}
		TagsManagerST::Get()->Store(ttp);
	}
	return true;
}

void Manager::AddFilesToProject(const wxArrayString &files, const wxString &vdFullPath, wxArrayString &actualAdded)
{
	wxString project;
	project = vdFullPath.BeforeFirst(wxT(':'));

	// Add the file to the project
	wxString errMsg;
	bool res = true;
	size_t i=0;
	for(i=0; i<files.GetCount(); i++){
		res = WorkspaceST::Get()->AddNewFile(vdFullPath, files.Item(i), errMsg);
		if(res){
			actualAdded.Add(files.Item(i));
		}
	}

	//convert wxArrayString to vector for the ctags api
	std::vector<wxFileName> vFiles;
	for(i=0; i<actualAdded.GetCount(); i++){
		vFiles.push_back(actualAdded.Item(i));
	}

	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	TagTreePtr ttp;
	if( project.IsEmpty() == false ){
		//in order to add files properly to the project
		//the application must set its working directory
		//to the project path
		DirSaver ds;

		wxString new_cwd = proj->GetFileName().GetPath();
		::wxSetWorkingDirectory(new_cwd);

		wxBusyCursor cursor;
		std::vector<DbRecordPtr> comments;
		if(TagsManagerST::Get()->GetParseComments()){
			ttp = TagsManagerST::Get()->ParseSourceFiles(vFiles, &comments);
			TagsManagerST::Get()->StoreComments(comments);
		}else{
			ttp = TagsManagerST::Get()->ParseSourceFiles(vFiles);
		}
		TagsManagerST::Get()->Store(ttp);
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
	wxFileName absPath(fileName);
	absPath.MakeAbsolute(GetProjectCwd(project));

	int count = nb->GetPageCount();
	for(int i=0; i<count; i++){
		LEditor *editor = dynamic_cast<LEditor*>(nb->GetPage(static_cast<size_t>(i)));
		if( editor ){
			wxString fn = editor->GetFileName().GetFullPath();
			
			if(fn == absPath.GetFullPath() && editor->GetProject() == project){
				nb->DeletePage(static_cast<size_t>(i));
				break;
			}
		}
	}

	wxString errMsg;
	bool res = WorkspaceST::Get()->RemoveFile(vdFullPath, fileName, errMsg);
	CHECK_MSGBOX_BOOL(res);

	RemoveFileFromSymbolTree(absPath, project);
	return true;
}

void Manager::RemoveFileFromSymbolTree(const wxFileName &fileName, const wxString &project)
{
	if( project.IsEmpty() == false ){
		// Remove the file from the tags database as well
		TagsManagerST::Get()->Delete(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), fileName.GetFullPath());
	}
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
	if( info.IsOk() && !info.IsShown() ){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

	// set the selection to focus win
	OutputPane *pane = Frame::Get()->GetOutputPane();
	int index = pane->CaptionToIndex(focusWin);
	if( index != wxNOT_FOUND && index != pane->GetNotebook()->GetSelection()){
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}

/**
 * \brief 
 * \param focusWin
 * \return 
 */
void Manager::ShowWorkspacePane(wxString focusWin){
	// make the output pane visible
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
	if( info.IsOk() && !info.IsShown() ){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}

	// set the selection to focus win
	WorkspacePane *pane = Frame::Get()->GetWorkspacePane();
	int index = pane->CaptionToIndex(focusWin);
	if( index != wxNOT_FOUND && index != pane->GetNotebook()->GetSelection()){
		pane->GetNotebook()->SetSelection((size_t)index);
	}
}

void Manager::HideOutputPane()
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Output"));
	if( info.IsOk() && info.IsShown()){
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}
}

void Manager::HideWorkspacePane()
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Workspace"));
	if( info.IsOk() && info.IsShown()){
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}	
}

ContextBasePtr Manager::NewContextByFileName(const wxFileName &fileName, LEditor *parent) const
{
	EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
	for(; iter != EditorConfigST::Get()->LexerEnd(); iter++){
		LexerConfPtr lexer = iter->second;
		wxString lexExt = lexer->GetFileSpec();
		wxStringTokenizer tkz(lexExt, wxT(";"));
		while(tkz.HasMoreTokens()){
			if(wxMatchWild(tkz.NextToken(), fileName.GetFullName())){
				return ContextManager::Get()->NewContext(parent, lexer->GetName());
			}
		}
	}

	// return the default context
	return ContextManager::Get()->NewContext(parent, wxT("Text"));
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

bool Manager::CreateDefaultNewCompiler(const wxString &name)
{
	if(BuildSettingsConfigST::Get()->IsCompilerExist(name)){
		wxMessageBox(wxT("A compiler with this name already exist"), wxT("Error"), wxOK | wxICON_HAND);
		return false;
	}
	
	CompilerPtr cmp = BuildSettingsConfigST::Get()->GetCompiler(name);
	cmp->SetName(name);
	BuildSettingsConfigST::Get()->SetCompiler(cmp);
	return true;
}

void Manager::DeleteCompiler(const wxString &name)
{
	if(wxMessageBox(wxT("Remove Compiler?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION) == wxYES){
		BuildSettingsConfigST::Get()->DeleteCompiler(name);
	}
}

void Manager::GetProjectTemplateList(std::list<ProjectPtr> &list)
{
	wxString tmplateDir = m_startupDir + PATH_SEP + wxT("templates");
	
	//read all files under this directory
	DirTraverser dt(wxT("*.project"));

	wxDir dir(tmplateDir);
	dir.Traverse(dt);

	wxArrayString &files = dt.GetFiles();

	if(files.GetCount() > 0){
		for(size_t i=0; i<files.GetCount(); i++){
			ProjectPtr proj(new Project());
			proj->Load(files.Item(i));
			list.push_back(proj);
		}
	}else{
		//create 3 default empty projects
		ProjectPtr exeProj(new Project());
		ProjectPtr libProj(new Project());
		ProjectPtr dllProj(new Project());
		libProj->Create(wxT("Static Library"), tmplateDir, Project::STATIC_LIBRARY);
		dllProj->Create(wxT("Dynamic Library"), tmplateDir, Project::DYNAMIC_LIBRARY);
		exeProj->Create(wxT("Executable"), tmplateDir, Project::EXECUTABLE);
		list.push_back(libProj);
		list.push_back(dllProj);
		list.push_back(exeProj);
	}
}

void Manager::SaveProjectTemplate(ProjectPtr proj, const wxString &name)
{
	//create new project
	wxString tmplateDir = m_startupDir + PATH_SEP + wxT("templates");

	ProjectPtr cloned(new Project());
	cloned->Create(name, tmplateDir);

	//copy project settings
	cloned->SetSettings(proj->GetSettings());
}

ProjectPtr Manager::GetProject(const wxString &name) const
{
	wxString errMsg;
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(name, errMsg);
	if( !proj ){
		wxMessageBox(errMsg, wxT("Error"), wxOK | wxICON_HAND);
		return NULL;
	}
	return proj;
}


void Manager::PopupProjectDependsDlg(const wxString &projectName)
{
	DependenciesDlg *dlg = new DependenciesDlg(GetMainFrame(), projectName);
	dlg->ShowModal();
	dlg->Destroy();
}

void Manager::CleanProject(const wxString &projectName)
{
	if( m_cleanRequest && m_cleanRequest->IsBusy() ){
		return;
	}

	if( m_cleanRequest ){
		delete m_cleanRequest;
	}

	m_cleanRequest = new CleanRequest(GetMainFrame(), projectName);
	m_cleanRequest->Process();
}

void Manager::BuildProject(const wxString &projectName)
{
	if( m_compileRequest && m_compileRequest->IsBusy() ){
		return;
	}

	if( m_compileRequest ){
		delete m_compileRequest;
	}

	//save all files before compiling, but dont saved new documents
	SaveAll(false);

	m_compileRequest = new CompileRequest(GetMainFrame(), projectName);
	m_compileRequest->Process();
}

void Manager::StopBuild()
{
	if( m_compileRequest && !m_compileRequest->IsBusy() ){
		return;
	}

	if( m_compileRequest ){
		m_compileRequest->Stop();
	}
}

bool Manager::IsBuildInProgress() const
{
	return (m_cleanRequest && m_cleanRequest->IsBusy()) || (m_compileRequest && m_compileRequest->IsBusy());
}

bool Manager::IsProgramRunning() const
{
	if(m_asyncExeCmd == NULL)
		return false;

	return (m_asyncExeCmd && m_asyncExeCmd->IsBusy());
}

void Manager::DebugMessage(wxString msg)
{
	Frame::Get()->GetOutputPane()->AppendText(OutputPane::OUTPUT_DEBUG, msg);
}

void Manager::ExecuteNoDebug(const wxString &projectName)
{
	//an instance is already running
	if(m_asyncExeCmd && m_asyncExeCmd->IsBusy()){
		return;
	}

	BuildConfigPtr bldConf = WorkspaceST::Get()->GetProjSelBuildConf(projectName);
	wxString cmd = bldConf->GetCommand();
	wxString cmdArgs = bldConf->GetCommandArguments();
	//execute command & cmdArgs
	wxString execLine(cmd + wxT(" ") + cmdArgs);
	wxString wd = bldConf->GetWorkingDirectory();

	//change directory to the working directory
	DirSaver ds;

	ProjectPtr proj = GetProject(projectName);
	//print the current directory
	::wxSetWorkingDirectory(proj->GetFileName().GetPath());
	DebugMessage(wxT("Setting working directory to: ") + proj->GetFileName().GetPath() + wxT("\n"));

	//now set the working directory according to working directory field from the 
	//project settings
	::wxSetWorkingDirectory(wd);
	DebugMessage(wxT("Setting working directory to: ") + wd + wxT("\n"));
	
	//execute the command line
	//the async command is a one time executable object,
	m_asyncExeCmd = new AsyncExeCmd(GetMainFrame());
	m_asyncExeCmd->Execute(execLine);
	if(m_asyncExeCmd->GetProcess())
		m_asyncExeCmd->GetProcess()->Connect(wxEVT_END_PROCESS, wxProcessEventHandler(Manager::OnProcessEnd), NULL, this);
}

void Manager::OnProcessEnd(wxProcessEvent &event)
{
	m_asyncExeCmd->ProcessEnd(event);
	m_asyncExeCmd->GetProcess()->Disconnect(wxEVT_END_PROCESS, wxProcessEventHandler(Manager::OnProcessEnd), NULL, this);
	delete m_asyncExeCmd;
	m_asyncExeCmd = NULL;
}

void Manager::SetWorkspaceConfigurationName(const wxString &name)
{
	BuildMatrixPtr matrix = GetWorkspaceBuildMatrix();
	matrix->SetSelectedConfigurationName(name);
	SetWorkspaceBuildMatrix(matrix);
}

void Manager::ShowMainToolbar(bool show)
{
	wxAuiPaneInfo &info = Frame::Get()->GetDockingManager().GetPane(wxT("Standard Toolbar"));
	if(info.IsOk() && show && !info.IsShown()){
		info.Show();
		Frame::Get()->GetDockingManager().Update();
	}else if(info.IsOk() && !show && info.IsShown()){
		info.Hide();
		Frame::Get()->GetDockingManager().Update();
	}
}

BuildConfigPtr Manager::GetActiveProjectBuildConf()
{
	BuildMatrixPtr matrix = GetWorkspaceBuildMatrix();
	wxString projConf = matrix->GetProjectSelectedConf(matrix->GetSelectedConfigurationName(), GetActiveProjectName());
	ProjectSettingsPtr settings = GetProjectSettings(GetActiveProjectName());
	return settings->GetBuildConfiguration(projConf);
}

void Manager::SetExternalDatabase(const wxFileName &dbname)
{
	// build the external database
	TagsManagerST::Get()->OpenExternalDatabase(dbname);
	Frame::Get()->GetStatusBar()->SetStatusText(wxString::Format(wxT("External DB: '%s'"), dbname.GetFullPath().GetData()), 2);
	EditorConfigST::Get()->SetTagsDatabase(dbname.GetFullPath());
}

void Manager::GetWorkspaceFiles(std::vector<wxFileName> &files, bool absPath)
{
	wxArrayString projects;
	GetProjectList(projects);
	for(size_t i=0; i<projects.GetCount(); i++)
	{
		ProjectPtr p = GetProject(projects.Item(i));
		p->GetFiles(files, absPath);
	}
}

bool Manager::IsFileInWorkspace(const wxString &fileName)
{
	wxFileName findme(fileName);
	std::vector<wxFileName> files;

	GetWorkspaceFiles(files);
	std::vector<wxFileName>::const_iterator iter = std::find(files.begin(), files.end(), findme);
	return iter != files.end();
}

void Manager::RetagProject(const wxString &projectName)
{
	DoRetagProject(projectName);	
}
 
void Manager::DoRetagProject(const wxString &projectName)
{
	//now rebuild the project
	ProjectPtr proj = GetProject(projectName);
	if( proj ){
		//remove project from database
		std::vector<wxFileName> projectFiles;
		proj->GetFiles(projectFiles, true);
		TagsManagerST::Get()->DeleteProject(projectFiles);

		//set cursor to busy
		wxBusyCursor cursor;

		std::vector<wxFileName> files;
		std::vector<DbRecordPtr> comments;
		TagTreePtr ttp;

		//change the directory to the project dir
		DirSaver ds;
		::wxSetWorkingDirectory(proj->GetFileName().GetPath());

		proj->GetFiles(files, true);

		bool parseComments = TagsManagerST::Get()->GetParseComments();
		ttp = TagsManagerST::Get()->ParseSourceFiles(files, parseComments ? &comments : NULL);

		TagsManagerST::Get()->Store(ttp);
		if(parseComments){
			TagsManagerST::Get()->StoreComments(comments);
		}
	} // if( proj )
}

void Manager::RetagWorkspace()
{
	wxArrayString projects;
	GetProjectList(projects);

	for(size_t i=0; i<projects.GetCount(); i++){
		DoRetagProject(projects.Item(i));
	}
}

void Manager::WriteProgram(const wxString &line)
{
	if(!IsProgramRunning())
		return;

	wxOutputStream *out = m_asyncExeCmd->GetOutputStream();
	if( out ) 
	{
		wxString cmd(line);
		cmd += wxT("\n");

		const wxCharBuffer pWriteData = _C(cmd);
		out->Write(pWriteData.data(), cmd.Length());
	}
}

void Manager::KillProgram()
{
	if(!IsProgramRunning())
		return;

	m_asyncExeCmd->Terminate();
}

wxString Manager::GetProjectNameByFile(const wxString &fullPathFileName)
{
	wxArrayString projects;
	GetProjectList(projects);

	std::vector<wxFileName> files;
	for(size_t i=0; i<projects.GetCount(); i++){
		files.clear();
		ProjectPtr proj = GetProject(projects.Item(i));
		proj->GetFiles(files, true);

		for(size_t xx=0; xx<files.size(); xx++)
		{
			if(files.at(xx).GetFullPath() == fullPathFileName)
			{
				return proj->GetName();
			}
		}
	}

	return wxEmptyString;
}

void Manager::ImportFromMakefile(const wxString &path)
{
	DebugMessage(path + wxT("\n"));
	
	wxFileName fileName = path;
	DebugMessage(fileName.GetPath() + wxT("\n"));
	
	/*wxString file;
	FileUtils::ReadFileUTF8(fileName, file);
	
	StringTokenizer tokenizer(file, wxT("\n"));
	wxArrayString fileContents;
	for(int i = 0; i < tokenizer.Count(); i++)
	{
		fileContents.Add(tokenizer[i]);
	}*/
	
	VariableLexer expander(path);
	wxArrayString expanded = expander.getResult();
	
	MakefileParser parser(expanded);
	TypedStrings parsed = parser.getResult();
	
	TargetLexer lexer(parsed);
	Targets lexed = lexer.getResult();
	CreateWorkspace(wxT("import_from_") + fileName.GetName(), fileName.GetPath());
	
	wxArrayString extentions;
	extentions.Add(wxT(".h"));
	extentions.Add(wxT(".hpp"));
	extentions.Add(wxT(".c"));
	extentions.Add(wxT(".cc"));
	extentions.Add(wxT(".cpp"));
	extentions.Add(wxT(".cxx"));
	
	for(size_t i = 0; i < lexed.size(); i++)
	{
		Target targ = lexed[i];
		wxArrayString deps = targ.getDeps();
		
		ProjectPtr proj(new Project());
		proj->Create(targ.getName(), fileName.GetPath(), wxT("importedProject"));
		proj->SetSettings(new ProjectSettings(NULL));
		
		for(size_t j = 0; j < deps.size(); j++)
		{
			wxString dep = deps[j];
			if(dep.Right(2) == wxT(".o")) // string ends with .o!
			{
				wxString file = dep.Left(dep.size()-2);
				
				for(size_t k = 0; k < extentions.size(); k++)
				{
					wxString ext = extentions[k];					
					wxFileName fileName = proj->GetFileName().GetPathWithSep() + file + ext;
					if(fileName.FileExists()) 
					{
						bool added = proj->AddFile(fileName.GetFullPath(), wxT("Source Files"));
						if(!added)
						{
							DebugMessage(wxT("WHOOPS WRONG BAD NOT GOOD!\n"));
						}
					}
				}
			}
			else if(deps.Index(dep, false) != wxNOT_FOUND)
			{
				wxArrayString dependencies= proj->GetDependencies();
				dependencies.Add(dep);
				proj->SetDependencies(dependencies);
			}
		}	
		proj->Save();
		//AddProject(proj->GetFileName().GetFullPath());
	}
	
	return;
}

LEditor *Manager::GetActiveEditor() const
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	if(!book)
	{
		return NULL;
	}

	int selected = book->GetSelection();
	if(selected == wxNOT_FOUND)
		return NULL;

	LEditor *editor = dynamic_cast<LEditor*>(book->GetPage(selected));
	return editor;
}

void Manager::AddToRecentlyOpenedFiles(const wxString &fileName)
{
	//get list recently opened files
	wxArrayString files;
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedFies(files);
	
	if(files.Index(fileName) == wxNOT_FOUND){
		//the file does not exist, add it and save the list
		files.Add(fileName);
		cfg->SetRecentlyOpenedFies(files);
		m_recentFiles.AddFileToHistory(fileName);
	} // if(files.Index(fileName) == wxNOT_FOUND)
}

void Manager::AddToRecentlyOpenedWorkspaces(const wxString &fileName)
{
	//get list recently opened files
	wxArrayString files;
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedWorkspaces(files);
	
	if(files.Index(fileName) == wxNOT_FOUND){
		//the file does not exist, add it and save the list
		files.Add(fileName);
		cfg->SetRecentlyOpenedWorkspaces(files);
		m_recentWorkspaces.AddFileToHistory(fileName);
	} // if(files.Index(fileName) == wxNOT_FOUND)
}

void Manager::GetRecentlyOpenedFiles(wxArrayString &files)
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedFies(files);
}

void Manager::GetRecentlyOpenedWorkspaces(wxArrayString &files)
{
	EditorConfig *cfg = EditorConfigST::Get();
	cfg->GetRecentlyOpenedWorkspaces(files);
}

void Manager::CloseExternalDatabase()
{
	TagsManager *mgr = TagsManagerST::Get();
	mgr->CloseExternalDatabase();
	//remove the entry from the status bar
	Frame::Get()->GetStatusBar()->SetStatusText(wxEmptyString, 2);

}
