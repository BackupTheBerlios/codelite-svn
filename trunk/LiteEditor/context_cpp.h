#ifndef CONTEXT_CPP_H
#define CONTEXT_CPP_H

#include "context_base.h"
#include "calltip.h"
#include <map>

class ContextCpp : public ContextBase {
	CallTipPtr m_ct;
	std::map<wxString, int> m_propertyInt;

	enum TipKind
	{
		TipNone = -1,
		TipHover,
		TipFuncProto
	};

	enum CalltipClickPos
	{
		Elsewhere = 0,
		ArrowUp ,
		ArrowDown
	};

	TipKind m_tipKind;

public:
	ContextCpp(LEditor *container);
	virtual ~ContextCpp();

	virtual void CompleteWord();
	virtual void CodeComplete();
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel();
	virtual	bool IsCommentOrString(long pos);

	//Event handlers
	virtual void OnDwellEnd(wxScintillaEvent &event);
	virtual void OnCallTipClick(wxScintillaEvent &event);
	virtual void OnDwellStart(wxScintillaEvent &event);

private:
	void GetWordAndScope(wxString& word, wxString &scope, wxString &scopeName);
};

#endif // CONTEXT_CPP_H
