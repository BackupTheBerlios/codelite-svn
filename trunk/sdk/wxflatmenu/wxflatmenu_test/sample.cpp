//--------------------------------------------------------------------
// Sample class for using wxFlatTable class
//--------------------------------------------------------------------

#include <wx/sysopt.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>

#include "../wxflatmenu/flat_menu.h"
#include "../wxflatmenu/flat_menu_bar.h"
#include "../wxflatmenu/imagebook.h"
#include "sample_resources.h"

//-------------------------------
// Menu items IDs
//-------------------------------
#define MENU_DISABLE_MENU_ITEM	10000
#define MENU_STYLE_XP			10001
#define MENU_STYLE_2007			10002
#define MENU_REMOVE_MENU		10003

#define MENU_NEW_FILE			10005
#define MENU_SAVE				10006
#define MENU_OPEN_FILE			10007
#define MENU_NEW_FOLDER			10008
#define MENU_COPY				10009
#define MENU_CUT				10010
#define MENU_PASTE				10011



//-------------------------------
#ifdef DEVELOPMENT
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; wxLogMessage( logmsg ); }
#else
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; }
#endif

class Frame;

//--------------------------------------------------------------------
// Declare our applicaion
//--------------------------------------------------------------------
class App : public wxApp
{
public:
	App(void) {};
	~App(void) {};
	virtual bool OnInit(void);
};
DECLARE_APP(App)

//------------------------------------------------------------
// Declare our main frame
//------------------------------------------------------------

class Frame : public wxFrame
{
	wxFlatMenu *m_popUpMenu;
	wxFlatMenu *m_longPopUpMenu;
	wxLog *m_logTargetOld;
	wxFlatMenuBar* m_mb;
	bool m_useDecoration;

#if wxUSE_AUI
//	wxFrameManager m_mgr;
#endif

public:
	// Construction of MainFrame
	Frame(wxWindow* parent, const wxChar *title);
	virtual ~Frame(void);

protected:
	bool Create(wxFrame * parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style );
	void ConnectEvents();
	void CreatePopupMenu();
	void CreateLongPopupMenu();

	wxString GetStringFromUser(const wxString& msg);

	// Event handlers
	void OnQuit(wxCommandEvent &event);
	void OnButtonClicked(wxCommandEvent & event);
	void OnLongButtonClicked(wxCommandEvent & event);
	void OnStyle(wxCommandEvent &event);
	void OnEdit(wxCommandEvent &event);
	void OnFlatMenuCmd(wxCommandEvent &event);
	void OnFlatMenuCmdUI(wxUpdateUIEvent &event);
};


//------------------------------------------------------------
// Implement out app
//------------------------------------------------------------
IMPLEMENT_APP(App)

// Initialize our application
bool App::OnInit(void)
{ 
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(wxT("msw.remap"), 0); 
 	wxImage::AddHandler( new wxPNGHandler ); 
	wxImage::AddHandler( new wxCURHandler ); 
	wxImage::AddHandler( new wxBMPHandler );
	wxImage::AddHandler( new wxXPMHandler );

	Frame *pMainFrame = new Frame(NULL, wxT("Frame"));

	// show the main frame
	pMainFrame->Show(TRUE);

	// set the main frame to be the main window
	SetTopWindow(pMainFrame);
	return TRUE;	
}

//------------------------------------------------------------
// Implement out frame
//------------------------------------------------------------

Frame::Frame(wxWindow* WXUNUSED(parent), const wxChar *title)
{
	Create(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);
	ConnectEvents();
}

Frame::~Frame(void)
{
#if wxUSE_AUI
	//m_mgr.UnInit();
#endif

	delete wxLog::SetActiveTarget(m_logTargetOld);
	wxArtManagerST::Free();
}

bool Frame::Create(wxFrame * parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
	m_useDecoration = true;
	m_popUpMenu = NULL;
	m_longPopUpMenu = NULL;
	if( !wxFrame::Create(parent, id, caption, pos, size, style) ) 
		return false;

	CreateStatusBar(2);

	GetStatusBar()->SetStatusText(wxT("Welcome to wxFlatMenu demo application!"));
#if wxUSE_AUI
	//m_mgr.SetFrame(this); 
#endif 

	wxFlatMenu *fileMenu  = new wxFlatMenu();
	wxFlatMenu *styleMenu = new wxFlatMenu();
	wxFlatMenu *editMenu  = new wxFlatMenu();
	wxFlatMenu* subMenu = new wxFlatMenu();
	wxFlatMenu* subMenu1 = new wxFlatMenu();
	
	// Create the menubar
	m_mb = new wxFlatMenuBar(this, wxID_ANY, true, LargeIcons); 

	// Set an icon to the exit menu item
	wxImage img(exit_xpm);
	img.SetAlpha(exit_alpha, true);
	wxBitmap exitImg(img); 

	// Load toolbar icons (32x32)
	wxBitmap new_file_bmp, 
			 save_bmp, 
			 open_folder_bmp, 
			 new_folder_bmp, 
			 copy_bmp, 
			 cut_bmp, 
			 paste_bmp;

	img = wxImage(copy_xpm);
	img.SetAlpha(copy_alpha, true);
	copy_bmp = wxBitmap(img);
	
	img = wxImage(cut_xpm);
	img.SetAlpha(cut_alpha, true);
	cut_bmp = wxBitmap(img);

	img = wxImage(paste_xpm);
	img.SetAlpha(paste_alpha, true);
	paste_bmp = wxBitmap(img);

	img = wxImage(open_folder_xpm);
	img.SetAlpha(open_folder_alpha, true);
	open_folder_bmp = wxBitmap(img);

	img = wxImage(new_file_xpm);
	img.SetAlpha(new_file_alpha, true);
	new_file_bmp = wxBitmap(img);

	img = wxImage(new_folder_xpm);
	img.SetAlpha(new_folder_alpha, true);
	new_folder_bmp = wxBitmap(img);

	img = wxImage(save_xpm);
	img.SetAlpha(save_alpha, true);
	save_bmp = wxBitmap(img);

	// Add some more items for, all of them will be in the toolbar
	wxFlatMenuItem *item = new wxFlatMenuItem(fileMenu, MENU_NEW_FILE, wxT("&New File\tCtrl+N"), wxT("New File"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(new_file_bmp, MENU_NEW_FILE, wxT("New File")));

	item = new wxFlatMenuItem(fileMenu, MENU_SAVE, wxT("&Save File\tCtrl+S"), wxT("Save File"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(save_bmp, MENU_SAVE, wxT("Save File")));
	m_mb->AppendToolbarItem(new wxFlatToolbarItem());	// Separator

	item = new wxFlatMenuItem(fileMenu, MENU_OPEN_FILE, wxT("&Open File\tCtrl+O"), wxT("Open File"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(open_folder_bmp, MENU_OPEN_FILE, wxT("Open File")));
	
	item = new wxFlatMenuItem(fileMenu, MENU_NEW_FOLDER, wxT("N&ew Foler\tCtrl+E"), wxT("New Folder"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(new_folder_bmp, MENU_NEW_FOLDER, wxT("New Folder")));
	m_mb->AppendToolbarItem(new wxFlatToolbarItem());	// Separator

	item = new wxFlatMenuItem(fileMenu, MENU_COPY, wxT("&Copy\tCtrl+C"), wxT("Copy"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(copy_bmp, MENU_COPY, wxT("Copy")));
	
	item = new wxFlatMenuItem(fileMenu, MENU_CUT, wxT("Cut\tCtrl+X"), wxT("Cut"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(cut_bmp, MENU_CUT, wxT("Cut")));
	
	item = new wxFlatMenuItem(fileMenu, MENU_PASTE, wxT("Paste\tCtrl+V"), wxT("Paste"), wxITEM_NORMAL);
	fileMenu->Append(item);
	m_mb->AppendToolbarItem(new wxFlatToolbarItem(paste_bmp, MENU_PASTE, wxT("Paste")));

	// Add non-toolbar item
	fileMenu->AppendSeparator();
	item = new wxFlatMenuItem(fileMenu, wxID_EXIT, wxT("E&xit\tAlt+X"), wxT("Exit demo"), wxITEM_NORMAL, NULL, exitImg);
	fileMenu->Append(item);

	// Second menu
	item = new wxFlatMenuItem(styleMenu, MENU_STYLE_XP, wxT("Menu style XP (Default)\tAlt+P"), wxT("Menu style XP (Default)"), wxITEM_RADIO);
	styleMenu->Append(item);
	item->Check( true );

	item = new wxFlatMenuItem(styleMenu, MENU_STYLE_2007, wxT("Menu style 2007\tAlt+V"), wxT("Menu style 2007"), wxITEM_RADIO);
	styleMenu->Append(item);
	
	// Add some radio items
	styleMenu->AppendSeparator();
	// Add sub-menu to main menu
	item = new wxFlatMenuItem(styleMenu, wxID_ANY, wxT("Sub-&menu radio items"), wxEmptyString, wxITEM_NORMAL, subMenu1);
	styleMenu->Append(item);
	
	item = new wxFlatMenuItem(subMenu1, 9997, wxT("Radio Item 1"), wxT("Radio Item 1"), wxITEM_RADIO);
	subMenu1->Append(item);

	item = new wxFlatMenuItem(subMenu1, 9998, wxT("Radio Item 2"), wxT("Radio Item 2"), wxITEM_RADIO);
	subMenu1->Append(item);
	item->Check(true);

	item = new wxFlatMenuItem(subMenu1, 9999, wxT("Radio Item 3"), wxT("Radio Item 3"), wxITEM_RADIO);
	subMenu1->Append(item);

	wxArtManagerST::Get()->SetMBVerticalGradient( true );

	item = new wxFlatMenuItem(editMenu, MENU_REMOVE_MENU, wxT("Remove Menu ..."), wxT("Remove menu"), wxITEM_NORMAL);
	editMenu->Append(item);

	item = new wxFlatMenuItem(editMenu, MENU_DISABLE_MENU_ITEM, wxT("Disable Menu Item ..."), wxT("Disable Menu Item"), wxITEM_NORMAL);
	editMenu->Append(item);

	editMenu->AppendSeparator();

	// Add some dummy entries to the sub menu
	// Add sub-menu to main menu
	item = new wxFlatMenuItem(editMenu, 9001, wxT("Sub-&menu items"), wxEmptyString, wxITEM_NORMAL, subMenu);
	editMenu->Append(item);

	// Create the submenu items and add them 
	item = new wxFlatMenuItem(subMenu, 9002, wxT("&Sub-menu Item 1"), wxEmptyString, wxITEM_NORMAL);
	subMenu->Append(item);

	item = new wxFlatMenuItem(subMenu, 9003, wxT("Su&b-menu Item 2"), wxEmptyString, wxITEM_NORMAL);
	subMenu->Append(item);

	item = new wxFlatMenuItem(subMenu, 9004, wxT("Sub-menu Item 3"), wxEmptyString, wxITEM_NORMAL);
	subMenu->Append(item);

	item = new wxFlatMenuItem(subMenu, 9005, wxT("Sub-menu Item 4"), wxEmptyString, wxITEM_NORMAL);
	subMenu->Append(item);

	// place a main panel on top of the frame
//#if !wxUSE_AUI
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	// Set the main frame sizer
	SetSizer( sizer );
	sizer->Add(m_mb, 0, wxEXPAND);
//#endif

	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);

	// Add menu to the menu bar
	m_mb->Append(fileMenu, wxT("&File"));
	m_mb->Append(styleMenu, wxT("&Style"));
	m_mb->Append(editMenu, wxT("&Edit"));

	// Create a main panel and place some controls on it
	wxPanel *mainPanel = new wxPanel(this, wxID_ANY);
	mainPanel->SetSizer( panelSizer );

//#if !wxUSE_AUI
	sizer->Add(mainPanel, 1, wxEXPAND);
//#endif

	// Add log window
	wxTextCtrl *text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 200),wxTE_MULTILINE | wxTE_READONLY);
	m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(text) );

//#if !wxUSE_AUI
	sizer->Add(text, 0, wxEXPAND);
//#endif 

	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	wxButton *btn = new wxButton(mainPanel, wxID_ANY, wxT("Press me for pop up menu!"));
	hs->Add(btn, 0, wxALL, 5);

	// Connect a button
	btn->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnButtonClicked), NULL, this);

	btn = new wxButton(mainPanel, wxID_ANY, wxT("Press me for a long menu!"));
	hs->Add(btn, 0, wxALL, 5);

	panelSizer->Add(hs, 0, wxALL, 5);
	// Connect a button
	btn->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Frame::OnLongButtonClicked), NULL, this);

#if wxUSE_AUI
	// AUI support
	//m_mgr.AddPane(mainPanel, wxPaneInfo().Name(wxT("main_panel")).
	//	CenterPane());

    //m_mgr.AddPane(text, wxPaneInfo().
    //              Name(wxT("test2")).Caption(wxT("log_window")).
    //              Bottom().Position(1));

		
	//m_mb->PositionAUI( &m_mgr );

	//m_mgr.Update();
#endif
	return true;
}

// TODO:: Add event halders here
void Frame::ConnectEvents()
{
	// Attache menu events to some handlers
	Connect( wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnQuit) );
	Connect( MENU_STYLE_2007, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnStyle) );
	Connect( MENU_STYLE_XP, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnStyle) );
	Connect( MENU_REMOVE_MENU, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnEdit) );
	Connect( MENU_DISABLE_MENU_ITEM, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnEdit) );
	Connect( 20001, 20013, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Frame::OnFlatMenuCmd ));
	Connect( 20001, 20013, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Frame::OnFlatMenuCmdUI ));
	Connect( MENU_NEW_FILE, MENU_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Frame::OnFlatMenuCmd ));
	Connect( 9997, 9999, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Frame::OnFlatMenuCmd ));
}

void Frame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::OnLongButtonClicked(wxCommandEvent & event)
{
	// Demonstrate using the wxFlatMenu without a menu bar
	wxButton *btn = static_cast<wxButton*>( event.GetEventObject() );

	// Create the popup menu
	CreateLongPopupMenu();

	// Postion the menu:
	// The menu should be positioned at the bottom left corner of the button.
	wxSize btnSize = btn->GetSize();

	// btnPt is returned relative to its parent 
	// so, we need to convert it to screen 
	wxPoint btnPt  = btn->GetPosition();
	btnPt = btn->GetParent()->ClientToScreen( btnPt );

	// if we wish the menu to appear under the button, we provide its height 
	m_longPopUpMenu->SetOwnerHeight(btnSize.y);
	m_longPopUpMenu->Popup(	wxPoint(btnPt.x, btnPt.y), this); 
}

void Frame::OnButtonClicked(wxCommandEvent & event)
{
	// Demonstrate using the wxFlatMenu without a menu bar
	wxButton *btn = static_cast<wxButton*>( event.GetEventObject() );

	// Create the popup menu
	CreatePopupMenu();

	// Postion the menu:
	// The menu should be positioned at the bottom left corner of the button.
	wxSize btnSize = btn->GetSize();

	// btnPt is returned relative to its parent 
	// so, we need to convert it to screen 
	wxPoint btnPt  = btn->GetPosition();
	btnPt = btn->GetParent()->ClientToScreen( btnPt );

	// A nice feature with the Popup menu, is the ability to provide an 
	// object that we wish to handle the menu events, in this case we
	// pass 'this'

	// if we wish the menu to appear under the button, we provide its height 
	m_popUpMenu->SetOwnerHeight(btnSize.y);
	m_popUpMenu->Popup(	wxPoint(btnPt.x, btnPt.y), this); 
}

void Frame::CreateLongPopupMenu()
{
	if( !m_longPopUpMenu )
	{
		m_longPopUpMenu = new wxFlatMenu;
		//-----------------------------------------------
		// Flat Menu test
		//-----------------------------------------------
		wxFlatMenuItem *menuItem;
		wxFlatMenu *sub = new wxFlatMenu;

		for(int i=0; i<30; i++)
		{
			if( i == 0 )
			{
				menuItem = new wxFlatMenuItem(m_longPopUpMenu, wxID_ANY, wxString::Format(wxT("Menu Item #%ld"), i), wxEmptyString, wxITEM_NORMAL, sub);
				m_longPopUpMenu->Append(menuItem);

				for(int k=0; k<5; k++)
				{
					menuItem = new wxFlatMenuItem(sub, wxID_ANY, wxString::Format(wxT("Sub Menu Item #%ld"), k));
					sub->Append( menuItem );
				}
			}
			else
			{
				menuItem = new wxFlatMenuItem(m_longPopUpMenu, wxID_ANY, wxString::Format(wxT("Menu Item #%ld"), i));
				m_longPopUpMenu->Append( menuItem );
			}
		}


		
	}
}

void Frame::CreatePopupMenu()
{
	if( !m_popUpMenu )
	{
		m_popUpMenu = new wxFlatMenu;
		//-----------------------------------------------
		// Flat Menu test
		//-----------------------------------------------
		wxFlatMenuItem *menuItem;

		// First we create the sub-menu item
		wxFlatMenu* subMenu = new wxFlatMenu;
		wxFlatMenu* subSubMenu = new wxFlatMenu;

		// Create the menu items
		menuItem = new wxFlatMenuItem(m_popUpMenu, 20001, wxT("First Menu Item"), wxEmptyString, wxITEM_CHECK);
		m_popUpMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(m_popUpMenu, 20002, wxT("Sec&ond Menu Item"), wxEmptyString, wxITEM_CHECK);
		m_popUpMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(m_popUpMenu, wxID_ANY, wxT("Checkable-Disabled Item"), wxEmptyString, wxITEM_CHECK);
		menuItem->Enable(false);
		m_popUpMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(m_popUpMenu, 20003, wxT("Third Menu Item"), wxEmptyString, wxITEM_CHECK);
		m_popUpMenu->Append(menuItem);

		m_popUpMenu->AppendSeparator();

		// Add sub-menu to main menu
		menuItem = new wxFlatMenuItem(m_popUpMenu, 20004, wxT("Sub-&menu item"), wxEmptyString, wxITEM_NORMAL, subMenu);
		m_popUpMenu->Append(menuItem);

		// Create the submenu items and add them 
		menuItem = new wxFlatMenuItem(subMenu, 20005, wxT("&Sub-menu Item 1"), wxEmptyString, wxITEM_NORMAL);
		subMenu->Append(menuItem);
	
		menuItem = new wxFlatMenuItem(subMenu, 20006, wxT("Su&b-menu Item 2"), wxEmptyString, wxITEM_NORMAL);
		subMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(subMenu, 20007, wxT("Sub-menu Item 3"), wxEmptyString, wxITEM_NORMAL);
		subMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(subMenu, 20008, wxT("Sub-menu Item 4"), wxEmptyString, wxITEM_NORMAL);
		subMenu->Append(menuItem);

		// Create the submenu items and add them 
		menuItem = new wxFlatMenuItem(subSubMenu, 20009, wxT("Sub-menu Item 1"), wxEmptyString, wxITEM_NORMAL);
		subSubMenu->Append(menuItem);
	
		menuItem = new wxFlatMenuItem(subSubMenu, 20010, wxT("Sub-menu Item 2"), wxEmptyString, wxITEM_NORMAL);
		subSubMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(subSubMenu, 20011, wxT("Sub-menu Item 3"), wxEmptyString, wxITEM_NORMAL);
		subSubMenu->Append(menuItem);

		menuItem = new wxFlatMenuItem(subSubMenu, 20012, wxT("Sub-menu Item 4"), wxEmptyString, wxITEM_NORMAL);
		subSubMenu->Append(menuItem);

		// Add sub-menu to submenu menu
		menuItem = new wxFlatMenuItem(subMenu, 20013, wxT("Sub-menu item"), wxEmptyString, wxITEM_NORMAL, subSubMenu);
		subMenu->Append(menuItem);
	}
}

// ------------------------------------------
// Event handlers
// ------------------------------------------

void Frame::OnStyle(wxCommandEvent & event )
{
	switch( event.GetId() )
	{
	case MENU_STYLE_2007:
		wxArtManagerST::Get()->SetMenuTheme( Style2007 );
		m_mb->Refresh();
		break;
	case MENU_STYLE_XP:
		wxArtManagerST::Get()->SetMenuTheme( StyleXP );
		m_mb->Refresh();
		break;
	}
}

void Frame::OnFlatMenuCmd(wxCommandEvent &event)
{
	FNB_LOG_MSG(wxT("Received Flat menu command event ID: ") << event.GetId());
}

void Frame::OnFlatMenuCmdUI(wxUpdateUIEvent &event)
{
	FNB_LOG_MSG(wxT("Received Flat menu update UI event ID: ") << event.GetId());
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

void Frame::OnEdit(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	case MENU_REMOVE_MENU:
		{
			wxString idxStr = GetStringFromUser(wxT("Insert menu index to remove:"));
			if( !idxStr.IsEmpty() )
			{
				int idx = wxAtoi(idxStr);
				m_mb->Remove( (size_t)idx );
			}
		}
		break;
	case MENU_DISABLE_MENU_ITEM:
		{
			wxString idxStr = GetStringFromUser(wxT("Insert menu item ID to disabled (10005 - 10011):"));
			if( !idxStr.IsEmpty() )
			{
				int idx = wxAtoi(idxStr);
				wxFlatMenuItem *mi = m_mb->FindMenuItem(idx);
				if(mi)
					mi->Enable(false);
			}
		}
		break;
	default:
		break;
	}
}

