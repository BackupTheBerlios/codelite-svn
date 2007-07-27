///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "tags_options_dlg.h"
#include "macros.h"

//---------------------------------------------------------

TagsOptionsDlg::TagsOptionsDlg( wxWindow* parent, const TagsOptionsData& data, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
, m_data(data)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	long book_style = wxFNB_NO_NAV_BUTTONS | wxFNB_NO_X_BUTTON | wxFNB_FF2 | wxFNB_BACKGROUND_GRADIENT;
	m_mainBook = new wxFlatNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, book_style);
	m_generalPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_generalPage, -1, wxT("General:") ), wxVERTICAL );
	m_checkParseComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Parse comments"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkParseComments, 0, wxALL, 5 );
	
	m_checkDisplayComments = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display comments in tooltip"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkDisplayComments, 0, wxALL, 5 );
	
	m_checkDisplayTypeInfo = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display type info tooltips"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkDisplayTypeInfo, 0, wxALL, 5 );
	
	m_checkDisplayFunctionTip = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Display function calltip "), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_checkDisplayFunctionTip, 0, wxALL, 5 );
	
	m_checkLoadLastDB = new wxCheckBox( m_generalPage, wxID_ANY, wxT("Automatically load the recently used additional symbols database"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer2->Add( m_checkLoadLastDB, 0, wxALL, 5 );
	
	bSizer4->Add( sbSizer2, 1, wxEXPAND, 5 );
	
	m_generalPage->SetSizer( bSizer4 );
	m_generalPage->Layout();
	bSizer4->Fit( m_generalPage );
	m_mainBook->AddPage( m_generalPage, wxT("CodeLite"), true );
	m_ctagsPage = new wxPanel( m_mainBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("Preprocessor file:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_filePicker = new FilePicker(m_ctagsPage, wxID_ANY);
	fgSizer2->Add( m_filePicker, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText3 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("File Types:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textFileSpec = new wxTextCtrl( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textFileSpec, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( m_ctagsPage, wxID_ANY, wxT("Force Language:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBoxLang = new wxComboBox( m_ctagsPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	fgSizer2->Add( m_comboBoxLang, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_ctagsPage->SetSizer( bSizer6 );
	m_ctagsPage->Layout();
	bSizer6->Fit( m_ctagsPage );
	m_mainBook->AddPage( m_ctagsPage, wxT("ctags"), false );
	
	mainSizer->Add( m_mainBook, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();

	InitValues();

	ConnectButton(m_buttonOK, TagsOptionsDlg::OnButtonOK);
}

void TagsOptionsDlg::InitValues()
{
	//initialize the CodeLite page
	m_checkParseComments->SetValue(m_data.GetFlags() & CC_PARSE_COMMENTS ? true : false);
	m_checkDisplayFunctionTip->SetValue(m_data.GetFlags() & CC_DISP_FUNC_CALLTIP ? true : false);
	m_checkLoadLastDB->SetValue(m_data.GetFlags() & CC_LOAD_EXT_DB ? true : false);
	m_checkDisplayTypeInfo->SetValue(m_data.GetFlags() & CC_DISP_TYPE_INFO ? true : false);
	m_checkDisplayComments->SetValue(m_data.GetFlags() & CC_DISP_COMMENTS ? true : false);

	//initialize the ctags page
	m_filePicker->SetPath(m_data.GetPreprocessorFilename().GetFullPath());
	m_textFileSpec->SetValue(m_data.GetFileSpec());
	
	m_comboBoxLang->Clear();
	m_comboBoxLang->Append(m_data.GetLanguages());
	wxString lan = m_data.GetLanguages().Item(0);
	m_comboBoxLang->SetStringSelection(lan);
}

void TagsOptionsDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	CopyData();
	EndModal(wxID_OK);
}

void TagsOptionsDlg::CopyData()
{
	//save data to the interal member m_data
	SetFlag(CC_DISP_COMMENTS, m_checkDisplayComments->IsChecked());
	SetFlag(CC_DISP_FUNC_CALLTIP, m_checkDisplayFunctionTip->IsChecked());
	SetFlag(CC_DISP_TYPE_INFO, m_checkDisplayTypeInfo->IsChecked());
	SetFlag(CC_LOAD_EXT_DB, m_checkLoadLastDB->IsChecked());
	SetFlag(CC_PARSE_COMMENTS, m_checkParseComments->IsChecked());
	m_data.SetFileSpec(m_textFileSpec->GetValue());
	m_data.SetPreprocessorFilename(m_filePicker->GetPath());
	m_data.SetLanguages(m_comboBoxLang->GetStrings());
	m_data.SetLanguageSelection(m_comboBoxLang->GetStringSelection());
}

void TagsOptionsDlg::SetFlag(CodeCompletionOpts flag, bool set)
{
	if(set){
		m_data.SetFlags(m_data.GetFlags() | flag);
	}else{
		m_data.SetFlags(m_data.GetFlags() & ~(flag));
	}
}
