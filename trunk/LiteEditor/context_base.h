#ifndef CONTEXT_BASE_H
#define CONTEXT_BASE_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"

class LEditor;

/**
 * \ingroup LiteEditor
 * \brief This class defines the language context of the editor
 *
 * \version 1.0
 * first version
 *
 * \date 04-30-2007
 *
 * \author Eran
 *
 */
class ContextBase {
protected:
	LEditor *m_container;

public:
	// ctor-dtor
	ContextBase(LEditor *container);
	ContextBase();
	virtual ~ContextBase();
	LEditor &GetCtrl() { return *m_container; }

	// every Context derived class must implement the Clone method
	virtual ContextBase *NewInstance(LEditor *container) = 0;

	virtual void CompleteWord() = 0;
	virtual void CodeComplete() = 0;
	virtual void GotoDefinition() = 0;
	virtual void GotoPreviousDefintion() = 0;
	virtual void AutoIndent(const wxChar&) = 0;
	virtual void CallTipCancel() = 0;

	virtual bool IsCommentOrString(long WXUNUSED(pos)) = 0;

	// event handlers
	virtual void OnDwellEnd(wxScintillaEvent & WXUNUSED(event)) = 0;
	virtual void OnCallTipClick(wxScintillaEvent& WXUNUSED(event)) = 0;
	virtual void OnDwellStart(wxScintillaEvent & WXUNUSED(event)) = 0;
};
typedef SmartPtr<ContextBase> ContextBasePtr;

#endif // CONTEXT_BASE_H

