#include "context_text.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextText::ContextText(LEditor *container)
: m_container(container)
{
	// Initialise default style settings

	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------

	// Set the key words and the lexer
	wxString keyWords;
	std::vector<AttributeStyle> styles;

	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		EditorConfigST::Get()->LoadStyle(wxT("Default"), styles);
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(wxSCI_LEX_NULL);
	rCtrl.StyleClearAll();

	if( styles.empty() == false ){ 
		int size = styles[0].GetFontSize();
		wxString face = styles[0].GetFaceName();
		bool bold = styles[0].IsBold();

		wxFont font(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		font.SetFaceName(face);

		rCtrl.StyleSetFont(0, font);
		rCtrl.StyleSetSize(0, styles[0].GetFontSize());
		rCtrl.StyleSetForeground(0, styles[0].GetFgColour());
	}
}

ContextText::~ContextText()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::AutoIndent(const wxChar &nChar)
{
	wxUnusedVar(nChar);
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::CodeComplete()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::CompleteWord()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::GotoDefinition()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextText::GotoPreviousDefintion()
{
}
