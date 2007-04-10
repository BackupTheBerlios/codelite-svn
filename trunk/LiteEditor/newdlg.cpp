#include "newdlg.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/listctrl.h>
#include "project.h"
#include <map>
#include "manager.h"

DEFINE_EVENT_TYPE(wxEVT_NEW_DLG_CREATE)

NewDlg::NewDlg() 
: wxDialog()
, m_selection(NEW_DLG_WORKSPACE)
{
}

NewDlg::NewDlg(wxWindow* parent, 
			   int selection, 
			   wxWindowID id, 
			   const wxString& caption, 
			   const wxPoint& pos, 
			   const wxSize& size, 
			   long style)
{
	Create(parent, selection, id, caption, pos, size, style);
}

bool NewDlg::Create(wxWindow* parent, 
					 int selection, 
					 wxWindowID id, 
					 const wxString& caption, 
					 const wxPoint& pos, 
					 const wxSize& size, 
					 long style)
{
	if( !wxDialog::Create(parent, id, caption, pos, size, style) )
		return false;

	m_selection = selection;

	CreateGUIControls();
	ConnectEvents();

	GetSizer()->Fit(this);
	GetSizer()->SetMinSize(500, 300);
	GetSizer()->SetSizeHints(this);
	return true;
}

NewDlg::~NewDlg()
{
}

int NewDlg::GetSelection() const 
{
	return m_selection;
}

void NewDlg::CreateGUIControls()
{
	wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(500, -1), 
								wxFNB_NO_NAV_BUTTONS | wxFNB_NO_X_BUTTON | wxFNB_NODRAG);

	m_book->AddPage(CreateProjectPage(), wxT("Project"), m_selection == NEW_DLG_PROJECT);
	m_book->AddPage(CreateWorkspacePage(), wxT("Workspace"), m_selection == NEW_DLG_WORKSPACE);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 5);
	
	// Add the buttons
	m_create = new wxButton(this, wxID_OK, wxT("&Create"));
	btnSizer->Add(m_create, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	m_cancel = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	btnSizer->Add(m_cancel, 0, wxALL | wxEXPAND | wxALIGN_RIGHT, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxALL | wxALIGN_RIGHT, 5);

	if( !ManagerST::Get()->IsWorkspaceOpen() ){
		m_book->Enable(NEW_DLG_PROJECT, false);
	}
}

wxWindow *NewDlg::CreateWorkspacePage()
{
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxPanel *panel = new wxPanel(m_book);
	panel->SetSizer(panelSizer);
	panel->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT) );

	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Workspace Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_name = new wxTextCtrl( panel, wxID_ANY, wxT(""));
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
	return panel;
}

wxWindow *NewDlg::CreateProjectPage()
{
	wxBoxSizer *panelSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hSizer = new wxBoxSizer(wxHORIZONTAL);
	wxPanel *panel = new wxPanel(m_book);
	panel->SetSizer(hSizer);
	panel->SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT) );

	wxStaticText* itemStaticText;

	m_projTypes = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_SMALL_ICON | wxLC_SINGLE_SEL); 
	wxListItem item;

	item.SetText(Project::STATIC_LIBRARY);
	m_projTypes->InsertItem(item);

	item.SetText(Project::DYMANIC_LIBRARY);
	m_projTypes->InsertItem(item);

	item.SetText(Project::EXECUTABLE);
	item.SetState(wxLIST_STATE_SELECTED);
	m_projTypes->InsertItem(item);
	m_projectData.m_type = Project::EXECUTABLE;
	
	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Project Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_projName = new wxTextCtrl( panel, wxID_ANY, wxT(""));
	panelSizer->Add(m_projName, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( panel, wxID_STATIC, wxT("Project Path:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	panelSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_projPathPicker = new wxDirPickerCtrl(panel, wxID_ANY, wxEmptyString, wxT("Choose directory:"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL); 
	panelSizer->Add(m_projPathPicker, 0, wxEXPAND | wxALL, 5);
	
	hSizer->Add(m_projTypes, 1, wxALL | wxEXPAND, 5);
	hSizer->Add(panelSizer, 2, wxALL | wxEXPAND, 5);
	return panel;
}

void NewDlg::OnClick(wxCommandEvent & WXUNUSED(event))
{
	// pass data from controls to the m_data
	if( m_book->GetSelection() == NEW_DLG_WORKSPACE ){
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

		wxCommandEvent event(wxEVT_NEW_DLG_CREATE, GetId());
		event.SetEventObject(this);
		::wxPostEvent(GetParent()->GetEventHandler(), event);
	} else if( m_book->GetSelection() == NEW_DLG_PROJECT ){
		m_projectData.m_name = m_projName->GetValue();
		m_projectData.m_path = m_projPathPicker->GetPath();
		
		if( m_projectData.m_name.Trim().IsEmpty() ){
			wxMessageBox(wxT("Invalid project name"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}

		if( !wxDirExists(m_projectData.m_path) ){
			wxMessageBox(wxT("Invalid path"), wxT("Error"), wxOK | wxICON_HAND);
			return;
		}
		wxCommandEvent event(wxEVT_NEW_DLG_CREATE, GetId());
		event.SetEventObject(this);
		::wxPostEvent(GetParent()->GetEventHandler(), event);
	}
	EndModal(wxID_OK);
}

void NewDlg::OnListItemSelected(wxListEvent &event)
{
	m_projectData.m_type = event.GetText();
}

void NewDlg::ConnectEvents()
{
	// Connect buttons
	m_create->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewDlg::OnClick), NULL, this);
	m_projTypes->Connect(wxID_ANY, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewDlg::OnListItemSelected), NULL, this);
}
	