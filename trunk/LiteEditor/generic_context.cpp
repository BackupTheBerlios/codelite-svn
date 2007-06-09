#include "generic_context.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextGeneric::ContextGeneric(LEditor *container, const wxString &name) 
: ContextBase(container)
{
	//-----------------------------------------------
	// Load laguage settings from configuration file
	//-----------------------------------------------
	SetName(name);

	// Set the key words and the lexer
	wxString keyWords;
	std::list<StyleProperty> styles;
	LexerConfPtr lexPtr;
	// Read the configuration file
	if(EditorConfigST::Get()->IsOk()){
		lexPtr = EditorConfigST::Get()->GetLexer(name);
	}

	// Update the control
	LEditor &rCtrl = GetCtrl();
	rCtrl.SetLexer(lexPtr->GetLexerId());
	rCtrl.SetKeyWords(0, lexPtr->GetKeyWords());
	rCtrl.StyleClearAll();
	
	styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for(; iter != styles.end(); iter++)
	{
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();
		
		wxFont font(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);

		rCtrl.StyleSetFont(st.GetId(), font);
		rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
		rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
	}
}


ContextGeneric::~ContextGeneric()
{
}

ContextBase *ContextGeneric::NewInstance(LEditor *container){
	return new ContextGeneric(container, GetName());
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::AutoIndent(const wxChar &nChar)
{
	ContextBase::AutoIndent(nChar);
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::CodeComplete()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::CompleteWord()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::GotoDefinition()
{
}

// Dont implement this function, maybe derived child will want 
// to do something with it
void ContextGeneric::GotoPreviousDefintion()
{
}
