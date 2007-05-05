#include "context_text.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextText::ContextText(LEditor *container) 
: ContextBase(container, wxT("Text"))
{
	// Initialise default style settings

	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------

	// Set the key words and the lexer
	wxString keyWords;
	std::list<StyleProperty> styles;

	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		styles = EditorConfigST::Get()->GetLexer(wxT("Default"))->GetProperties();
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(wxSCI_LEX_NULL);
	rCtrl.StyleClearAll();

	std::list<StyleProperty>::iterator iter = styles.begin();
	if(iter != styles.end()){
		int size = (*iter).GetFontSize();
		wxString face = (*iter).GetFaceName();
		bool bold = (*iter).IsBold();

		wxFont font(size, wxFONTFAMILY_DEFAULT, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
		font.SetFaceName(face);

		rCtrl.StyleSetFont(0, font);
		rCtrl.StyleSetSize(0, (*iter).GetFontSize());
		rCtrl.StyleSetForeground(0, (*iter).GetFgColour());
	}
}

ContextText::~ContextText()
{
}

ContextBase *ContextText::NewInstance(LEditor *container){
	return new ContextText(container);
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
