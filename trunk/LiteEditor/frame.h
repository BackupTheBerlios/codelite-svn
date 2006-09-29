#ifndef LITEEDITOR_FRAME_H
#define LITEEDITOR_FRAME_H

#include <wx/wxFlatNotebook/styled_notebook.h>
#include <wx/dcbuffer.h>
#include <wx/process.h>

// forward decls
namespace codelite
{
    class SymbolTree;
    class TagEntry;
    class TagsProcess;
}
class wxSplitterWindow;
class wxConfigBase;

class Frame : public wxFrame
{
	codelite::SymbolTree* m_tree;
	codelite::TagsProcess *m_ctags;
	codelite::TagsProcess *m_localCtags;
	bool m_restartCtags;
	wxStyledNotebook *m_notebook;
	wxSplitterWindow *m_splitter;
	wxLog *m_logTargetOld;

	static Frame* m_theFrame;
	wxString m_installPath;
	wxConfigBase *m_config;
public:
	static Frame* Get();
	virtual ~Frame(void);
	void OpenFile(const codelite::TagEntry& tag);
	const wxString& GetInstallPath() const { return m_installPath; }
private:

	// Signleton
	Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN, const wxString& name = wxT("Main Frame"));

private:
	void CreateGUIControls(void);
	wxString GetStringFromUser(const wxString& msg);

	// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()

	void OnQuit(wxCommandEvent& WXUNUSED(event));
	void OnClose(wxCloseEvent &event);
	void OnAddSourceFile(wxCommandEvent& event);
	void OnBuildFromDatabase(wxCommandEvent& event);
	void OnCtagsEnd(wxProcessEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnCompleteWord(wxCommandEvent& event);
	void OnGotoDefinition(wxCommandEvent& event);
	void OnGotoPreviousDefinition(wxCommandEvent& event);
	void OnDeleteProject(wxCommandEvent& event);
	void OnBuildExternalDatabase(wxCommandEvent& event);
	void OnUseExternalDatabase(wxCommandEvent& event);
	void OnParseComments(wxCommandEvent& event);
};

#endif // LITEEDITOR_FRAME_H
