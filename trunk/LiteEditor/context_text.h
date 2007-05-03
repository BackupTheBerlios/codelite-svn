#ifndef CONTEXT_TEXT_H
#define CONTEXT_TEXT_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief the basic editor from which complicated editors derives from (e.g. ContextCpp)
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 *
 */
class ContextText {
	LEditor *m_container;

public:
	ContextText(LEditor *container);
	virtual ~ContextText();
	LEditor &GetCtrl() { return *m_container; }

	virtual void CompleteWord();
	virtual void CodeComplete();
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel(){};

	virtual bool IsCommentOrString(long WXUNUSED(pos)){ return false; }

	// event handlers
	virtual void OnDwellEnd(wxScintillaEvent & WXUNUSED(event)) {};
	virtual void OnCallTipClick(wxScintillaEvent& WXUNUSED(event)) {};
	virtual void OnDwellStart(wxScintillaEvent & WXUNUSED(event)) {};
};
#endif // CONTEXT_TEXT_H

