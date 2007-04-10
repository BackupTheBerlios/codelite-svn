#ifndef LITEEDITOR_FRAME_H
#define LITEEDITOR_FRAME_H

#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/dcbuffer.h>
#include <wx/process.h>
#include "wx/aui/aui.h"
#include "wx/frame.h"
#include "findinfilesdlg.h"

// forward decls
class SymbolTree;
class TagEntry;
class TagsProcess;
class wxSplitterWindow;
class wxConfigBase;
class LEditor;
class FindInFilesDialog;

/**
 * The main frame class
 * \author Eran Ifrah
 */
class Frame : public wxFrame
{
	SymbolTree* m_tree;
	TagsProcess *m_ctags;
	TagsProcess *m_localCtags;
	bool m_restartCtags;
	wxFlatNotebook *m_notebook;
	wxSplitterWindow *m_splitter;

	static Frame* m_theFrame;
	wxString m_installPath;
	wxConfigBase *m_config;
	wxAuiManager m_mgr;
	wxTextCtrl *m_debugWin;
	FindInFilesDialog *m_findInFilesDlg;
	FindReplaceData m_data;

public:
	// the access method to the singleton frame is by using the Get method
	static Frame* Get();
	virtual ~Frame(void);

	void OpenFile(const TagEntry& tag);
	const wxString& GetInstallPath() const { return m_installPath; }
	wxFlatNotebook *GetNotebook() { return m_notebook; }
	SymbolTree *GetSymbolTree() { return m_tree; }
	void CloseActiveFile();

private:
	// make our frame's constructor private
	Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN, const wxString& name = wxT("Main Frame"));

private:
	void CreateGUIControls(void);
	wxString GetStringFromUser(const wxString& msg);
	void ClosePage(LEditor *editor, int index, bool doDelete, bool &veto);
	void DispatchCommandEvent(wxCommandEvent &event);
	void DispatchUpdateUIEvent(wxUpdateUIEvent &event);

protected:
	// event handlers
	void OnSearchThread(wxCommandEvent &event);
	void OnQuit(wxCommandEvent& WXUNUSED(event));
	void OnClose(wxCloseEvent &event);
	void OnAddSourceFile(wxCommandEvent& event);
	void OnBuildFromDatabase(wxCommandEvent& event);
	void OnCtagsEnd(wxProcessEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnCompleteWord(wxCommandEvent& event);
	void OnGotoDefinition(wxCommandEvent& event);
	void OnGotoPreviousDefinition(wxCommandEvent& event);
	void OnDeleteProject(wxCommandEvent& event);
	void OnBuildExternalDatabase(wxCommandEvent& event);
	void OnUseExternalDatabase(wxCommandEvent& event);
	void OnParseComments(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnFileNew(wxCommandEvent &event);
	void OnFileOpen(wxCommandEvent &event);
	void OnFileClose(wxCommandEvent &event);
	void OnFileSaveAll(wxCommandEvent &event);
	void OnFileFindAndReplace(wxCommandEvent &event);
	void OnFileExistUpdateUI(wxUpdateUIEvent &event);
	void OnCompleteWordUpdateUI(wxUpdateUIEvent &event);
	void OnWorkspaceOpen(wxUpdateUIEvent &event);
	void OnFindInFiles(wxCommandEvent &event);
	void OnCreateWorkspace(wxCommandEvent &event);
	void OnSwitchWorkspace(wxCommandEvent &event);
	
	// New Dialog 'Create' button pressed
	void OnNewDlgCreate(wxCommandEvent &event);
	
	// Project->New Workspace
	void OnProjectNewWorkspace(wxCommandEvent &event);
	void OnProjectNewProject(wxCommandEvent &event);

	// this event is sent from the notebook container to the frame
	void OnFileClosing(wxFlatNotebookEvent &event);
	void OnPageChanged(wxFlatNotebookEvent &event);

	// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()

};

#endif // LITEEDITOR_FRAME_H
