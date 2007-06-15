#ifndef CONTEXT_BASE_H
#define CONTEXT_BASE_H

#include "wx/string.h"
#include "wx/wxscintilla.h"
#include "smart_ptr.h"
#include "wx/filename.h"

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
	wxString m_name;

protected:
	void SetName(const wxString &name){m_name = name;}

public:

	// ctor-dtor
	ContextBase(LEditor *container);
	ContextBase(const wxString &name);
	virtual ~ContextBase();

	/**
	 * Return the context parent control
	 */
	LEditor &GetCtrl() { return *m_container; }

	/**
	 * Return the context name
	 */
	const wxString &GetName() const { return m_name; }

	// every Context derived class must implement the following methods
	virtual ContextBase *NewInstance(LEditor *container) = 0;
	virtual void CompleteWord() = 0;
	virtual void CodeComplete() = 0;
	virtual void GotoDefinition() = 0;
	virtual void GotoPreviousDefintion() = 0;
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel() = 0;
	virtual bool IsCommentOrString(long WXUNUSED(pos)) = 0;
	virtual void OnDwellEnd(wxScintillaEvent & WXUNUSED(event)) = 0;
	virtual void OnCallTipClick(wxScintillaEvent& WXUNUSED(event)) = 0;
	virtual void OnDwellStart(wxScintillaEvent & WXUNUSED(event)) = 0;
	virtual void SwapFiles(const wxFileName & WXUNUSED(fileName)){};
	virtual bool IsSwapFilesEnabled() const { return false; }
};
typedef SmartPtr<ContextBase> ContextBasePtr;

#endif // CONTEXT_BASE_H

