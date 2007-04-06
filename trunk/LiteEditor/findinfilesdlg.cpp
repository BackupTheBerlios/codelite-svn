#include "findinfilesdlg.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "search_thread.h"
#include <wx/filepicker.h>
#include <wx/collpane.h>
#include <wx/statline.h>
#include <wx/combobox.h>

DEFINE_EVENT_TYPE(wxEVT_FIF_FIND)
DEFINE_EVENT_TYPE(wxEVT_FIF_STOP)
DEFINE_EVENT_TYPE(wxEVT_FIF_CLOSE)

BEGIN_EVENT_TABLE(FindInFilesDialog, wxDialog)
EVT_CLOSE(FindInFilesDialog::OnClose)
END_EVENT_TABLE()

FindInFilesDialog::FindInFilesDialog() 
: wxDialog()
, m_owner(NULL)
{
}

FindInFilesDialog::FindInFilesDialog(wxWindow* parent, 
									 const FindReplaceData& data, 
									 wxWindowID id, 
									 const wxString& caption, 
									 const wxPoint& pos, 
									 const wxSize& size, 
									 long style)
{
	Create(parent, data, id, caption, pos, size, style);
}

bool FindInFilesDialog::Create(wxWindow* parent, 
							   const FindReplaceData& data, 
							   wxWindowID id, 
							   const wxString& caption, 
							   const wxPoint& pos, 
							   const wxSize& size, 
							   long style)
{
	if( !wxDialog::Create(parent, id, caption, pos, size, style) )
		return false;

	m_data = data;
	m_owner = NULL;

	CreateGUIControls();
	ConnectEvents();

	GetSizer()->Fit(this);
	GetSizer()->SetSizeHints(this);
	return true;
}

FindInFilesDialog::~FindInFilesDialog()
{
}

void FindInFilesDialog::CreateGUIControls()
{
	wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Find What:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mainSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_findString = new wxTextCtrl( this, wxID_ANY, m_data.GetFindString(), wxDefaultPosition, wxSize(200, -1));
	mainSizer->Add(m_findString, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Look In:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mainSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_dirPicker = new wxDirPickerCtrl(this, wxID_ANY, wxEmptyString, wxT("Look In"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL  | wxDIRP_DIR_MUST_EXIST); 
	mainSizer->Add(m_dirPicker, 0, wxEXPAND | wxALL, 5);

	// Add the options
	wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Options:"));
	mainSizer->Add(sz, 1, wxEXPAND | wxALL, 5);

	m_matchCase = new wxCheckBox(this, wxID_ANY, wxT("&Match case"));
	m_matchCase->SetValue(m_data.GetFlags() & wxFRD_MATCHCASE ? true : false);
	sz->Add(m_matchCase, 1, wxALL | wxEXPAND, 5 );

	m_matchWholeWord = new wxCheckBox(this, wxID_ANY, wxT("Match &whole word"));
	m_matchWholeWord->SetValue(m_data.GetFlags() & wxFRD_MATCHWHOLEWORD ? true : false);
	sz->Add(m_matchWholeWord, 1, wxALL | wxEXPAND, 5 );

	m_regualrExpression = new wxCheckBox(this, wxID_ANY, wxT("Regular &expression"));
	m_regualrExpression->SetValue(m_data.GetFlags() & wxFRD_REGULAREXPRESSION ? true : false);
	sz->Add(m_regualrExpression, 1, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, wxT("Look at these file &types:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	sz->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	wxString options [] = {
			wxT("*.c;*.cpp;*.cxx;*.cc;*.tli;*.tlh;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc;*.resx;*.idl;*.asm;*.inc"), 
			wxT("*.*") };

	m_fileTypes = new wxComboBox(this, 
								wxID_ANY, 
								wxT("*.c;*.cpp;*.cxx;*.cc;*.tli;*.tlh;*.h;*.hpp;*.hxx;*.hh;*.inl;*.rc;*.resx;*.idl;*.asm;*.inc"), 
								wxDefaultPosition, wxDefaultSize,
								2, options, wxCB_DROPDOWN);
	sz->Add(m_fileTypes, 0, wxEXPAND | wxALL, 5);

	// Add the buttons
	m_find = new wxButton(this, wxID_ANY, wxT("&Find"));
	btnSizer->Add(m_find, 1, wxALL | wxEXPAND, 5 ); 

	m_stop = new wxButton(this, wxID_ANY, wxT("&Stop Search"));
	btnSizer->Add(m_stop, 1, wxALL | wxEXPAND, 5 ); 

	m_cancel = new wxButton(this, wxID_ANY, wxT("Cancel"));
	btnSizer->Add(m_cancel, 1, wxALL | wxEXPAND, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxEXPAND|wxALL, 5);
}

void FindInFilesDialog::OnClick(wxCommandEvent &event)
{
	wxObject *btnClicked = event.GetEventObject();
	size_t flags = m_data.GetFlags();
	m_data.SetFindString( m_findString->GetValue() );

	if(btnClicked == m_stop){
		SearchThreadST::Get()->StopSearch();
	} else if(btnClicked == m_find){

		SearchData data;
		data.SetFindString(m_data.GetFindString());
		data.SetMatchCase( (m_data.GetFlags() & wxFRD_MATCHCASE) != 0);
		data.SetMatchWholeWord((m_data.GetFlags() & wxFRD_MATCHWHOLEWORD) != 0);
		data.SetRegularExpression((m_data.GetFlags() & wxFRD_REGULAREXPRESSION) != 0);
		data.SetRootDir(m_dirPicker->GetPath());
		data.SetExtensions(m_fileTypes->GetValue());

		// Convert file types to array
		SearchThreadST::Get()->PerformSearch(data);

	} else if(btnClicked == m_cancel){
		// Hide the dialog
		Hide();
	} else if(btnClicked == m_matchCase){
		if(m_matchCase->IsChecked()) {
			flags |= wxFRD_MATCHCASE;
		} else {
			flags &= ~(wxFRD_MATCHCASE);
		}
	} else if(btnClicked == m_matchWholeWord){
		if(m_matchWholeWord->IsChecked()) {
			flags |= wxFRD_MATCHWHOLEWORD;
		} else {
			flags &= ~(wxFRD_MATCHWHOLEWORD);
		}
	} else if(btnClicked == m_regualrExpression){
		if(m_regualrExpression->IsChecked()) {
			flags |= wxFRD_REGULAREXPRESSION;
		} else {
			flags &= ~(wxFRD_REGULAREXPRESSION);
		}
	}

	// Set the updated flags
	m_data.SetFlags(flags);
}

void FindInFilesDialog::OnClose(wxCloseEvent &event)
{
	wxUnusedVar(event);

	// Fire a close event
	SendEvent(wxEVT_FRD_CLOSE);
	Hide();
}

void FindInFilesDialog::ConnectEvents()
{
	// Connect buttons
	m_find->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_cancel->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_stop->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);

	// connect options
	m_matchCase->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_matchWholeWord->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
	m_regualrExpression->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindInFilesDialog::OnClick), NULL, this);
}

void FindInFilesDialog::SendEvent(wxEventType type)
{
	wxCommandEvent event(type, GetId());
	event.SetEventObject(this);

	if( GetEventOwner() == NULL )
		GetEventHandler()->ProcessEvent( event );
	else
		// If an event owner was provided, pass it the event
		GetEventOwner()->ProcessEvent( event );
}

bool FindInFilesDialog::Show()
{
	if( IsShown() )
		return true;

	return wxDialog::Show();
}
