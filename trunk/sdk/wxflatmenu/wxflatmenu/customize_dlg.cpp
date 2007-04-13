#include <wx/listbox.h>
#include <wx/image.h>
#include "imagebook.h"
#include "customize_dlg.h"
#include "flat_menu_bar.h"
#include "fm_resources.h"
#include "art_manager.h"
#include <wx/checklst.h>
#include <wx/arrstr.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/statline.h>

wxFMCustomizeDlg::wxFMCustomizeDlg(wxWindow* parent)
: m_book(NULL)
{
	Create(parent);
	ConnectEvents();
	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	GetSizer()->Layout();
	Centre();
}

wxFMCustomizeDlg::wxFMCustomizeDlg()
: wxDialog()
, m_book(NULL)
{
}

wxFMCustomizeDlg::~wxFMCustomizeDlg()
{
} 

void wxFMCustomizeDlg::Create(wxWindow* parent)
{
	if(  !wxDialog::Create(parent, wxID_ANY, wxT("Customize"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE) )
		return;

	wxBoxSizer *sz = new wxBoxSizer( wxVERTICAL );
	SetSizer( sz );

	// Create the main book and add some pages into it
	long style = wxINB_NO_RESIZE | wxINB_LEFT | wxINB_DRAW_SHADOW | wxINB_BORDER;
	m_book = new wxLabelBook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	sz->Add(m_book, 1, wxEXPAND);

	wxLabelBook* lableBook = (wxLabelBook*)m_book;
	lableBook->SetColour(wxINB_TAB_AREA_BACKGROUND_COLOR, wxArtManagerST::Get()->GetMenuFaceColour());

	wxColour colour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	lableBook->SetColour(wxINB_ACTIVE_TAB_COLOR, colour);

	Initialise();

	wxBoxSizer *hsizer = new wxBoxSizer(wxHORIZONTAL);

	// add a separator between the book & the buttons area
	hsizer->Add(new wxButton(this, wxID_OK, wxT("&Close")), 0, wxEXPAND | wxALIGN_RIGHT);
	sz->Add(new wxStaticLine(this), 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
	sz->Add(hsizer, 0, wxALIGN_RIGHT | wxALL, 5);
}

void wxFMCustomizeDlg::Initialise()
{
	m_book->DeleteAllPages();
	m_book->AddPage(CreateMenusPage(), wxT("Menus"), true);
	m_book->AddPage(CreateOptionsPage(), wxT("Options"), false);
}

void wxFMCustomizeDlg::CloseDialog()
{
	EndModal( wxID_OK );
}

void wxFMCustomizeDlg::ConnectEvents()
{
}

wxWindow* wxFMCustomizeDlg::CreateMenusPage()
{
	wxPanel *menus = new wxPanel(m_book, wxID_ANY, wxDefaultPosition, wxSize(300, 300));
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	menus->SetSizer(sz);

	wxArrayString choices;
	wxFlatMenuBar *mb = static_cast<wxFlatMenuBar*>(GetParent());
	
	// Add all the menu items that are currently visible to the list
	size_t i(0);
	for(; i<mb->m_items.size(); i++)
	{
		wxString lableOnly;
		wxArtManagerST::Get()->GetAccelIndex(mb->m_items[i]->GetTitle(), lableOnly);
		choices.Add( lableOnly );

		// Add the menu to the visible menus map
		m_visibleMenus[ lableOnly ] = mb->m_items[i]->GetMenu();
	}
	
	// Add all hidden menus to the menu bar
	std::map<wxString, wxFlatMenu*>::iterator iter = m_hiddenMenus.begin();
	for(; iter != m_hiddenMenus.end(); iter++) {
		choices.Add( iter->first );
	}

	m_menuListId = wxNewId();
	m_checkListMenus = new wxCheckListBox(menus, m_menuListId, wxDefaultPosition, wxSize(250, 250), choices, wxBORDER_SIMPLE);
	m_checkListMenus->Connect(wxID_ANY, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, wxCommandEventHandler(wxFMCustomizeDlg::OnMenuChecked), NULL, this);

	// check all visible items
	for(size_t j=0; j<i; j++){
		m_checkListMenus->Check((unsigned int)j);
	}

	// Add title panel
	wxFMTitlePanel *title = new wxFMTitlePanel(menus, wxT("Select Menu to Add/Remove:"));
	sz->Add(title, 0, wxEXPAND | wxALL, 2);
	sz->Add(m_checkListMenus, 1, wxEXPAND | wxTOP | wxRIGHT | wxLEFT, 2);
	return menus;
}

wxWindow* wxFMCustomizeDlg::CreateShortcutsPage()
{
	wxPanel *shorcuts = new wxPanel(m_book, wxID_ANY, wxDefaultPosition, wxSize(300, 300));
	return shorcuts;
}

wxWindow* wxFMCustomizeDlg::CreateOptionsPage()
{
	wxPanel *options = new wxPanel(m_book, wxID_ANY, wxDefaultPosition, wxSize(300, 300));

	// Create some options here
	wxBoxSizer *vsizer = new wxBoxSizer(wxVERTICAL);
	options->SetSizer( vsizer );

	//-----------------------------------------------------------
	// options page layout 
	// - Menu Style: Default or 2007	(radio group)
	//
	// - Default Style Settings:		(static box)
	//     + Draw vertical gradient		(check box)
	//     + Draw border				(check box)
	//     + Drop toobar shadow			(check box)
	//
	// - Colour Scheme					(static box)
	//     + Menu bar background colour (combo button)
	//-----------------------------------------------------------

	m_menuStyleID = wxNewId();
	wxString choices[] = { wxT("Default Style"), wxT("Metalic") };
	m_menuStyle = new wxRadioBox(options, m_menuStyleID, wxT("Menu bar style"), 
											 wxDefaultPosition, wxDefaultSize, 
											 2, choices);

	// update the selection
	wxArtManagerST::Get()->GetMenuTheme() == Style2007 ? m_menuStyle->SetSelection(1) : m_menuStyle->SetSelection(0);

	// connect event to the control
	m_menuStyle->Connect(m_menuStyleID, wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler(wxFMCustomizeDlg::OnChangeStyle), NULL, this);
	
	vsizer->Add(m_menuStyle, 0, wxEXPAND | wxALL, 5);

	m_sbStyle = new wxStaticBoxSizer(wxVERTICAL, options, wxT("Default style settings"));

	m_drawVertGradID = wxNewId();
	m_verticalGradient = new wxCheckBox(options, m_drawVertGradID, wxT("Draw vertical gradient"));
	m_verticalGradient->Connect(m_drawVertGradID, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFMCustomizeDlg::OnChangeStyle), NULL, this);
	m_sbStyle->Add(m_verticalGradient, 0, wxEXPAND | wxALL, 3);
	m_verticalGradient->SetValue(wxArtManagerST::Get()->GetMBVerticalGardient());

	m_drawBorderID = wxNewId();
	m_drawBorder = new wxCheckBox(options, m_drawBorderID, wxT("Draw border around menu bar"));
	m_drawBorder->Connect(m_drawBorderID, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFMCustomizeDlg::OnChangeStyle), NULL, this);
	m_sbStyle->Add(m_drawBorder, 0, wxEXPAND | wxALL, 3);
	m_drawBorder->SetValue(wxArtManagerST::Get()->GetMenuBarBorder());

	m_shadowUnderTBID = wxNewId();
	m_shadowUnderTB = new wxCheckBox(options, m_shadowUnderTBID, wxT("Toolbar float over menu bar"));
	m_shadowUnderTB->Connect(m_shadowUnderTBID, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(wxFMCustomizeDlg::OnChangeStyle), NULL, this);
	m_sbStyle->Add(m_shadowUnderTB, 0, wxEXPAND | wxALL, 3);
	m_shadowUnderTB->SetValue(wxArtManagerST::Get()->GetRaiseToolbar());

	vsizer->Add(m_sbStyle, 0, wxEXPAND | wxALL, 5);

	// Misc 
	wxStaticBoxSizer *sb = new wxStaticBoxSizer(wxVERTICAL, options, wxT("Colour Scheme"));
	m_colourID = wxNewId();

	wxString colorChoices[] = { wxT("Default"), wxT("Generic"), wxT("Dark"), wxT("Dark Olive Green") };

	m_colour = new wxComboBox(options, m_colourID, wxArtManagerST::Get()->GetMenuBarColourScheme(), wxDefaultPosition, wxDefaultSize, 4, colorChoices, wxCB_DROPDOWN | wxCB_READONLY);
	sb->Add(m_colour, 0, wxEXPAND);
	vsizer->Add(sb, 0, wxEXPAND | wxALL, 5);
	m_colour->Connect(m_colourID, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(wxFMCustomizeDlg::OnChangeStyle), NULL, this);
	

	// update the dialog by sending all possible events to us
	wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_menuStyleID);
	event.SetEventObject(this);
	event.SetInt(m_menuStyle->GetSelection());
	m_menuStyle->ProcessEvent(event);

	event.SetEventType(wxEVT_COMMAND_CHECKBOX_CLICKED);
	event.SetId(m_drawVertGradID);
	event.SetInt(static_cast<int>(wxArtManagerST::Get()->GetMBVerticalGardient()));
	m_verticalGradient->ProcessEvent(event);

	event.SetEventType(wxEVT_COMMAND_CHECKBOX_CLICKED);
	event.SetId(m_shadowUnderTBID);
	event.SetInt(static_cast<int>(wxArtManagerST::Get()->GetRaiseToolbar()));
	m_shadowUnderTB->ProcessEvent(event);

	event.SetEventType(wxEVT_COMMAND_CHECKBOX_CLICKED);
	event.SetId(m_drawBorderID);
	event.SetInt(static_cast<int>(wxArtManagerST::Get()->GetMenuBarBorder()));
	m_drawBorder->ProcessEvent(event);

	event.SetEventType(wxEVT_COMMAND_COMBOBOX_SELECTED);
	event.SetId(m_colourID);
	m_colour->ProcessEvent(event);

	return options;
}

void wxFMCustomizeDlg::OnMenuChecked(wxCommandEvent &event)
{
	int id = event.GetInt();
	bool checked = m_checkListMenus->IsChecked( id );
	wxString menuName = m_checkListMenus->GetString( id );
	wxFlatMenu *menu = NULL;
	wxFlatMenuBar *mb = static_cast<wxFlatMenuBar*>(GetParent());

	if( checked ) 
	{
		// remove the item from the hidden map
		std::map<wxString, wxFlatMenu*>::iterator iter = m_hiddenMenus.find(menuName);
		if( iter != m_hiddenMenus.end() ){
			menu = iter->second;
			m_hiddenMenus.erase(iter);
		}

		// add it to the visible map
		if( menu ) {
			m_visibleMenus[menuName] = menu;
		}
		// update the menubar
		mb->Append(menu, menu->m_menuBarFullTitle);
		mb->Refresh();
	}	
	else
	{
		// remove the item from the visible items
		std::map<wxString, wxFlatMenu*>::iterator iter = m_visibleMenus.find(menuName);
		if( iter != m_visibleMenus.end() ){
			menu = iter->second;
			m_visibleMenus.erase(iter);
		}

		// add it to the hidden map
		if( menu ) {
			m_hiddenMenus[menuName] = menu;
		}
		
		// update the menubar
		int pos = mb->FindMenu(menuName);
		if( pos != wxNOT_FOUND ) 
		{
			mb->Remove((size_t)pos);
		}
		mb->Refresh();
	}
}

void wxFMCustomizeDlg::OnChangeStyle(wxCommandEvent &event)
{
	wxFlatMenuBar *mb = static_cast<wxFlatMenuBar*>(GetParent());
	
	if( event.GetId() == m_menuStyleID )
	{
		if( event.GetSelection() == 0) 
		{
			// Default style
			wxArtManagerST::Get()->SetMenuTheme( StyleXP );
			m_drawBorder->Enable();
			m_verticalGradient->Enable();
			mb->Refresh();
		}
		else
		{
			wxArtManagerST::Get()->SetMenuTheme( Style2007 );
			m_drawBorder->Enable(false);
			m_verticalGradient->Enable(false);			
			mb->Refresh();
		}
		return;
	}

	if( event.GetId() == m_drawBorderID )
	{
		wxArtManagerST::Get()->DrawMenuBarBorder( event.IsChecked() );
		mb->Refresh();
		return;
	}

	if( event.GetId() == m_drawVertGradID )
	{
		wxArtManagerST::Get()->SetMBVerticalGradient( event.IsChecked() );
		mb->Refresh();
		return;
	}

	if( event.GetId() == m_colourID )
	{
		// select new colour scheme
		wxString selection(wxT("Default"));
		int sel = m_colour->GetSelection();
		if(sel != wxNOT_FOUND)
			selection = m_colour->GetStringSelection();

		wxArtManagerST::Get()->SetMenuBarColour( selection );
		mb->Refresh();
		return;
	}

	if( event.GetId() == m_shadowUnderTBID )
	{
		wxArtManagerST::Get()->SetRaiseToolbar( event.IsChecked() );
		mb->Refresh();
		return;
	}
}

//---------------------------------------------------------
// title panel
//---------------------------------------------------------
wxFMTitlePanel::wxFMTitlePanel(wxWindow* parent, const wxString &title)
: wxPanel(parent)
, m_title(title)
{
	ConnectEvents();

	// Set the panel size
	wxMemoryDcEx dc;
	dc.SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ));
	static int hh(-1);
	static int ww(-1);
	static wxString text(wxT("Tp"));

	if( hh == -1 ) 
	{
		dc.GetTextExtent(text, &ww, &hh);
	}

	// Set minimum panel size
	if( ww < 250 )
		ww = 250;

	SetSize(wxSize(ww, hh + 10));
}

wxFMTitlePanel::~wxFMTitlePanel()
{
}

void wxFMTitlePanel::ConnectEvents()
{
	Connect(wxID_ANY, wxEVT_PAINT, wxPaintEventHandler(wxFMTitlePanel::OnPaint), NULL, this);
	Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(wxFMTitlePanel::OnEraseBg), NULL, this);
}

void wxFMTitlePanel::OnEraseBg(wxEraseEvent &event)
{
	wxUnusedVar(event);
}

void wxFMTitlePanel::OnPaint(wxPaintEvent &event)
{
	wxUnusedVar(event);
	wxBufferedPaintDC dc(this);

	// Draw the background
	wxColour colour1 = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
	wxColour colour2 = wxArtManagerST::Get()->LightColour(colour1, 70);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, GetClientRect(), colour1, colour2, false);

	// Draw the text
	wxFont font(wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ));
	font.SetWeight( wxBOLD );
	dc.SetFont(font);
	dc.SetTextForeground( *wxBLACK );
	dc.DrawText(m_title, 5, 5);
}

