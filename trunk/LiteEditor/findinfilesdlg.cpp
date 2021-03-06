#include "findinfilesdlg.h"
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "search_thread.h"
#include <wx/collpane.h>
#include <wx/statline.h>
#include <wx/combobox.h>
#include "dirpicker.h"
#include "wx/sizer.h"
#include "macros.h"

DEFINE_EVENT_TYPE(wxEVT_FIF_FIND)
DEFINE_EVENT_TYPE(wxEVT_FIF_STOP)
DEFINE_EVENT_TYPE(wxEVT_FIF_CLOSE)

BEGIN_EVENT_TABLE(FindInFilesDialog, wxDialog)
EVT_CLOSE(FindInFilesDialog::OnClose)
EVT_CHAR_HOOK(FindInFilesDialog::OnCharEvent)
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
	GetSizer()->SetMinSize(wxSize(600, 300));
	GetSizer()->SetSizeHints(this);
	Centre();
	m_findString->SetFocus();
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

	m_findString = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1));
	mainSizer->Add(m_findString, 0, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Look In:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	mainSizer->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	m_dirPicker = new DirPicker(this, wxID_ANY, wxT("Browse"), wxEmptyString, wxT("Select a folder:"), wxDefaultPosition, wxDefaultSize, wxDP_USE_COMBOBOX);
	mainSizer->Add(m_dirPicker, 0, wxEXPAND | wxALL, 5);
	wxArrayString choices;
	choices.Add(SEARCH_IN_WORKSPACE);
	choices.Add(SEARCH_IN_PROJECT);
	m_dirPicker->SetValues(choices, 0);

	// Add the options
	wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Options:"));
	mainSizer->Add(sz, 1, wxEXPAND | wxALL, 5);

	m_matchCase = new wxCheckBox(this, wxID_ANY, wxT("&Match case"));
	sz->Add(m_matchCase, 1, wxALL | wxEXPAND, 5 );

	m_matchWholeWord = new wxCheckBox(this, wxID_ANY, wxT("Match &whole word"));
	sz->Add(m_matchWholeWord, 1, wxALL | wxEXPAND, 5 );

	m_regualrExpression = new wxCheckBox(this, wxID_ANY, wxT("Regular &expression"));
	sz->Add(m_regualrExpression, 1, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, wxT("Look at these file &types:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	sz->Add(itemStaticText, 0, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5 );

	wxString options [] = {
			wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"), 
			wxT("*.*") };

	m_fileTypes = new wxComboBox(this, 
								wxID_ANY, 
								wxT("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc"), 
								wxDefaultPosition, wxDefaultSize,
								2, options, wxCB_DROPDOWN);
	sz->Add(m_fileTypes, 0, wxEXPAND | wxALL, 5);

	// Add the buttons
	m_find = new wxButton(this, wxID_ANY, wxT("&Find"));
	btnSizer->Add(m_find, 1, wxALL | wxEXPAND, 5 ); 

	m_stop = new wxButton(this, wxID_ANY, wxT("&Stop Search"));
	btnSizer->Add(m_stop, 1, wxALL | wxEXPAND, 5 ); 

	m_cancel = new wxButton(this, wxID_ANY, wxT("Close"));
	btnSizer->Add(m_cancel, 1, wxALL | wxEXPAND, 5 ); 

	mainSizer->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND );
	mainSizer->Add(btnSizer, 0, wxEXPAND|wxALL, 5);
	
	SetData(m_data);
	m_findString->SetSelection(-1, -1); // select all
	m_findString->SetFocus();
}

void FindInFilesDialog::SetData(FindReplaceData &data)
{
	//sets the previous values
	m_findString->Clear();
	m_findString->Append(data.GetFindStringArr());
	m_findString->SetValue(data.GetFindString());
	m_matchCase->SetValue(data.GetFlags() & wxFRD_MATCHCASE ? true : false);
	m_matchWholeWord->SetValue(data.GetFlags() & wxFRD_MATCHWHOLEWORD ? true : false);
	m_regualrExpression->SetValue(data.GetFlags() & wxFRD_REGULAREXPRESSION ? true : false);
}

void FindInFilesDialog::DoSearch()
{
	SearchData data;
	data.SetFindString(m_data.GetFindString());
	data.SetMatchCase( (m_data.GetFlags() & wxFRD_MATCHCASE) != 0);
	data.SetMatchWholeWord((m_data.GetFlags() & wxFRD_MATCHWHOLEWORD) != 0);
	data.SetRegularExpression((m_data.GetFlags() & wxFRD_REGULAREXPRESSION) != 0);
	data.SetRootDir(m_dirPicker->GetPath());
	data.SetExtensions(m_fileTypes->GetValue());

	// Convert file types to array
	SearchThreadST::Get()->PerformSearch(data);
}

void FindInFilesDialog::OnClick(wxCommandEvent &event)
{
	wxObject *btnClicked = event.GetEventObject();
	size_t flags = m_data.GetFlags();
	m_data.SetFindString( m_findString->GetValue() );

	if(btnClicked == m_stop){
		SearchThreadST::Get()->StopSearch();
	} else if(btnClicked == m_find){
		DoSearch();		
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

void FindInFilesDialog::OnCharEvent(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_ESCAPE){
		Hide();
		return;
	} 
	else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER){
		DoSearch();
		return;
	}
	event.Skip();
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

	SetData(m_data);
	m_findString->SetSelection(-1, -1); // select all
	return wxDialog::Show();
}
