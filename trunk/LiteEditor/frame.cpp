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
#include "editor.h"
#include "cpp_symbol_tree.h"
#include "language.h"
#include "process.h"
#include "editor_config.h"
#include <wx/config.h>
#include <wx/confbase.h>
#include "manager.h"
#include "menumanager.h"
#include <wx/aboutdlg.h>

#define ID_CTAGS_GLOBAL_ID		10500
#define ID_CTAGS_LOCAL_ID		10501

extern wxImageList* CreateSymbolTreeImages();
//----------------------------------------------------------------
// Our main frame
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_END_PROCESS(ID_CTAGS_GLOBAL_ID, Frame::OnCtagsEnd)
	EVT_END_PROCESS(ID_CTAGS_LOCAL_ID, Frame::OnCtagsEnd)

	// Handler menu events
	EVT_MENU(wxID_EXIT, Frame::OnQuit)
	EVT_MENU(wxID_SAVE, Frame::OnSave)
	EVT_MENU(wxID_SAVEAS, Frame::OnSaveAs)
	EVT_MENU(XRCID("about"), Frame::OnAbout)
	EVT_MENU(wxID_NEW, Frame::OnFileNew)
	EVT_MENU(XRCID("add_file_to_project"), Frame::OnAddSourceFile)
	EVT_MENU(XRCID("open_workspace"), Frame::OnBuildFromDatabase)
	EVT_MENU(wxID_OPEN, Frame::OnFileOpen)
	EVT_FLATNOTEBOOK_PAGE_CLOSING(-1, Frame::OnFileClosing)
	EVT_FLATNOTEBOOK_PAGE_CHANGED(-1, Frame::OnPageChanged)
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
	EVT_UPDATE_UI(wxID_FIND, Frame::OnFileExistUpdateUI)

	/*
	EVT_MENU(ID_COMPLETE_WORD, Frame::OnCompleteWord)
	EVT_MENU(ID_GOTO_DEFINTION, Frame::OnGotoDefinition)
	EVT_MENU(ID_GOTO_PREV_DEFINTION, Frame::OnGotoPreviousDefinition)
	EVT_MENU(ID_DELETE_PROJECT, Frame::OnDeleteProject)
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
{
	CreateGUIControls();
}

Frame::~Frame(void)
{
	delete wxLog::SetActiveTarget(m_logTargetOld);

	// Release singleton objects
	TagsManagerST::Free();
	LanguageST::Free();
	EditorConfigST::Free();
	Manager::Free();

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
	wxFlatNotebook::CleanUp();
	MenuManager::Free();

	if(m_config)
		delete m_config;

	// uninitialize AUI manager
	m_mgr.UnInit();
}

Frame* Frame::Get()
{
    // Startup size will be 90% of the screen size
    int frameWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) * 90/100;
    int frameHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) * 90/100;
    
	if( !m_theFrame )
		m_theFrame = new Frame(NULL, wxID_ANY, _("Lite Editor"), wxDefaultPosition, wxSize(frameWidth, frameHeight));
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
	// Add log window
	//---------------------------------------------
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, _T(""),
                            wxDefaultPosition, wxSize(-1, 200), wxTE_MULTILINE);
	m_mgr.AddPane(text, wxAuiPaneInfo().Name(wxT("Debug Window")).
		Caption(wxT("Debug Window")).Bottom().Layer(1).Position(1).MaximizeButton(true).CloseButton(true));

	m_logTargetOld = wxLog::SetActiveTarget(new wxLogTextCtrl(text));

	// Add the class view tree
	m_tree = new CppSymbolTree(this, wxID_ANY, wxDefaultPosition, wxSize(250, 600));

	// Set the images for the symbols
	m_tree->SetSymbolsImages( CreateSymbolTreeImages() );

	// Add the tree pane
	m_mgr.AddPane(m_tree, wxAuiPaneInfo().
		Name(wxT("SymbolTree")).Caption(wxT("Symbol Tree")).
		Left().Layer(1).Position(1).
		CloseButton(true).MaximizeButton(true));

	// Create the notebook for all the files
	long style = 
		wxFNB_TABS_BORDER_SIMPLE | 
		wxFNB_NODRAG | 
		wxFNB_VC8 | 
		wxFNB_BACKGROUND_GRADIENT | 
		wxFNB_NO_X_BUTTON |
		wxFNB_NO_NAV_BUTTONS |
		wxFNB_DROPDOWN_TABS_LIST |
		wxFNB_SMART_TABS |
		wxFNB_X_ON_TAB ;

	m_notebook = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	m_mgr.AddPane(m_notebook, wxAuiPaneInfo().Name(wxT("Editor")).
                  CenterPane().PaneBorder(false));

	//--------------------------------------------------------------------------------------
	// Start ctags process. 
	// ctags process must be started somewhere in the main frames' (here is a good place ^^)
	// constructor since to execute it, we are using wxExecute() which,  
	// according to the documentation, will fail if used outside of the main thread. 
	//--------------------------------------------------------------------------------------
	DEBUG_MSG(_T("Starting ctags processes ..."))
	// We keep a pointer to wxProcess object returend from ctags starting process
	m_ctags = TagsManagerST::Get()->StartCtagsProcess(this, 10500, TagsGlobal);
	m_localCtags = TagsManagerST::Get()->StartCtagsProcess(this, 10501, TagsLocal);
	DEBUG_MSG(_T("Done"))

	//--------------------------------------------------------------------------------------
	// Start the parsing thread, the parsing thread and the SymbolTree (or its derived)
	// Are connected. The constructor of SymbolTree, calls ParseThreadST::Get()->SetNotifyWindow(this)
	// to allows it to receive events for gui changes.
	//
	// If you wish to connect another object for it, simply call ParseThreadST::Get()->SetNotifyWindow(this)
	// with another object as 'this'
	//--------------------------------------------------------------------------------------
	DEBUG_MSG(_T("Starting parser thread ..."))
	ParseThreadST::Get()->Start();
	DEBUG_MSG(_T("Done"))

	// And finally create a status bar
	wxStatusBar* statusBar = new wxStatusBar(this, wxID_ANY);
	statusBar->SetFieldsCount(3);
	SetStatusBar(statusBar);
	
	GetStatusBar()->SetStatusText(_("Welcome to CodeLite sample application!"));

	// Get the install path from the configuration
	m_config = wxConfigBase::Get();
	m_config->Read(_T("LiteEditor/InstallPath"), &m_installPath);
	
	// "commit" all changes made to wxAuiManager
    m_mgr.Update();

	SetAutoLayout (true);
	Layout();

	
}

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void Frame::DispatchCommandEvent(wxCommandEvent &event)
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
	if( !editor )
		return;	

	editor->OnMenuCommand(event);
}

void Frame::DispatchUpdateUIEvent(wxUpdateUIEvent &event)
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));
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
    info.SetName(wxT("CodeLite"));
    info.SetVersion(wxT("0.1 Alpha"));
    info.SetDescription(wxT("A lighweight cross-platform editor for C/C++ "));
    info.SetCopyright(wxT("(C) 2007 By Eran Ifrah <eran.ifrah@gmail.com>"));
    wxAboutBox(info);
}

void Frame::OnClose(wxCloseEvent& event)
{
	// Since we dont want CTAGS process to be restarted
	// we turn the 'restartCtags' flag to false
	m_restartCtags = false;
	wxKill(m_localCtags->GetPid(), wxSIGKILL);
	wxKill(m_ctags->GetPid(), wxSIGKILL);

	// Sleep for 20 ms to allow the process to be killed and 
	// the main frame to handle the event or else we can get 
	// memory leak
	wxMilliSleep( 20 );
	event.Skip();
}

// Provide a callback function for the process termination
void Frame::OnCtagsEnd(wxProcessEvent& event)
{
	//-----------------------------------------------------------
	// This event handler is a must if you wish to delete 
	// the process and prevent memory leaks
	// In addition, I implemented here some kind of a watchdog
	// mechanism: if ctags process terminated abnormally, it will
	// be restarted automatically by this function (unless the 
	// termination of it was from OnClose() function, then we 
	// choose to ignore the restart)
	//-----------------------------------------------------------

	// Which ctags process died?
	switch( event.GetId() )
	{
	case ID_CTAGS_GLOBAL_ID:
		delete m_ctags;
		if(m_restartCtags)
			m_ctags = TagsManagerST::Get()->StartCtagsProcess(this, ID_CTAGS_GLOBAL_ID, TagsGlobal);
		break;
	case ID_CTAGS_LOCAL_ID:
		delete m_localCtags;
		if(m_restartCtags)
			m_localCtags = TagsManagerST::Get()->StartCtagsProcess(this, ID_CTAGS_LOCAL_ID, TagsLocal);
		break;
	default:
		break;
	}
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

//-----------------------------------------------------------
// Demonstrate adding source file to an existing database
//-----------------------------------------------------------
void Frame::OnAddSourceFile(wxCommandEvent& WXUNUSED(event))
{   
	const wxString ALL( _T("C/C++ Files (*.c;*.cpp;*.cc;*.cxx;*.C;*.h;*.hpp;*.hh;*.hxx;*.H)|*.c;*.cpp;*.cc;*.cxx;*.C;*.h;*.hpp;*.hh;*.hxx;*.H|")
                        _T("C/C++ Source Files (*.c;*.cpp;*.cc;*.cxx;*.C)|*.c;*.cpp;*.cc;*.cxx;*.C|")
						_T("C/C++ Header Files (*.h;*.hpp;*.hh;*.hxx;*.H)|*.h;*.hpp;*.hh;*.hxx;*.H|")
						_T("All Files (*.*)|*.*") );
	wxFileDialog *dlg = new wxFileDialog(this, _("Open file"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK)
	{
		wxArrayString files;
		wxString projectName;

		// get the paths
		dlg->GetPaths(files);
		
		// Prompt user for project name for this source file
		projectName = GetStringFromUser(_("Enter a project name for this source files (can be a new project name):"));

		if(projectName.IsEmpty())
			projectName = _("Project One");
		
		// Convert the string array into wxFileName vector
		std::vector<wxFileName> fp_vector;
		for(size_t i=0; i<files.GetCount(); i++)
			fp_vector.push_back(wxFileName(files[i]));
					
		// Get tags from the source files, and save them to the database
		std::vector<DbRecordPtr> comments;

		TagTreePtr tree;
		if( TagsManagerST::Get()->GetParseComments() )
			tree = TagsManagerST::Get()->ParseSourceFiles(fp_vector, projectName, &comments);
		else
			tree = TagsManagerST::Get()->ParseSourceFiles(fp_vector, projectName);

		// We store it to database, we don't provide database name since we want to use
		// the one that is opened, if no database is open, we will prompt the user for 
		// database
		wxFileName dbName;
		if(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().IsOk() == false)
		{
			dbName = wxFileName( GetStringFromUser(_("No database is currently open, please insert name of database to use (example: C:\\Tags.db):")) );
			if(dbName.IsOk() == false)
				return;
		}
		else
			dbName = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();

		// Store the tree into the database using dbName as the underlying file
		TagsManagerST::Get()->Store(tree, dbName);

		// if comments are enabled, store them as well
		if( TagsManagerST::Get()->GetParseComments() && comments.empty() == false )
			TagsManagerST::Get()->StoreComments( comments, dbName );

		// Rebuild the gui tree. The data to build the gui tree is taken from the database.
		// Since we dont provide a paramter to the BuildTree() function, it will use the 
		// currently opened database
		// Note that the tree is constructed from the tree only and not from the rejected tags
		TagTreePtr dummy;
		m_tree->BuildTree( dummy );
		GetStatusBar()->SetStatusText(wxString::Format(_("Workspace DB: '%s'"), dbName.GetFullPath().GetData()), 1);
	}
	dlg->Destroy();
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

void Frame::OnBuildFromDatabase(wxCommandEvent& WXUNUSED(event))
{
	const wxString ALL(_T("Tags Database File (*.db)|*.db|")
					   _T("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, _("Open file"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST | wxMULTIPLE , wxDefaultPosition);

	if (dlg->ShowModal() == wxID_OK)
	{
		// get the path
		wxFileName databaseFile(dlg->GetPath());
		TagsManagerST::Get()->OpenDatabase(databaseFile);

		//------------------------------------------------------------------------------------------
		// Re-build the gui tree
		// This function, if it does not receive a TagTree to work with, it will call
		// TagsManagerST::Get()->Load() - which will return the tree for the current
		// open database
		// By calling TagsManagerST::Get()->OpenDatabase(databaseFile);, we force the BuildTree() 
		// function to work with our database
		//------------------------------------------------------------------------------------------
		TagTreePtr dummy;
		m_tree->BuildTree( dummy );

		GetStatusBar()->SetStatusText(wxString::Format(_("Workspace DB: '%s'"), databaseFile.GetFullPath().GetData()), 1);
	}
	dlg->Destroy();
}

/// Open file and set the cursor to be on the line
void Frame::OpenFile(const TagEntry& tag)
{
	wxFileName fileName(tag.GetFile());

	// Search to see if this file is already opened
	// in the notebook
	LEditor* editor = NULL;
	size_t nCount = 0;
	for(; nCount < (size_t)m_notebook->GetPageCount(); nCount++)
	{
		editor = static_cast<LEditor*>(m_notebook->GetPage(nCount));
		if( editor->GetFileName() == fileName.GetFullPath() )
		{
			m_notebook->SetSelection( nCount );
			break;
		}
		editor = NULL;
	}

	if( !editor )
	{
		/// Open the file and read the text
		if(fileName.IsOk() == false)
			return;

		// Create new editor and add it to the notebook
		m_notebook->Freeze();
		editor = new LEditor(this, wxID_ANY, wxSize(1, 1), fileName.GetFullPath(), tag.GetProject());
		m_notebook->AddPage(editor, fileName.GetFullName(), true);
		m_notebook->Thaw();
	}

	// Go to tag line number and gives scintilla the focus
	editor->GotoLine( tag.GetLine() - 1 );
	editor->SetFocus ();
}

//------------------------------------------------------
// Complete word, complete a word from the current caret
// position.
// The list of words, is constructed from the database &
// from the local scope
//------------------------------------------------------
void Frame::OnCompleteWord(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;
	editor->CompleteWord();	
}

void Frame::OnGotoDefinition(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;
	editor->GotoDefinition();	
}

void Frame::OnGotoPreviousDefinition(wxCommandEvent& WXUNUSED(event))
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;
	editor->GotoPreviousDefintion();	
}

void Frame::OnDeleteProject(wxCommandEvent& WXUNUSED(event))
{
	wxString projectName = GetStringFromUser(_("Insert Project name to delete:"));

	// this function will validate the project name for us
	TagsManagerST::Get()->DeleteProject(projectName);

	// Notify the symbol tree to update the gui
	SymbolTreeEvent evt(projectName, wxEVT_COMMAND_SYMBOL_TREE_DELETE_PROJECT);
	wxPostEvent(m_tree, evt);
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
	const wxString ALL(	_T("Tags Database File (*.db)|*.db|")
						_T("All Files (*.*)|*.*"));
	
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
	LEditor *editor = new LEditor(this, wxID_ANY, wxSize(1, 1), fileName.GetFullPath(), wxEmptyString);
	m_notebook->AddPage(editor, fileName.GetFullName(), true);
	m_notebook->Thaw();

	editor->SetFocus ();
}

void Frame::OnFileOpen(wxCommandEvent & WXUNUSED(event))
{
	const wxString ALL(	_T("All Files (*.*)|*.*"));
	wxFileDialog *dlg = new wxFileDialog(this, _("Open File"), wxEmptyString, wxEmptyString, ALL, wxOPEN | wxFILE_MUST_EXIST , wxDefaultPosition);
	if (dlg->ShowModal() == wxID_OK)
	{
		// get the path
		wxFileName fname(dlg->GetPath());
		m_notebook->Freeze();
		LEditor *editor = new LEditor(this, wxID_ANY, wxSize(1, 1), fname.GetFullPath(), wxEmptyString);
		m_notebook->AddPage(editor, fname.GetFullName(), true);
		m_notebook->Thaw();
		editor->SetFocus ();
	}
	dlg->Destroy();	
}

void Frame::OnFileClose(wxCommandEvent &event)
{
	wxUnusedVar( event );
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetCurrentPage());
	if( !editor )
		return;
	
	bool veto;
	ClosePage(editor, m_notebook->GetSelection(), true, veto);
}

void Frame::OnFileClosing(wxFlatNotebookEvent &event)
{
	// get the page that is now closing
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetPage(event.GetSelection()));
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
	LEditor *editor = static_cast<LEditor*>(m_notebook->GetPage(event.GetSelection()));
	editor->SetActive();
	event.Skip();
}

void Frame::OnFileSaveAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Manager::Get()->SaveAll();
}

void Frame::OnCompleteWordUpdateUI(wxUpdateUIEvent &event)
{
	LEditor* editor = static_cast<LEditor*>(m_notebook->GetPage(m_notebook->GetSelection()));

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
