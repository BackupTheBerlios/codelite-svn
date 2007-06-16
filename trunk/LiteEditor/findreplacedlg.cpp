#include "findreplacedlg.h"
#include <wx/gbsizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include "macros.h"

DEFINE_EVENT_TYPE(wxEVT_FRD_FIND_NEXT)
DEFINE_EVENT_TYPE(wxEVT_FRD_CLOSE)
DEFINE_EVENT_TYPE(wxEVT_FRD_REPLACE)
DEFINE_EVENT_TYPE(wxEVT_FRD_REPLACEALL)
DEFINE_EVENT_TYPE(wxEVT_FRD_BOOKMARKALL)

BEGIN_EVENT_TABLE(FindReplaceDialog, wxDialog)
EVT_CLOSE(FindReplaceDialog::OnClose)
END_EVENT_TABLE()

FindReplaceDialog::FindReplaceDialog() 
: wxDialog()
, m_owner(NULL)
{
}

FindReplaceDialog::FindReplaceDialog(wxWindow* parent, 
									 const FindReplaceData& data, 
									 wxWindowID id, 
									 const wxString& caption, 
									 const wxPoint& pos, 
									 const wxSize& size, 
									 long style)
{
	Create(parent, data, id, caption, pos, size, style | wxWANTS_CHARS);
}

bool FindReplaceDialog::Create(wxWindow* parent, 
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

FindReplaceDialog::~FindReplaceDialog()
{
}

void FindReplaceDialog::CreateGUIControls()
{
	wxBoxSizer *hMainSzier = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *btnSizer = new wxBoxSizer(wxVERTICAL);
	wxGridBagSizer *gbSizer = new wxGridBagSizer();
	SetSizer(hMainSzier);
	
	hMainSzier->Add(gbSizer, 3, wxEXPAND | wxALL, 5);
	hMainSzier->Add(btnSizer, 1, wxALL, 5);

	wxStaticText* itemStaticText;
	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Find What:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	gbSizer->Add(itemStaticText, wxGBPosition(0, 0), wxDefaultSpan, wxALL | wxEXPAND, 5 );

	m_findString = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);
	gbSizer->Add(m_findString, wxGBPosition(0, 1), wxDefaultSpan, wxALL | wxEXPAND, 5 );

	itemStaticText = new wxStaticText( this, wxID_STATIC, _("Replace With:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	gbSizer->Add(itemStaticText, wxGBPosition(1, 0), wxDefaultSpan, wxALL | wxEXPAND, 5 );

	m_replaceString = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);
	gbSizer->Add(m_replaceString, wxGBPosition(1, 1), wxDefaultSpan, wxALL | wxEXPAND, 5 );

	wxStaticBoxSizer *sz = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Options"));
	gbSizer->Add(sz, wxGBPosition(2, 0), wxGBSpan(1, 2),  wxALL | wxEXPAND, 5 );

	m_replacementsMsg = new wxStaticText(this, wxID_ANY, wxEmptyString);
	gbSizer->Add(m_replacementsMsg, wxGBPosition(3, 0), wxGBSpan(1, 2), wxALL | wxEXPAND, 5); 
	SetReplacementsMessage(wxT("Replacements: 0"));

	m_matchCase = new wxCheckBox(this, wxID_ANY, wxT("&Match case"));
	sz->Add(m_matchCase, 1, wxALL | wxEXPAND, 5 );

	m_matchWholeWord = new wxCheckBox(this, wxID_ANY, wxT("Match &whole word"));
	sz->Add(m_matchWholeWord, 1, wxALL | wxEXPAND, 5 );

	m_regualrExpression = new wxCheckBox(this, wxID_ANY, wxT("Regular &expression"));
	sz->Add(m_regualrExpression, 1, wxALL | wxEXPAND, 5 );

	m_wrapsearch = new wxCheckBox(this, wxID_ANY, wxT("Wrap around"));
	sz->Add(m_wrapsearch, 1, wxALL | wxEXPAND, 5 );

	m_searchUp = new wxCheckBox(this, wxID_ANY, wxT("Search &up"));
	sz->Add(m_searchUp, 1, wxALL | wxEXPAND, 5 ); 
	
	// Add the buttons
	
	m_find = new wxButton(this, wxID_ANY, wxT("&Find Next"));
	btnSizer->Add(m_find, 1, wxALL | wxEXPAND, 5 ); 

	m_replace = new wxButton(this, wxID_ANY, wxT("&Replace"));
	btnSizer->Add(m_replace, 1, wxALL | wxEXPAND, 5 ); 

	m_replaceAll = new wxButton(this, wxID_ANY, wxT("Replace &All"));
	btnSizer->Add(m_replaceAll, 1, wxALL | wxEXPAND, 5 ); 

	m_markAll = new wxButton(this, wxID_ANY, wxT("&Bookmark All"));
	btnSizer->Add(m_markAll, 1, wxALL | wxEXPAND, 5 ); 

	m_cancel = new wxButton(this, wxID_ANY, wxT("Cancel"));
	btnSizer->Add(m_cancel, 1, wxALL | wxEXPAND, 5 ); 

	//set values
	SetFindReplaceData(m_data);
}

void FindReplaceDialog::SetFindReplaceData(const FindReplaceData &data)
{
	m_findString->SetValue(data.GetFindString());
	m_replaceString->SetValue(data.GetReplaceString());
	m_matchCase->SetValue(data.GetFlags() & wxFRD_MATCHCASE ? true : false);
	m_matchWholeWord->SetValue(data.GetFlags() & wxFRD_MATCHWHOLEWORD ? true : false);
	m_regualrExpression->SetValue(data.GetFlags() & wxFRD_REGULAREXPRESSION ? true : false);
	m_wrapsearch->SetValue(data.GetFlags() & wxFRD_WRAPSEARCH ? true : false);
	m_searchUp->SetValue(data.GetFlags() & wxFRD_SEARCHUP ? true : false);
}

void FindReplaceDialog::OnClick(wxCommandEvent &event)
{
	wxObject *btnClicked = event.GetEventObject();
	size_t flags = m_data.GetFlags();
	m_data.SetFindString( m_findString->GetValue() );
	m_data.SetReplaceString( m_replaceString->GetValue() );

	if(btnClicked == m_find){
		SendEvent(wxEVT_FRD_FIND_NEXT);
	} else if(btnClicked == m_replace){
		SendEvent(wxEVT_FRD_REPLACE);
	} else if(btnClicked == m_replaceAll){
		SendEvent(wxEVT_FRD_REPLACEALL);
	} else if(btnClicked == m_markAll){
		SendEvent(wxEVT_FRD_BOOKMARKALL);
	} else if(btnClicked == m_cancel){
		// Fire a close event
		SendEvent(wxEVT_FRD_CLOSE);
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
	} else if(btnClicked == m_searchUp){
		if(m_searchUp->IsChecked()) {
			flags |= wxFRD_SEARCHUP;
		} else {
			flags &= ~(wxFRD_SEARCHUP);
		}
	} else if(btnClicked == m_wrapsearch){
		if(m_wrapsearch->IsChecked()) {
			flags |= wxFRD_WRAPSEARCH;
		} else {
			flags &= ~(wxFRD_WRAPSEARCH);
		}
	}

	// Set the updated flags
	m_data.SetFlags(flags);
}

void FindReplaceDialog::OnClose(wxCloseEvent &event)
{
	wxUnusedVar(event);

	// Fire a close event
	SendEvent(wxEVT_FRD_CLOSE);
	Hide();
}
void FindReplaceDialog::OnKeyDown(wxKeyEvent &event)
{
	if(event.GetKeyCode() == WXK_RETURN){
		// start the search
		size_t flags = m_data.GetFlags();
		m_data.SetFindString( m_findString->GetValue() );
		m_data.SetReplaceString( m_replaceString->GetValue() );
		SendEvent(wxEVT_FRD_FIND_NEXT);
		// Set the updated flags
		m_data.SetFlags(flags);
		event.Skip(false);
	}else if(event.GetKeyCode() == WXK_ESCAPE){
		//hide the dialog
		Hide();
		event.Skip(false);
	}else{ 
		//skip event
		event.Skip();
	}
}

void FindReplaceDialog::ConnectEvents()
{
	// Connect buttons
	ConnectKeyDown(m_findString, FindReplaceDialog::OnKeyDown);
	ConnectKeyDown(m_replaceString, FindReplaceDialog::OnKeyDown);

	m_find->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_replace->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_replaceAll->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_markAll->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_cancel->Connect(wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);

	// connect options
	m_matchCase->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_matchWholeWord->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_regualrExpression->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_searchUp->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);
	m_wrapsearch->Connect(wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED , wxCommandEventHandler(FindReplaceDialog::OnClick), NULL, this);

}

void FindReplaceDialog::SendEvent(wxEventType type)
{
	wxCommandEvent event(type, GetId());
	event.SetEventObject(this);

	if( GetEventOwner() == NULL )
		GetEventHandler()->ProcessEvent( event );
	else
		// If an event owner was provided, pass it the event
		GetEventOwner()->ProcessEvent( event );
}

bool FindReplaceDialog::Show()
{
	if( IsShown() )
		return true;

	return wxDialog::Show();
}

void FindReplaceDialog::SetReplacementsMessage(const wxString &msg)
{
	m_replacementsMsg->SetLabel(msg);
}
