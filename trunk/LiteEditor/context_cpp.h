#ifndef CONTEXT_CPP_H
#define CONTEXT_CPP_H

#include "context_base.h"
#include "calltip.h"
#include <map>
#include "entry.h"

class ContextCpp : public ContextBase {
	clCallTipPtr m_ct;
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

	//images used by the C++ context
	static wxBitmap m_classBmp;
	static wxBitmap m_structBmp;
	static wxBitmap m_namespaceBmp;
	static wxBitmap m_variableBmp;
	static wxBitmap m_tpyedefBmp;
	static wxBitmap m_memberPrivateBmp;
	static wxBitmap m_memberPublicBmp;
	static wxBitmap m_memberProtectedeBmp;
	static wxBitmap m_functionPrivateBmp;
	static wxBitmap m_functionPublicBmp;
	static wxBitmap m_functionProtectedeBmp;
	static wxBitmap m_macroBmp;
	static wxBitmap m_enumBmp;

private:
	bool TryOpenFile(const wxFileName &fileName);

public:
	ContextCpp(LEditor *container);
	virtual ~ContextCpp();
	ContextCpp() : ContextBase(wxT("C++")) {};
	virtual ContextBase *NewInstance(LEditor *container);

	virtual void CompleteWord();
	virtual void CodeComplete();
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel();
	virtual	bool IsCommentOrString(long pos);

	//override swapfiles features
	virtual bool IsSwapFilesEnabled() const {return true;}
	virtual void SwapFiles(const wxFileName &fileName);

	//Event handlers
	virtual void OnDwellEnd(wxScintillaEvent &event);
	virtual void OnCallTipClick(wxScintillaEvent &event);
	virtual void OnDwellStart(wxScintillaEvent &event);

private:
	void GetWordAndScope(wxString& word, wxString &scope, wxString &scopeName);
	wxString GetImageString(const TagEntry &entry);
};

#endif // CONTEXT_CPP_H
