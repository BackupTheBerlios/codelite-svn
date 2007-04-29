#ifndef EDITOR_BASE_H
#define EDITOR_BASE_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief the basic editor from which complicated editors derives from (e.g. EditorCpp)
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
class EditorBase {
	LEditor *m_container;

public:
	EditorBase(LEditor *container);
	virtual ~EditorBase();
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
typedef SmartPtr<EditorBase> EditorBasePtr;

#endif // EDITOR_BASE_H

