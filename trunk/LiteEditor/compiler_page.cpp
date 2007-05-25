///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
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

#include "compiler_page.h"
#include "macros.h"
#include "editor_config.h"

CompilerPage::CompilerPage( wxWindow* parent, wxString name, int id, wxPoint pos, wxSize size, int style ) 
: wxPanel( parent, id, pos, size, style )
, m_cmpname(name)
{
	//load the compiler from the configuration file
	CompilerPtr cmp = EditorConfigST::Get()->GetCompiler(name);

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Compiler Error Pattern:") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_textErrorPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textErrorPattern, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textErrorPattern->SetValue(cmp->GetErrPattern());

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_textErrorFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textErrorFileIndex, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textErrorFileIndex->SetValue(cmp->GetErrFileNameIndex());

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_textErrorLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textErrorLineNumber, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textErrorLineNumber->SetValue(cmp->GetErrLineNumberIndex());

	sbSizer5->Add( fgSizer3, 0, wxALL|wxEXPAND, 5 );

	bSizer11->Add( sbSizer5, 1, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Compiler Warning Pattern:") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer31;
	fgSizer31 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer31->AddGrowableCol( 1 );
	fgSizer31->SetFlexibleDirection( wxBOTH );
	fgSizer31->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText51 = new wxStaticText( this, wxID_ANY, wxT("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_textWarnPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textWarnPattern, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textWarnPattern->SetValue(cmp->GetWarnPattern());

	m_staticText61 = new wxStaticText( this, wxID_ANY, wxT("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );

	m_textWarnFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textWarnFileIndex, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textWarnFileIndex->SetValue(cmp->GetWarnFileNameIndex());

	m_staticText71 = new wxStaticText( this, wxID_ANY, wxT("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_staticText71, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_textWarnLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer31->Add( m_textWarnLineNumber, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	m_textWarnLineNumber->SetValue(cmp->GetWarnLineNumberIndex());

	sbSizer4->Add( fgSizer31, 0, wxALL|wxEXPAND, 5 );

	bSizer11->Add( sbSizer4, 1, wxALL|wxEXPAND, 5 );

	mainSizer->Add( bSizer11, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Object extesion:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_textObjectExtension = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textObjectExtension, 1, wxALL|wxEXPAND, 5 );
	m_textObjectExtension->SetValue(cmp->GetObjectSuffix());

	mainSizer->Add( fgSizer2, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Switches:") ), wxVERTICAL );

	m_listSwitches = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	sbSizer3->Add( m_listSwitches, 1, wxALL|wxEXPAND, 5 );
	InitSwitches();

	mainSizer->Add( sbSizer3, 1, wxALL|wxEXPAND, 5 );
	this->SetSizer( mainSizer );
	this->Layout();
	ConnectEvents();
}

void CompilerPage::AddSwitch(const wxString &name, const wxString &value, bool choose)
{
	wxListItem info;
	info.SetText(name);
	info.SetColumn(0);
	if(choose == true){
		info.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
		m_selSwitchName = name;
		m_selSwitchValue = value;
	}
	long item = m_listSwitches->InsertItem(info);

	info.SetColumn(1);
	info.SetId(item);
	info.SetText(value);
	info.SetState(0);
	m_listSwitches->SetItem(info); 
}

void CompilerPage::ConnectEvents()
{
	ConnectListCtrlItemActivated(m_listSwitches, CompilerPage::OnItemActivated);
	ConnectListCtrlItemSelected(m_listSwitches, CompilerPage::OnItemSelected);
}

void CompilerPage::OnItemActivated(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listSwitches->GetItem(info) ){
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listSwitches->GetItem(info)){
		m_selSwitchValue = info.m_text;
	}
	EditSwitch();
}

void CompilerPage::OnItemSelected(wxListEvent &event)
{
	//get the var name & value
	wxListItem info;
	info.m_itemId = event.m_itemIndex;
	info.m_col = 0;	//name
	info.m_mask = wxLIST_MASK_TEXT;

	if( m_listSwitches->GetItem(info) ){
		m_selSwitchName = info.m_text;
	}

	info.m_col = 1;//value
	if( m_listSwitches->GetItem(info)){
		m_selSwitchValue = info.m_text;
	}
}

void CompilerPage::InitSwitches()
{
	//insert columns:
	m_listSwitches->Freeze();
	m_listSwitches->ClearAll();
	m_listSwitches->InsertColumn(0, wxT("Name"));
	m_listSwitches->InsertColumn(1, wxT("Value"));

	//populate the list control
	CompilerPtr cmp = EditorConfigST::Get()->GetCompiler(m_cmpname);
	Compiler::ConstIterator iter = cmp->SwitchesBegin();
	for(; iter != cmp->SwitchesEnd(); iter++){
		AddSwitch(iter->first, iter->second, iter == cmp->SwitchesBegin());
	}
	m_listSwitches->Thaw();
}

void CompilerPage::EditSwitch()
{
	wxString message;
	message << wxT("Edit switch ") << m_selSwitchName << wxT(":");
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, message, wxT("Edit"), m_selSwitchValue);
	if(dlg->ShowModal() == wxID_OK){
		wxString newVal = dlg->GetValue();
		CompilerPtr cmp = EditorConfigST::Get()->GetCompiler(m_cmpname);
		cmp->SetSwitch(m_selSwitchName, dlg->GetValue());
		EditorConfigST::Get()->SetCompiler(cmp);
		InitSwitches();
	}
	dlg->Destroy();
}

