#include "precompiled_header.h"

#include "frame.h"
#include <wx/xrc/xmlres.h>
#include "symbol_tree.h"
#include <wx/splitter.h>
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
#include "options_base_dlg.h"
#include "configuration_manager_dlg.h"
#include "filedroptarget.h"
#include "advanced_settings.h"
#include "build_settings_config.h"
#include "list"
#include "macros.h"
#include "editor_creator.h"
#include "async_executable_cmd.h"
#include "close_all_dlg.h"
#include "open_resouce_dlg.h"
#include "workspace_pane.h"

//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------
BEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_SYMBOLTREE_ADD_ITEM(wxID_ANY, Frame::OnAddSymbols)
	EVT_SYMBOLTREE_DELETE_ITEM(wxID_ANY, Frame::OnDeleteSymbols)
	EVT_SYMBOLTREE_UPDATE_ITEM(wxID_ANY, Frame::OnUpdateSymbols)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_MATCHFOUND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHCANCELED, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHEND, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_SEARCH_THREAD_SEARCHSTARTED, Frame::OnSearchThread)
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_ADDLINE, Frame::OnBuildEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_STARTED, Frame::OnBuildEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_BUILD_ENDED, Frame::OnBuildEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ADDLINE, Frame::OnOutputWindowEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_STARTED, Frame::OnOutputWindowEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_ASYNC_PROC_ENDED, Frame::OnOutputWindowEvent)
	EVT_COMMAND(wxID_ANY, wxEVT_NEW_DLG_CREATE_LE, Frame::OnNewDlgCreate)
	EVT_MENU(wxID_CLOSE_ALL, Frame::OnFileCloseAll)
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
	EVT_MENU(wxID_REFRESH, Frame::OnFileReload)
	EVT_MENU(XRCID("select_to_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("match_brace"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_next"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("find_previous"), Frame::DispatchCommandEvent)
	EVT_MENU(wxID_FIND, Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("goto_linenumber"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("toggle_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("next_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("previous_bookmark"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("swap_files"), Frame::DispatchCommandEvent)
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
	EVT_UPDATE_UI(wxID_CLOSE_ALL, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(wxID_REFRESH, Frame::OnFileExistUpdateUI)
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
	EVT_UPDATE_UI(viewAsSubMenuID, Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("previous_bookmark"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("removeall_bookmarks"), Frame::OnFileExistUpdateUI)
	EVT_UPDATE_UI(XRCID("new_project"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("add_project"), Frame::OnWorkspaceOpen)
	EVT_UPDATE_UI(XRCID("goto_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("goto_previous_definition"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("swap_files"), Frame::DispatchUpdateUIEvent)
	EVT_UPDATE_UI(XRCID("view_as_menu"), Frame::OnFileExistUpdateUI)
	EVT_MENU(XRCID("complete_word"), Frame::OnCompleteWord)
	EVT_MENU(XRCID("tags_options"), Frame::OnCtagsOptions)
	EVT_MENU(XRCID("workspace_pane"), Frame::OnViewWorkspacePane)
	EVT_MENU(XRCID("show_toolbar"), Frame::OnViewToolbar)
	EVT_UPDATE_UI(XRCID("show_toolbar"), Frame::OnViewToolbarUI)
	EVT_MENU(XRCID("output_pane"), Frame::OnViewOutputPane)
	EVT_UPDATE_UI(XRCID("output_pane"), Frame::OnViewOutputPaneUI)
	EVT_UPDATE_UI(XRCID("workspace_pane"), Frame::OnViewWorkspacePaneUI)
	
	EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchCommandEvent)
	EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchUpdateUIEvent)

	EVT_MENU(XRCID("options"), Frame::OnViewOptions)
	EVT_UPDATE_UI(XRCID("word_wrap"), Frame::DispatchUpdateUIEvent)
	EVT_MENU(XRCID("word_wrap"), Frame::DispatchCommandEvent)
	EVT_MENU(XRCID("configuration_manager"), Frame::OnConfigurationManager)
	EVT_UPDATE_UI(XRCID("configuration_manager"), Frame::OnWorkspaceOpen)
	EVT_MENU(XRCID("toggle_panes"), Frame::OnTogglePanes)
	EVT_UPDATE_UI(XRCID("add_envvar"), Frame::OnWorkspaceOpen)
	EVT_MENU(XRCID("add_envvar"), Frame::OnAddEnvironmentVariable)
	EVT_MENU(XRCID("advance_settings"), Frame::OnAdvanceSettings)

	EVT_MENU(XRCID("build_active_project"), Frame::OnBuildProject)
	EVT_MENU(XRCID("clean_active_project"), Frame::OnCleanProject)
	EVT_MENU(XRCID("stop_active_project_build"), Frame::OnStopBuild)
	EVT_UPDATE_UI(XRCID("stop_active_project_build"), Frame::OnStopBuildUI)
	EVT_UPDATE_UI(XRCID("clean_active_project"), Frame::OnCleanProjectUI)
	EVT_UPDATE_UI(XRCID("build_active_project"), Frame::OnBuildProjectUI)
	EVT_MENU(XRCID("execute_no_debug"), Frame::OnExecuteNoDebug)
	EVT_UPDATE_UI(XRCID("execute_no_debug"), Frame::OnExecuteNoDebugUI)

	EVT_MENU(XRCID("create_ext_database"), Frame::OnBuildExternalDatabase)
	EVT_MENU(XRCID("open_ext_database"), Frame::OnUseExternalDatabase)
	EVT_MENU(XRCID("find_resource"), Frame::OnFindResource)
	EVT_UPDATE_UI(XRCID("find_resource"), Frame::OnWorkspaceOpen)

	EVT_MENU(XRCID("add_project"), Frame::OnProjectAddProject)
	EVT_MENU(XRCID("import_from_makefile"), Frame::OnImportMakefile)
	EVT_UPDATE_UI(XRCID("import_from_makefile"), Frame::OnImportMakefileUI)
	EVT_CLOSE(Frame::OnClose)
	EVT_TIMER(wxID_ANY, Frame::OnTimer)

END_EVENT_TABLE()
Frame* Frame::m_theFrame = NULL;

Frame::Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxFrame(pParent, id, title, pos, size, style, name)
, m_restartCtags(true)
, m_findInFilesDlg(NULL)

{
	CreateGUIControls();
	ManagerST::Get();	// Dummy call

	//allow the main frame to receive files by drag and drop
	SetDropTarget( new FileDropTarget() );

	// Start the search thread
	SearchThreadST::Get()->SetNotifyWindow(this);
	SearchThreadST::Get()->Start();
	
	//start the editor creator thread
	EditorCreatorST::Get()->SetParent(m_notebook);
	m_timer = new wxTimer(this);
	m_timer->Start(100);
}

Frame::~Frame(void)
{
	m_timer->Stop();
	delete m_timer;
	ManagerST::Free();

	// uninitialize AUI manager
	m_mgr.UnInit();
}

Frame* Frame::Get()
{
    if( !m_theFrame )
		m_theFrame = new Frame(NULL, wxID_ANY, wxT("Lite Editor"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxMAXIMIZE | wxNO_FULL_REPAINT_ON_RESIZE);
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
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 2);

	// Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));

	//---------------------------------------------
	// Add the output pane
	//---------------------------------------------
	m_outputPane = new OutputPane(this, wxT("Output"));
	
	m_mgr.AddPane(m_outputPane, wxAuiPaneInfo().Name(wxT("Output")).
		Caption(wxT("Output")).Bottom().Layer(1).Position(1).CloseButton(true));

	// Add the explorer pane
	m_workspacePane = new WorkspacePane(this, wxT("Workspace"));
	m_mgr.AddPane(m_workspacePane, wxAuiPaneInfo().
		Name(m_workspacePane->GetCaption()).Caption(m_workspacePane->GetCaption()).
		Left().Layer(1).Position(1).
		CloseButton(true));

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
		wxFNB_X_ON_TAB |
		wxFNB_CUSTOM_DLG;

	m_notebook = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_notebook->SetCustomizeOptions(wxFNB_CUSTOM_LOCAL_DRAG | wxFNB_CUSTOM_ORIENTATION | wxFNB_CUSTOM_TAB_LOOK | wxFNB_CUSTOM_CLOSE_BUTTON );
	
	m_mgr.AddPane(m_notebook, wxAuiPaneInfo().Name(wxT("Editor")).
                  CenterPane().PaneBorder(false));

	// Connect the main notebook events
	m_notebook->Connect(m_notebook->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CHANGED, wxFlatNotebookEventHandler(Frame::OnPageChanged), NULL, this);
	m_notebook->Connect(m_notebook->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSING, wxFlatNotebookEventHandler(Frame::OnFileClosing), NULL, this);
	m_notebook->Connect(m_notebook->GetId(), wxEVT_COMMAND_FLATNOTEBOOK_PAGE_CLOSED, wxFlatNotebookEventHandler(Frame::OnPageClosed), NULL, this);

	// Initialise editor configuration files
	EditorConfigST::Get()->Load();
	CreateViewAsSubMenu();
	BuildSettingsConfigST::Get()->Load();

	//start ctags process
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
	
	// Connect this tree to the parse thread
	ParseThreadST::Get()->SetNotifyWindow( this );

	// And finally create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(3);
	SetStatusBar(statusBar);
	
	GetStatusBar()->SetStatusText(wxT("Ready"));

	//load windows perspective
	wxString pers = EditorConfigST::Get()->LoadPerspective(wxT("Default"));
	CreateToolbars();

	// load notebooks style
	long book_style = 0;
	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("Editor"));
	if(book_style != wxNOT_FOUND){
		m_notebook->SetWindowStyleFlag(book_style);
	}

	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("OutputPane"));
	if(book_style != wxNOT_FOUND){
		m_outputPane->GetNotebook()->SetWindowStyleFlag(book_style);
	}

	book_style = EditorConfigST::Get()->LoadNotebookStyle(wxT("WorkspacePane"));
	if(book_style != wxNOT_FOUND){
		m_workspacePane->GetNotebook()->SetWindowStyleFlag(book_style);
	}
	
	// if we have a perspective, use it, else use the default persprective
	if( pers.IsEmpty() == false ){
		m_mgr.LoadPerspective(pers);
	} 

	//load the tab right click menu
	m_tabRightClickMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));
	m_notebook->SetRightClickMenu(m_tabRightClickMenu);

	m_mgr.Update();
	SetAutoLayout (true);
	Layout();
}

void Frame::CreateViewAsSubMenu()
{
	//get the 'View As' menu

	int idx = GetMenuBar()->FindMenu(wxT("View"));
	if(idx != wxNOT_FOUND){
		wxMenu *menu = GetMenuBar()->GetMenu(idx);
		wxMenu *submenu = new wxMenu();

		//create a view as sub menu and attach it
		wxMenuItem *item(NULL);
		
		int minId = viewAsMenuItemID;

		//load all lexers
		// load generic lexers
		EditorConfig::ConstIterator iter = EditorConfigST::Get()->LexerBegin();
		for(; iter != EditorConfigST::Get()->LexerEnd(); iter++){
			LexerConfPtr lex = iter->second;
			item = new wxMenuItem(submenu, minId, lex->GetName(), wxEmptyString, wxITEM_CHECK);
			m_viewAsMap[minId] = lex->GetName();
			minId++;
			submenu->Append(item);
		}
		menu->Append(viewAsSubMenuID, wxT("View As"), submenu);
	}
}

wxString Frame::GetViewAsLanguageById(int id) const
{
	if(m_viewAsMap.find(id) == m_viewAsMap.end()){
		return wxEmptyString;
	}
	return m_viewAsMap.find(id)->second;
}

void Frame::CreateToolbars()
{
	//create the standard toolbar
	wxToolBar *tb = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	tb->AddTool(wxID_NEW, wxT("New"), wxXmlResource::Get()->LoadBitmap(wxT("page_new")), wxT("New File (Ctrl+N)"));
	tb->AddTool(wxID_OPEN, wxT("Open"), wxXmlResource::Get()->LoadBitmap(wxT("page_open")), wxT("Open File (Ctrl+O)"));
	tb->AddTool(wxID_REFRESH, wxT("Reload"), wxXmlResource::Get()->LoadBitmap(wxT("refresh")), wxT("Reload File (Ctrl+R)"));
	tb->AddSeparator();
	tb->AddTool(wxID_SAVE, wxT("Save"), wxXmlResource::Get()->LoadBitmap(wxT("page_save")), wxT("Save (Ctrl+S)"));
	tb->AddTool(wxID_SAVEAS, wxT("Save As"), wxXmlResource::Get()->LoadBitmap(wxT("save_as")), wxT("Save As"));
	tb->AddTool(XRCID("save_all"), wxT("Save All"), wxXmlResource::Get()->LoadBitmap(wxT("save_all")), wxT("Save All"));
	tb->AddSeparator();
	tb->AddTool(wxID_CLOSE, wxT("Close"), wxXmlResource::Get()->LoadBitmap(wxT("page_close")), wxT("Close File (Ctrl+W)"));
	tb->AddSeparator();
	tb->AddTool(wxID_CUT, wxT("Cut"), wxXmlResource::Get()->LoadBitmap(wxT("cut")), wxT("Cut (Ctrl+X)"));
	tb->AddTool(wxID_COPY, wxT("Copy"), wxXmlResource::Get()->LoadBitmap(wxT("copy")), wxT("Copy (Ctrl+C)"));
	tb->AddTool(wxID_PASTE, wxT("Paste"), wxXmlResource::Get()->LoadBitmap(wxT("paste")), wxT("Paste (Ctrl+V)"));
	tb->AddSeparator();
	tb->AddTool(wxID_UNDO, wxT("Undo"), wxXmlResource::Get()->LoadBitmap(wxT("undo")), wxT("Undo (Ctrl+Z)"));
	tb->AddTool(wxID_REDO, wxT("Redo"), wxXmlResource::Get()->LoadBitmap(wxT("redo")), wxT("Redo (Ctrl+Y)"));
	
	tb->AddSeparator();
	tb->AddTool(XRCID("toggle_bookmark"), wxT("Toggle Bookmark"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark")), wxT("Toggle Bookmark (Ctrl+F2)"));
	tb->AddTool(XRCID("next_bookmark"), wxT("Next Bookmark"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark_previous")), wxT("Next Bookmark (F2)"));
	tb->AddTool(XRCID("previous_bookmark"), wxT("Previous Bookmark"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark_next")), wxT("Previous Bookmark (Shift+F2)"));
	tb->AddTool(XRCID("removeall_bookmarks"), wxT("Remove All Bookmarks"), wxXmlResource::Get()->LoadBitmap(wxT("bookmark_remove_all")), wxT("Remove All Bookmarks"));
	
	tb->AddSeparator();
	tb->AddTool(wxID_FIND, wxT("Find and Replace"), wxXmlResource::Get()->LoadBitmap(wxT("find_and_replace")), wxT("Find And Replace (Ctrl+F)"));
	tb->AddTool(XRCID("find_in_files"), wxT("Find In Files"), wxXmlResource::Get()->LoadBitmap(wxT("find_in_files")), wxT("Find In Files (Ctrl+Shift+F)"));
	
	tb->AddSeparator();

	wxArrayString choices;
	m_workspaceConfig = new wxComboBox(tb, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices, wxCB_READONLY);
	m_workspaceConfig->Enable(false);
	tb->AddControl(m_workspaceConfig);
	
	// Connect an event to handle changes in the choice control
	ConnectCombo(m_workspaceConfig, Frame::OnWorkspaceConfigChanged);
	
	tb->AddTool(XRCID("build_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("build_active_project")), wxT("Build Active Project (Ctrl+B)"));
	tb->AddTool(XRCID("clean_active_project"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("clean")), wxT("Clean Active Project"));
	tb->AddTool(XRCID("stop_active_project_build"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("stop_build")), wxT("Stop Current Build"));
	tb->AddTool(XRCID("execute_no_debug"), wxEmptyString, wxXmlResource::Get()->LoadBitmap(wxT("execute")), wxT("Run Active Project (Ctrl+F5)"));

	tb->Realize();
	m_mgr.AddPane(tb, wxAuiPaneInfo().Name(wxT("Standard Toolbar")).LeftDockable(true).RightDockable(true).Caption(wxT("Standard")).ToolbarPane().Top());
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

	if(event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID){
		//keep the old id as int and override the value set in the event object
		//to trick the event system
		event.SetInt(event.GetId());
		event.SetId(viewAsMenuItemID);
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

	if(event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID){
		//keep the old id as int and override the value set in the event object
		//to trick the event system
		event.SetInt(event.GetId());
		event.SetId(viewAsMenuItemID);
	}
	editor->OnUpdateUI(event);
}

void Frame::OnFileExistUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
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
	ManagerST::Get()->KillProgram();
	SearchThreadST::Get()->StopSearch();
	EditorConfigST::Get()->SavePerspective(wxT("Default"), m_mgr.SavePerspective());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("Editor"), m_notebook->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("OutputPane"), m_outputPane->GetNotebook()->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveNotebookStyle(wxT("WorkspacePane"), m_workspacePane->GetNotebook()->GetWindowStyleFlag());
	EditorConfigST::Get()->SaveLexers();
	event.Skip();
}

wxString Frame::GetStringFromUser(const wxString& msg)
{
	wxString userString;
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, msg, wxT("Enter Text"));
	if(dlg->ShowModal() == wxID_OK)
	{
		userString = dlg->GetValue();
	}
	dlg->Destroy();
	return userString;
}

void Frame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	// SaveFile contains the logic of "Untiltled" files
	editor->SaveFile();
}

void Frame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	editor->SaveFileAs();
}

void Frame::OnFileReload(wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;

	editor->ReloadFile();
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
	wxDirDialog *dlg = new wxDirDialog(	this, 
										wxT("Select the root directory of the include path:"), 
										ManagerST::Get()->GetStarupDirectory());
	if(dlg->ShowModal() == wxID_OK)
	{
		// get output file name from user
		wxString db = GetStringFromUser(wxT("Insert database name:"));

		// Get the dirname
		wxString path = dlg->GetPath();
		TrimString(path);

		if(!path.IsEmpty()){
			wxFileName fn(path);
			wxFileName dbname(ManagerST::Get()->GetStarupDirectory() + PATH_SEP + db);

			// set cursor to busy
			wxBusyCursor busy;

			// build the external database
			TagsManagerST::Get()->BuildExternalDatabase(fn, dbname, wxEmptyString, this);
		}
	}

	dlg->Destroy();
}

void Frame::OnUseExternalDatabase(wxCommandEvent& WXUNUSED(event))
{
	const wxString ALL(	wxT("Tags Database File (*.db)|*.db|")
						wxT("All Files (*.*)|*.*"));
	
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Select an external database:"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK)
	{
		// get the path
		wxFileName dbname(dlg->GetPath());
		ManagerST::Get()->SetExternalDatabase(dbname);
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

	m_notebook->Freeze();
	//allocate new editor instance using the creator
	//this is done due to low performance on GTK 
	LEditor *editor = EditorCreatorST::Get()->NewInstance();
	editor->SetFileName(fileName);

	m_notebook->AddPage(editor, fileName.GetFullName(), true);
	m_notebook->Thaw();
	editor->SetFocus ();
	editor->SetSCIFocus(true);
}

void Frame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
{
	const wxString ALL(	wxT("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open File"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK){
		wxArrayString paths;
		dlg->GetPaths(paths);
		for(size_t i=0; i<paths.GetCount(); i++){
			ManagerST::Get()->OpenFile(paths.Item(i), wxEmptyString);
		}
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
	ClosePage(editor, false, m_notebook->GetSelection(), true, veto);
}

void Frame::OnFileClosing(wxFlatNotebookEvent &event)
{
	// get the page that is now closing
	LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage(event.GetSelection()));
	if( !editor )
		return;	

	bool veto;
	ClosePage(editor, true, event.GetSelection(), false, veto);
	if( veto ) 
	{
		event.Veto();
	}
	else
	{
		//update the symbol view 
		GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
	}
	event.Skip();
}

void Frame::OnPageChanged(wxFlatNotebookEvent &event)
{
	// pass the event to the editor
	LEditor *editor = dynamic_cast<LEditor*>( m_notebook->GetPage(event.GetSelection()) );
	if( !editor ){
		return;
	}
	
	//update the symbol view as well
	GetWorkspacePane()->DisplaySymbolTree(editor->GetFileName());
	editor->SetActive();
	event.Skip();
}

void Frame::OnPageClosed(wxFlatNotebookEvent &event)
{
	wxUnusedVar(event);
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

void Frame::ClosePage(LEditor *editor, bool notify, int index, bool doDelete, bool &veto)
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
					//we faild in saving the file, dont allow the tab removal
					//process to continue
					veto = true;
					return;
				} else {
					if( doDelete ) 
					{
						GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
						m_notebook->DeletePage(index, notify);
					}
				}
			}
			break;
		case wxCANCEL:
			veto = true;
			return; // do nothing
		case wxNO:
			// just delete the tab without saving the changes
			if( doDelete ) 
			{
				GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
				m_notebook->DeletePage(index, notify);
			}
			break;
		}
	} 
	else 
	{
		// file is not modified, just remove the tab
		if( doDelete ) 
		{
			GetWorkspacePane()->DeleteSymbolTree(editor->GetFileName());
			m_notebook->DeletePage(index, notify);
		} // if( doDelete )
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

	//if we have an open editor, and a selected text, make this text
	//the search string
	if(m_notebook->GetPageCount() > 0)
	{
		LEditor *editor = dynamic_cast<LEditor*>(m_notebook->GetPage((size_t)m_notebook->GetSelection()));
		if(editor)
		{
			wxString selText = editor->GetSelectedText();
			if(selText.IsEmpty() == false)
			{
				m_findInFilesDlg->GetData().SetFindString(selText);
			}
		}
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
			ManagerST::Get()->CreateProject(data);
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
		TagsManagerST::Get()->ParseComments(dlg->GetCtagsOptions().GetParseComments());
	}
	dlg->Destroy();
}

void Frame::OnViewOutputPane(wxCommandEvent &event)
{
	ViewPane(wxT("Output"), event);
}

void Frame::OnViewWorkspacePane(wxCommandEvent &event)
{
	ViewPane(wxT("Workspace"), event);
}

void Frame::OnViewToolbar(wxCommandEvent &event)
{
	ViewPane(wxT("Standard Toolbar"), event);
}

void Frame::OnViewWorkspacePaneUI(wxUpdateUIEvent &event){
	ViewPaneUI(wxT("Workspace"), event);
}

void Frame::OnViewToolbarUI(wxUpdateUIEvent &event){
	ViewPaneUI(wxT("Standard Toolbar"), event);
}

void Frame::OnViewOutputPaneUI(wxUpdateUIEvent &event){
	ViewPaneUI(wxT("Output"), event);
}

void Frame::ViewPane(const wxString &paneName, wxCommandEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
	if(info.IsOk()){
		if( event.IsChecked() ){
			info.Show();
		} else {
			info.Hide();
		}
		m_mgr.Update();
	}

}

void Frame::ViewPaneUI(const wxString &paneName, wxUpdateUIEvent &event)
{
	wxAuiPaneInfo &info = m_mgr.GetPane(paneName);
	if(info.IsOk()){
		event.Check(info.IsShown());
	}
}

void Frame::OnViewOptions(wxCommandEvent & WXUNUSED( event))
{
	OptionsDlg *dlg = new OptionsDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnConfigurationManager(wxCommandEvent &event)
{	
	wxUnusedVar(event);
	ConfigurationManagerDlg *dlg = new ConfigurationManagerDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnTogglePanes(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->TogglePanes();
}

void Frame::OnAddEnvironmentVariable(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EnvVarsTableDlg *dlg = new EnvVarsTableDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnAdvanceSettings(wxCommandEvent &event)
{
	wxUnusedVar(event);
	AdvancedDlg *dlg = new AdvancedDlg(this);
	dlg->ShowModal();
	dlg->Destroy();
}

void Frame::OnBuildEvent(wxCommandEvent &event)
{
	// make sure that the output pane is visible and selection
	// is set to the 'Find In Files' tab
	m_outputPane->CanFocus(true);
	ManagerST::Get()->ShowOutputPane(OutputPane::BUILD_WIN);
	if(event.GetEventType() == wxEVT_BUILD_STARTED){
		m_outputPane->Clear();
		m_outputPane->AppendText(OutputPane::BUILD_WIN, BUILD_START_MSG);
	}else if(event.GetEventType() == wxEVT_BUILD_ADDLINE){
		m_outputPane->AppendText(OutputPane::BUILD_WIN, event.GetString());
	}else if(event.GetEventType() == wxEVT_BUILD_ENDED){
		m_outputPane->AppendText(OutputPane::BUILD_WIN, BUILD_END_MSG);
	}
}

void Frame::OnOutputWindowEvent(wxCommandEvent &event)
{
	// make sure that the output pane is visible and selection
	// is set to the 'Find In Files' tab
	m_outputPane->CanFocus(true);
	ManagerST::Get()->ShowOutputPane(OutputPane::OUTPUT_WIN);
	if(event.GetEventType() == wxEVT_ASYNC_PROC_STARTED){
		m_outputPane->Clear();
		m_outputPane->AppendText(OutputPane::OUTPUT_WIN, event.GetString());
	}else if(event.GetEventType() == wxEVT_ASYNC_PROC_ADDLINE){
		m_outputPane->AppendText(OutputPane::OUTPUT_WIN, event.GetString());
	}else if(event.GetEventType() == wxEVT_ASYNC_PROC_ENDED){
		m_outputPane->AppendText(OutputPane::OUTPUT_WIN, event.GetString());
	}
}

// Build operations
void Frame::OnBuildProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->BuildProject(ManagerST::Get()->GetActiveProjectName());
}

void Frame::OnBuildProjectUI(wxUpdateUIEvent &event)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void Frame::OnStopBuildUI(wxUpdateUIEvent &event)
{
	bool enable = ManagerST::Get()->IsBuildInProgress();
	event.Enable(enable);
}

void Frame::OnStopBuild(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->StopBuild();
}

void Frame::OnCleanProject(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->CleanProject(ManagerST::Get()->GetActiveProjectName());
}

void Frame::OnCleanProjectUI(wxUpdateUIEvent &event)
{
	bool enable = !ManagerST::Get()->IsBuildInProgress() && !ManagerST::Get()->GetActiveProjectName().IsEmpty();
	event.Enable(enable);
}

void Frame::OnExecuteNoDebug(wxCommandEvent &event)
{
	wxUnusedVar(event);
	ManagerST::Get()->ExecuteNoDebug(ManagerST::Get()->GetActiveProjectName());
}

void Frame::OnExecuteNoDebugUI(wxUpdateUIEvent &event)
{
	event.Enable(ManagerST::Get()->GetActiveProjectName().IsEmpty() == false
				&&
				!ManagerST::Get()->IsBuildInProgress()
				&&
				!ManagerST::Get()->IsProgramRunning());
}

void Frame::OnWorkspaceConfigChanged(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString selectionStr = m_workspaceConfig->GetStringSelection();
	//update the workspace configuration file
	ManagerST::Get()->SetWorkspaceConfigurationName(selectionStr);
}

void Frame::OnTimer(wxTimerEvent &event)
{
	//increase pool size if needed
	if(EditorCreatorST::Get()->GetQueueCount() == 0){
		EditorCreatorST::Get()->Add(new LEditor(m_notebook, wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, true)); 
	}
	event.Skip();
}

void Frame::OnFileCloseAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	bool modifyDetected = false;

	//check if any of the files is modified
	for(int i=0; i<m_notebook->GetPageCount(); i++)
	{
		LEditor *editor = dynamic_cast<LEditor*>(m_notebook->GetPage((size_t)i));
		if(editor)
		{
			if(editor->GetModify())
			{
				modifyDetected = true;
				break;
			}
		}
	}

	int retCode(CLOSEALL_DISCARDALL);
	CloseAllDialog *dlg(NULL);

	if(modifyDetected)
	{
		dlg = new CloseAllDialog(this);
		retCode = dlg->ShowModal();
	}

	switch(retCode)
	{
		case CLOSEALL_SAVEALL:
			{
				ManagerST::Get()->SaveAll();
				//and now close them all
				m_notebook->DeleteAllPages();
				m_notebook->Refresh();
			}
			break;
		case CLOSEALL_DISCARDALL:
			{
				m_notebook->DeleteAllPages();
				m_notebook->Refresh();
			}
			break;
		case CLOSEALL_ASKFOREACHFILE:
			{
				int count = m_notebook->GetPageCount();
				for(int i=0; i<count; i++)
				{
					LEditor* editor = dynamic_cast<LEditor*>(m_notebook->GetPage((size_t)i));
					if( !editor )
						continue;

					bool veto;
					ClosePage(editor, false, m_notebook->GetSelection(), true, veto);
				}
				//once all files have been prompted if needed, remove them all
				m_notebook->DeleteAllPages();
				m_notebook->Refresh();
			}
			break;
		default:
			break;
	}
	
	if(dlg)
	{
		dlg->Destroy();	
	}
}


void Frame::OnFindResource(wxCommandEvent &event)
{
	wxUnusedVar(event);
	OpenResourceDlg *dlg = new OpenResourceDlg(this);
	if(dlg->ShowModal() == wxID_OK)
	{
		wxString fileName = dlg->GetFileName();
		if(fileName.IsEmpty() == false)
		{
			wxString projectName = ManagerST::Get()->GetProjectNameByFile(fileName);
			ManagerST::Get()->OpenFile(fileName, projectName);
		}
	}
	dlg->Destroy();
}

void Frame::OnImportMakefileUI(wxUpdateUIEvent &event)
{
	event.Enable(false);
}

void Frame::OnImportMakefile(wxCommandEvent &event)
{
	wxUnusedVar(event);
	const wxString ALL(wxT("Import makefile files (Makefile) |Makefile|")
					   wxT("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, wxT("Open Makefile"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK){
		ManagerST::Get()->ImportFromMakefile(dlg->GetPath());
	}
	dlg->Destroy();
}

void Frame::OnAddSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if(tree)
		tree->AddSymbols(event);
}

void Frame::OnDeleteSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if(tree)
		tree->DeleteSymbols(event);
}

void Frame::OnUpdateSymbols(SymbolTreeEvent &event)
{
	SymbolTree *tree = GetWorkspacePane()->GetSymbolTree();
	if(tree)
		tree->UpdateSymbols(event);
}
