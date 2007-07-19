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
	wxMenu *m_rclickMenu;

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
	void DisplayCompletionBox(const std::vector<TagEntryPtr> &tags, const wxString &word);

public:
	ContextCpp(LEditor *container);
	virtual ~ContextCpp();
	ContextCpp();
	virtual ContextBase *NewInstance(LEditor *container);

	virtual void CompleteWord();
	virtual void CodeComplete();
	virtual void GotoDefinition();
	virtual void GotoPreviousDefintion();
	virtual void AutoIndent(const wxChar&);
	virtual void CallTipCancel();
	virtual	bool IsCommentOrString(long pos);

	//override swapfiles features
	virtual void SwapFiles(const wxFileName &fileName);

	//Event handlers
	virtual void OnDwellEnd(wxScintillaEvent &event);
	virtual void OnCallTipClick(wxScintillaEvent &event);
	virtual void OnDwellStart(wxScintillaEvent &event);
	
	//Capture menu events
	//return this context specific right click menu
	virtual wxMenu *GetMenu(){return m_rclickMenu;}
	virtual void OnSwapFiles(wxCommandEvent &event);
	virtual void OnInsertDoxyComment(wxCommandEvent &event);

private:
	wxString GetWordUnderCaret();
	wxString GetImageString(const TagEntry &entry);
	wxString GetExpression(long pos);
};

#endif // CONTEXT_CPP_H

