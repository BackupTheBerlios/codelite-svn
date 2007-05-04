#include "precompiled_header.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

#include "frame.h"
#include <wx/xrc/xmlres.h>
#include "symbol_tree.h"
#include <wx/splitter.h>
#include "parse_thread.h"
#include "cpp_symbol_tree.h"
#include "language.h"
#include "editor_config.h"
#include "manager.h"
#include "menumanager.h"
#include <wx/aboutdlg.h>
#include "findinfilesdlg.h" 
#include "search_thread.h"
#include "project.h"
#include "newdlg.h"
#include "fileview.h"
#include "wx/aui/framemanager.h"

#define ID_CTAGS_GLOBAL_ID		10500
#define ID_CTAGS_LOCAL_ID		10501

//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(Frame, wxFrame)
	// Search Thread handler
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, Frame::OnSearchThread)

	// New dialog handlers
	EVT_COMMAND(wxID_ANY, wxEVT_NEW_DLG_CREATE, Frame::OnNewDlgCreate)

	// Handler menu events
	EVT_MENU(wxID_EXIT, Frame::OnQuit)
	EVT_MENU(wxID_SAVE, Frame::OnSave)
	EVT_MENU(wxID_SAVEAS, Frame::OnSaveAs)
	EVT_MENU(XRCID("about"), Frame::OnAbout)
	EVT_MENU(wxID_NEW, Frame::OnFileNew)
	EVT_MENU(wxID_OPEN, Frame::OnFileOpen)
	
	EVT_MENU(wxID_CLOSE, Frame::OnFileClose)
	EVT_MENU(XRCID("save_all"), Frame::OnFileSaveAll)
	EVT_MENU(wxID_CUT, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_COPY, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_PASTE, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_UNDO, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_REDO, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_SELECTALL, Frame::DispatchCommandEvent)
	EVT_MENU(wxID_DUPLICATE, Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("select_to_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("match_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_next"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous"), Frame::DispatchCommandEvent)
	EVT_MENU(wxID_FIND, Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_linenumber"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("previous_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("removeall_bookmarks"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_definition"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_previous_definition"), Frame::DispatchCommandEvent)

	EVT_MENU(XRCID("find_in_files"), Frame::OnFindInFiles)
	EVT_MENU(XRCID("new_workspace"), Frame::OnProjectNewWorkspace)
	EVT_MENU(XRCID("new_project"), Frame::OnProjectNewProject)
	EVT_MENU(XRCID("switch_to_workspace"), Frame::OnSwitchWorkspace)
	EVT_MENU(XRCID("add_project"), Frame::OnProjectAddProject)

	EVT_UPDATE_UI(wxID_SAVE, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_SAVEAS, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_CLOSE, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("save_all"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_CUT, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_COPY, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_PASTE, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_UNDO, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_REDO, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_SELECTALL, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(wxID_DUPLICATE, Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("select_to_brace"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("match_brace"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("complete_word"), Frame::OnCompleteWordUpdateUI)
	EVT_UPDATE_UI(XRCID("find_next"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("find_previous"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("goto_linenumber"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_FIND, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("toggle_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("next_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("previous_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("removeall_bookmarks"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("new_project"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("add_project"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("goto_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("goto_previous_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("view_as_menu"), Frame::OnFileExistUpdateUI)
	EVT_MENU(XRCID("complete_word"), Frame::OnCompleteWord)
	EVT_MENU(XRCID("tags_options"), Frame::OnCtagsOptions)
	EVT_MENU(XRCID("workspace_pane"), Frame::OnViewWorkspacePane)
	EVT_MENU(XRCID("output_pane"), Frame::OnViewOutputPane)
	EVT_UPDATE_UI(XRCID("output_pane"), Frame::OnViewOutputPaneUI)
	EVT_UPDATE_UI(XRCID("workspace_pane"), Frame::OnViewWorkspacePaneUI)
	EVT_MENU(XRCID("view_as_cpp"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("view_as_text"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("view_as_java"), Frame::DispatchCommandEvent)

	/*
	EVT_MENU(ID_BUILD_EXTERNAL_DB, Frame::OnBuildExternalDatabase)
	EVT_MENU(ID_USE_EXTERNAL_DB, Frame::OnUseExternalDatabase)
	EVT_MENU(ID_PARSE_COMMENTS, Frame::OnParseComments)
	*/

	EVT_CLOSE(Frame::OnClose)
END_EVENT_TABLE()
Frame* Frame::m_theFrame = NULL;

Frame::Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxFrame(pParent, id, title, pos, size, style, name)
, m_restartCtags(true)
, m_findInFilesDlg(NULL)
{
	CreateGUIControls();

	ManagerST::Get();	// Dummy call

	// Start the search thread
	SearchThreadST::Get()->SetNotifyWindow(this);
	SearchThreadST::Get()->Start();
}

Frame::~Frame(void)
{
	ManagerST::Free();

	// uninitialize AUI manager
	m_mgr.UnInit();
}

Frame* Frame::Get()
{
    if( !m_theFrame )
		m_theFrame = new Frame(NULL, wxID_ANY, _("Lite Editor"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE | wxNO_FULL_REPAINT_ON_RESIZE);
	return m_theFrame;
}

void Frame::CreateGUIControls(void)
{
#ifdef __WXMSWIN__
    SetIcon(wxICON(mainicon));
#endif


	// tell wxAuiManager to manage this frame
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(m_mgr.GetFlags() | wxAUI_MGR_ALLOW_ACTIVE_PANE);
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

	// Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));
	
	//---------------------------------------------
	// Add the output pane
	//---------------------------------------------
	m_outputPane = new OutputPane(this, wxT("Output"));
	
	m_mgr.AddPane(m_outputPane, wxAuiPaneInfo().Name(wxT("Output")).
		Caption(wxT("Output")).Bottom().Layer(1).Position(1).MaximizeButton(true).CloseButton(true));

	// Add the explorer pane
	m_workspacePane = new WorkspacePane(this, wxT("Workspace"));
	m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().
		Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).
		Left().Layer(1).Position(1).
		CloseButton(true).MaximizeButton(true));

	// Create the notebook for all the files
	long style = 
		wxFNB_TABS_BORDER_SIMPLE | 
		wxFNB_NODRAG | 
		wxFNB_FF2 | 
		wxFNB_BACKGROUND_GRADIENT | 
		wxFNB_NO_X_BUTTON |
		wxFNB_NO_NAV_BUTTONS |
		wxFNB_DROPDOWN_TABS_LIST |
		wxFNB_SMART_TABS |
		wxFNB_X_ON_TAB;

	m_notebook = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_mgr.AddPane(m_notebook, wxAuiPaneInfo().Name(wxT("Editor")).
                  CenterPane().PaneBorder(false));

	// Connect the main notebook events
	m_notebook->Connect(m_notebook->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED, wxFlatNotebookEventHandler(Frame::OnPageChanged), NULL, this);
	m_notebook->Connect(m_notebook->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, wxFlatNotebookEventHandler(Frame::OnFileClosing), NULL, this);

	// Initialise editor configuration file
	wxFileName configFile(wxT("liteeditor.xml"));
	EditorConfigST::Get()->Load(configFile);

	//--------------------------------------------------------------------------------------
	// Start ctags process. 
	// ctags process must be started somewhere in the main frames' (here is a good place ^^)
	// constructor since to execute it, we are using wxExecute() which,  
	// according to the documentation, will fail if used outside of the main thread. 
	//--------------------------------------------------------------------------------------
	
	// We keep a pointer to wxProcess object returend from ctags starting process
	TagsManagerST::Get()->StartCtagsProcess(TagsGlobal);
	TagsManagerST::Get()->StartCtagsProcess(TagsLocal);

	//--------------------------------------------------------------------------------------
	// Start the parsing thread, the parsing thread and the SymbolTree (or its derived)
	// Are connected. The constructor of SymbolTree, calls ParseThreadST::Get()->SetNotifyWindow(this)
	// to allows it to receive events for gui changes.
	//
	// If you wish to connect another object for it, simply call ParseThreadST::Get()->SetNotifyWindow(this)
	// with another object as 'this'
	//--------------------------------------------------------------------------------------
	ParseThreadST::Get()->Start();

	// And finally create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(3);
	SetStatusBar(statusBar);
	
	GetStatusBar()->SetStatusText(_("Ready"));

	// "commit" all changes made to wxAuiManager
	wxString pers = EditorConfigST::Get()->LoadPerspective(wxT("Default"));

	// if we have a perspective, use it, else use the default persprective
	if( pers.IsEmpty() == false ){
		m_mgr.LoadPerspective(pers);
	} else {
		m_mgr.Update();
	}

	SetAutoLayout (true);
	Layout();
}

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void Frame::DispatchCommandEvent(wxCommandEvent &event)
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
	if( !editor ){
		return;	
	}

	editor->OnMenuCommand(event);
}

void Frame::DispatchUpdateUIEvent(wxUpdateUIEvent &event)
{
	if( m_notebook->GetPageCount() == 0 ){
		event.Enable(false);
		return;	
	}

	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
	if( !editor ){ 
		event.Enable(false);
		return;	
	}

	editor->OnUpdateUI(event);
}

void Frame::OnFileExistUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
	if( !editor ){ 
		event.Enable(false);
	} else {
		event.Enable(true);
	}
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo info;
    info.SetName(wxT("Lite Editor"));
    info.SetVersion(wxT("0.1 Alpha"));
    info.SetDescription(wxT("A lightweight cross-platform editor for C/C++ "));
    info.SetCopyright(wxT("(C) 2007 By Eran Ifrah <eran.ifrah@gmail.com>"));
    wxAboutBox(info);
}

void Frame::OnClose(wxCloseEvent& event)
{
	// Stop the search thread
	SearchThreadST::Get()->StopSearch();
	EditorConfigST::Get()->SavePerspective(wxT("Default"), m_mgr.SavePerspective());
	event.Skip();
}

wxString Frame::GetStringFromUser(const wxString& msg)
{
	wxString userString;
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, msg, _("Enter Text"));
	if(dlg->ShowModal() == wxID_OK)
	{
		userString = dlg->GetValue();
	}
	dlg->Destroy();
	return userString;
}

void Frame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	// SaveFile contains the logic of "Untiltled" files
	editor->SaveFile();
}

void Frame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	editor->SaveFileAs();
}

void Frame::OnSwitchWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(wxT("Lite Editor Workspace files (*.workspace)|*.workspace|")
					   wxT("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Workspace"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK){
		ManagerST::Get()->OpenWorkspace(dlg->GetPath());
	}
	dlg->Destroy();
}

//------------------------------------------------------
// Complete word, complete a word from the current caret
// position.
// The list of words, is constructed from the database &
// from the local scope
//------------------------------------------------------
void Frame::OnCompleteWord(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	editor->CompleteWord();	
}

void Frame::OnBuildExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	wxDirDialog *dlg = new wxDirDialog(this, _("Select the root directory of the include path:"), wxGetCwd());
	if(dlg->ShowModal() == wxID_OK)
	{
		// get output file name from user
		wxString db = GetStringFromUser(_("Insert full path name for the database (example: /home/eran/cpp.db):"));

		// Get the dirname
		wxString path = dlg->GetPath();
		wxFileName fn(path);
		wxFileName dbname(db);

		// set cursor to busy
		wxBusyCursor busy;

		// build the external database
		TagsManagerST::Get()->BuildExternalDatabase(fn, dbname, wxEmptyString, this);
	}
	dlg->Destroy();
}


void Frame::OnUseExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	const wxString ALL(	wxT("Tags Database File (*.db)|*.db|")
						wxT("All Files (*.*)|*.*"));
	
	wxFileDialog *dlg = new wxFileDialog(this, _("Select an external database:"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK)
	{
		// get the path
		wxFileName dbname(dlg->GetPath());

		// build the external database
		TagsManagerST::Get()->OpenExternalDatabase(dbname);

		GetStatusBar()->SetStatusText(wxString::Format(_("External DB: '%s'"), dbname.GetFullPath().GetData()), 2);

	}
	dlg->Destroy();
}

void Frame::OnParseComments(wxCommandEvent &event)
{
	TagsManagerST::Get()->ParseComments( event.IsChecked() );
}

// Open new file
void Frame::OnFileNew(wxCommandEvent &event)
{
	static int fileCounter = 0;

	wxUnusedVar(event);
	wxString fileNameStr(wxT("Untitled"));
	fileNameStr << ++fileCounter;

	wxFileName fileName(fileNameStr);

	// Create new editor and add it to the notebook
	m_notebook->Freeze();
	LEditor *editor = new LEditor(m_notebook, wxID_ANY, wxSize(1, 1), fileName.GetFullPath(), wxEmptyString);
	m_notebook->AddPage(editor, fileName.GetFullName(), true);
	m_notebook->Thaw();

	editor->SetFocus ();
	editor->SetSCIFocus(true);
}

void Frame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
{
	const wxString ALL(	wxT("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, _("Open File"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK){
		ManagerST::Get()->OpenFile(dlg->GetPath(), wxEmptyString);
	}
	dlg->Destroy();	
}

void Frame::OnFileClose(wxCommandEvent &event)
{
	wxUnusedVar( event );
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;
	
	bool veto;
	ClosePage(editor, m_notebook->GetSelection(), true, veto);
}

void Frame::OnFileClosing(wxFlatNotebookEvent &event)
{
	// get the page that is now closing
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(event.GetSelection()));
	if( !editor )
		return;	

	bool veto;
	ClosePage(editor, event.GetSelection(), false, veto);
	if( veto ) event.Veto();
	event.Skip();
}

void Frame::OnPageChanged(wxFlatNotebookEvent &event)
{
	// pass the event to the editor
	LEditor *editor = dynamic_cast<LEditor*>( m_notebook->GetPage(event.GetSelection()) );
	if( !editor ){
		return;
	}
	editor->SetActive();
	event.Skip();
}

void Frame::OnFileSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->SaveAll();
}

void Frame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));

	// This menu item is enabled only if the current editor
	// belongs to a project 
	event.Enable(editor && !editor->GetProjectName().IsEmpty());
}

void Frame::ClosePage(LEditor *editor, int index, bool doDelete, bool &veto)
{
	veto = false;
	if( editor->GetModify() ) 
	{
		// prompt user to save file
		wxString msg;
		msg << wxT("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
		int answer = wxMessageBox(msg, wxT("Confirm"),wxYES_NO | wxCANCEL);
		switch( answer ) 
		{
		case wxYES:
			{
				// try to save the file, if an error occured, return without
				// closing the tab
				if( !editor->SaveFile() ) {
					return;
				} else {
					if( doDelete ) m_notebook->DeletePage(index, false);
				}
			}
			break;
		case wxCANCEL:
			veto = true;
			return; // do nothing
		case wxNO:
			// just delete the tab without saving the changes
			if( doDelete ) m_notebook->DeletePage(index, false);
			break;
		}
	} 
	else 
	{
		// file is not modified, just remove the tab
		if( doDelete ) m_notebook->DeletePage(index, false);
	}
}

void Frame::OnSearchThread(wxCommandEvent &event)
{
	// make sure that the output pane is visible and selection
	// is set to the 'Find In Files' tab
	ManagerST::Get()->ShowOutputPane(OutputPane::FIND_IN_FILES_WIN);

	m_outputPane->CanFocus(false);
	if( event.GetEventType() == wxEVT_SEARCH_THREAD_MATCHFOUND)
	{
		SearchResultList *res = (SearchResultList*)event.GetClientData();
		SearchResultList::iterator iter = res->begin();

		wxString msg;
		for(; iter != res->end(); iter++){
			msg.Append((*iter).GetMessage() + wxT("\n"));
		}
		m_outputPane->AppendText(OutputPane::FIND_IN_FILES_WIN, msg);
		delete res;
	}
	else if(event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHCANCELED)
	{
		m_outputPane->AppendText(OutputPane::FIND_IN_FILES_WIN, event.GetString() + wxT("\n"));
	}
	else if(event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHSTARTED)
	{
		m_outputPane->Clear();
		m_outputPane->AppendText(OutputPane::FIND_IN_FILES_WIN, event.GetString() + wxT("\n"));
	}
	else if(event.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHEND)
	{
		SearchSummary *summary = (SearchSummary*)event.GetClientData();
		m_outputPane->AppendText(OutputPane::FIND_IN_FILES_WIN, summary->GetMessage() + wxT("\n"));
		delete summary;
	}
	m_outputPane->CanFocus(true);
}

void Frame::OnFindInFiles(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if( m_findInFilesDlg == NULL ){
		m_findInFilesDlg = new FindInFilesDialog(this, m_data);
		m_findInFilesDlg->SetEventOwner(GetEventHandler());
	}
	
	if( m_findInFilesDlg->IsShown() )
	{
		// make sure that dialog has focus and that this instace
		m_findInFilesDlg->SetFocus();
		return;
	}

	m_findInFilesDlg->Show();
}

void Frame::OnWorkspaceOpen(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->IsWorkspaceOpen());
}

// Project->New Workspace
void Frame::OnProjectNewWorkspace(wxCommandEvent &event)
{
	wxUnusedVar(event);
	NewDlg *dlg = new NewDlg(this, NEW_DLG_WORKSPACE);
	dlg->ShowModal();
	dlg->Destroy();
}

// Project->New Project
void Frame::OnProjectNewProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	NewDlg *dlg = new NewDlg(this, NEW_DLG_PROJECT);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnProjectAddProject(wxCommandEvent &event)
{
	wxUnusedVar(event);

	// Prompt user for project path
	const wxString ALL(	wxT("Lite Editor Projects (*.project)|*.project|")
						wxT("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Project"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK)
	{
		// Open it
		ManagerST::Get()->AddProject(dlg->GetPath());  
	}
	dlg->Destroy();	
}

// NewDlg->Create handler
void Frame::OnNewDlgCreate(wxCommandEvent &event)
{
	wxUnusedVar(event);            
    
	NewDlg *dlg = dynamic_cast<NewDlg*>(event.GetEventObject());
	if( dlg ){
		if( dlg->GetSelection() == NEW_DLG_WORKSPACE ){
			WorkspaceData data = dlg->GetWorksapceData();
			CtagsOptions ctagsOptions = dlg->GetCtagsOptions();

			ManagerST::Get()->CreateWorkspace(data.m_name, data.m_path, ctagsOptions);

		} else if( dlg->GetSelection() == NEW_DLG_PROJECT ) {
			ProjectData data = dlg->GetProjectData();
			ManagerST::Get()->CreateProject(data.m_name, data.m_path, data.m_type);
		}
	}
}

void Frame::OnCtagsOptions(wxCommandEvent &event)
{
	wxUnusedVar(event);
	CtagsOptionsDlg *dlg = new CtagsOptionsDlg(this);

	if(dlg->ShowModal() == wxID_OK){
		// update the ctags options
		TagsManagerST::Get()->SetCtagsOptions(dlg->GetCtagsOptions());
		ManagerST::Get()->SetWorkspaceCtagsOptions(dlg->GetCtagsOptions());
	}
	dlg->Destroy();
}

void Frame::OnViewOutputPane(wxCommandEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Output"));
	if(info.IsOk()){
		if( event.IsChecked() ){
			info.Show();
		} else {
			info.Hide();
		}
		m_mgr.Update();
	}
}

void Frame::OnViewWorkspacePane(wxCommandEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Workspace"));
	if(info.IsOk()){
		if( event.IsChecked() ){
			info.Show();
		} else {
			info.Hide();
		}
		m_mgr.Update();
	}
}

void Frame::OnViewWorkspacePaneUI(wxUpdateUIEvent &event){
	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Workspace"));
	if(info.IsOk()){
		event.Check(info.IsShown());
	}
}

void Frame::OnViewOutputPaneUI(wxUpdateUIEvent &event){
	wxAuiPaneInfo &info = m_mgr.GetPane(wxT("Output"));
	if(info.IsOk()){
		event.Check(info.IsShown());
	}
}

