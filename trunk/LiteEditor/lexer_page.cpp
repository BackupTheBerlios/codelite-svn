///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "lexer_page.h"
#include "lexer_configuration.h"
#include "attribute_style.h"
#include <wx/font.h>
#include "editor_config.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( LexerPage, wxPanel )
EVT_LISTBOX( wxID_ANY, LexerPage::OnItemSelected )
EVT_FONTPICKER_CHANGED(wxID_ANY, LexerPage::OnFontChanged)
EVT_COLOURPICKER_CHANGED(wxID_ANY, LexerPage::OnColourChanged)
END_EVENT_TABLE()

LexerPage::LexerPage( wxWindow* parent, LexerConfPtr lexer, int id, wxPoint pos, wxSize size, int style ) 
: wxPanel(parent, id, pos, size, style)
, m_lexer(lexer)
, m_selection(0)
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, -1, wxT("Display Item:") ), wxHORIZONTAL );

	m_properties = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer5->Add( m_properties, 1, wxALL|wxEXPAND, 5 );

	m_propertyList = m_lexer->GetProperties();
	std::list<StyleProperty>::iterator it = m_propertyList.begin();

	for(; it != m_propertyList.end(); it++){
		m_properties->Append((*it).GetName());
	}
	m_properties->SetSelection(0);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxString initialColor = wxT("BLACK");
	wxFont initialFont = wxNullFont;

	if(m_propertyList.empty() == false){
		StyleProperty p;
		p = (*m_propertyList.begin());
		initialColor = p.GetFgColour();

		int size = p.GetFontSize();
		wxString face = p.GetFaceName();
		bool bold = p.IsBold();

		initialFont = wxFont(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		initialFont.SetFaceName(face);
	}

	m_fontPicker = new wxFontPickerCtrl(this, wxID_ANY, initialFont);
	bSizer7->Add( m_fontPicker, 0, wxALL|wxEXPAND, 5 );

	m_colourPicker = new wxColourPickerCtrl(this, wxID_ANY, wxColour(initialColor), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	bSizer7->Add( m_colourPicker, 0, wxALL|wxEXPAND, 5 );

	sbSizer5->Add( bSizer7, 1, wxEXPAND, 5 );

	wxStaticBoxSizer *hs = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT("File Types:"));
	m_fileSpec = new wxTextCtrl(this, wxID_ANY, m_lexer->GetFileSpec()); 
	hs->Add(m_fileSpec, 1, wxALL | wxEXPAND, 5);

	bSizer6->Add( sbSizer5, 1, wxEXPAND, 5 );
	bSizer6->Add( hs, 0, wxEXPAND, 5 );
	this->SetSizer( bSizer6 );
	this->Layout();

	if(m_propertyList.empty()){
		m_fontPicker->Enable(false);
		m_colourPicker->Enable(false);
	}
}

void LexerPage::OnItemSelected(wxCommandEvent & event)
{
	// update colour picker & font pickers
	wxString selectionString = event.GetString();
	m_selection = event.GetSelection();

	std::list<StyleProperty>::iterator iter = m_propertyList.begin();
	for(; iter != m_propertyList.end(); iter++){
		if(iter->GetName() == selectionString){
			// update font & color
			StyleProperty p = (*iter);
			wxString colour = p.GetFgColour();
			wxFont font = wxNullFont;

			int size = p.GetFontSize();
			wxString face = p.GetFaceName();
			bool bold = p.IsBold();

			font = wxFont(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
			m_fontPicker->SetSelectedFont(font);
			m_colourPicker->SetColour(colour);
		}
	}
}

void LexerPage::OnFontChanged(wxFontPickerEvent &event)
{
	// update font
	wxFont font = event.GetFont();
	std::list<StyleProperty>::iterator iter = m_propertyList.begin();
	for(int i=0; i<m_selection; i++)
		iter++;

	iter->SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
	iter->SetFaceName(font.GetFaceName());
	iter->SetFontSize(font.GetPointSize());
}

void LexerPage::OnColourChanged(wxColourPickerEvent &event)
{
	// update font
	wxColour colour = event.GetColour();
	std::list<StyleProperty>::iterator iter = m_propertyList.begin();
	for(int i=0; i<m_selection; i++)
		iter++;

	iter->SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
}

void LexerPage::SaveSettings()
{
	m_lexer->SetProperties( m_propertyList );
	m_lexer->SetFileSpec( m_fileSpec->GetValue() );
	EditorConfigST::Get()->SetLexer( m_lexer );
}
