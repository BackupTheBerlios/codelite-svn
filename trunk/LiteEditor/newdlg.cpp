#include "newdlg.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>

DEFINE_EVENT_TYPE(wxEVT_CREATE_WORKSPACE)

NewDlg::NewDlg() 
: wxDialog()
, m_selection(NEW_DLG_WORKSPACE)
{
}

NewDlg::NewDlg(wxWindow* parent, 
			   int type, 
			   wxWindowID id, 
			   const wxString& caption, 
			   const wxPoint& pos, 
			   const wxSize& size, 
			   long style)
{
	Create(parent, type, id, caption, pos, size, style);
}

bool NewDlg::Create(wxWindow* parent, 
					 int type, 
					 wxWindowID id, 
					 const wxString& caption, 
					 const wxPoint& pos, 
					 const wxSize& size, 
					 long style)
{
	if( !wxDialog::Create(parent, id, caption, pos, size, style) )
		return false;

	m_selection = type;

	CreateGUIControls();
	ConnectEvents();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	return true;
}

NewDlg::~NewDlg()
{
}

void NewDlg::CreateGUIControls()
{
	wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxPanel *panel = new wxPanel(this);

	panel->SetSizer(panelSizer);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	
	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Workspace Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_name = new wxTextCtrl( panel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(400, -1));
	panelSizer->Add(m_name, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Workspace Path:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_pathPicker = new wxDirPickerCtrl(panel, wxID_ANY, wxEmptyString, wxT("Choose directory:"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL); 
	panelSizer->Add(m_pathPicker, 0, wxEXPAND | wxALL, 5);

	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("External Tags Database:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	const wxString WildCard(wxT("Code Lite Tags file (*.tags)|*.tags|")
							wxT("All Files (*.*)|*.*"));

	m_tagsPicker = new wxFilePickerCtrl(panel, wxID_ANY, wxEmptyString, wxT("Choose directory:"), WildCard, wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL  | wxFLP_OPEN | wxFLP_FILE_MUST_EXIST); 
	panelSizer->Add(m_tagsPicker, 0, wxEXPAND | wxALL, 5);
	mainSizer->Add(panel, 1, wxEXPAND | wxALL, 5);

	// Add the buttons
	m_create = new wxButton(this, wxID_OK, wxT("&Create"));
	btnSizer->Add(m_create, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	m_cancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	btnSizer->Add(m_cancel, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxALL | wxALIGN_RIGHT, 5);
}

void NewDlg::OnClick(wxCommandEvent & WXUNUSED(event))
{
	// pass data from controls to the m_data
	if( m_selection == NEW_DLG_WORKSPACE ){
		m_workspaceData.m_path = m_pathPicker->GetPath();
		m_workspaceData.m_externalTagsDB = m_tagsPicker->GetPath();
		m_workspaceData.m_name = m_name->GetValue();

		if( m_workspaceData.m_name.Trim().IsEmpty() ){
			wxMessageBox(wxT("Invalid workspace name"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}

		if( !wxDirExists(m_workspaceData.m_path) ){
			wxMessageBox(wxT("Invalid path"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}
	}

	

	EndModal(wxID_OK);
}

void NewDlg::ConnectEvents()
{
	// Connect buttons
	m_create->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDlg::OnClick), NULL, this);
}
	