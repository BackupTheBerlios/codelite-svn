#ifndef LITEEDITOR_FRAME_H
#define LITEEDITOR_FRAME_H

#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include <wx/dcbuffer.h>
#include <wx/process.h>
#include "wx/aui/aui.h"
#include "wx/frame.h"
#include "findinfilesdlg.h"
#include "editor.h"
#include "output_pane.h"
#include "workspace_pane.h"
#include "workspace_pane.h"
#include "findinfilesdlg.h"
#include "ctags_dialog.h"

// forward decls
class TagEntry;
class TagsProcess;

/**
 * The main frame class
 * \author Eran Ifrah
 */
class Frame : public wxFrame
{
	TagsProcess *m_ctags;
	TagsProcess *m_localCtags;
	bool m_restartCtags;
	wxFlatNotebook *m_notebook;
	static Frame* m_theFrame;
	wxAuiManager m_mgr;
	OutputPane *m_outputPane;
	FindInFilesDialog *m_findInFilesDlg;
	FindReplaceData m_data;
	WorkspacePane *m_workspacePane;

public:
	// the access method to the singleton frame is by using the Get method
	static Frame* Get();
	virtual ~Frame(void);

	wxFlatNotebook *GetNotebook() { return m_notebook; }
	void CloseActiveFile();
	
	// Getters
	OutputPane *GetOutputPane() { return m_outputPane; }
	WorkspacePane *GetWorkspacePane() { return m_workspacePane; }

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
	void OnFindInFiles(wxCommandEvent &event);
	
	// New Dialog 'Create' button pressed
	void OnNewDlgCreate(wxCommandEvent &event);
	
	// Project Menu
	void OnProjectNewWorkspace(wxCommandEvent &event);
	void OnProjectNewProject(wxCommandEvent &event);
	void OnCreateWorkspace(wxCommandEvent &event);
	void OnSwitchWorkspace(wxCommandEvent &event);
	void OnProjectAddProject(wxCommandEvent &event);
	void OnWorkspaceOpen(wxUpdateUIEvent &event);

	// tags menu
	void OnCtagsOptions(wxCommandEvent &event);

	// this event is sent from the notebook container to the frame
	void OnFileClosing(wxFlatNotebookEvent &event);
	void OnPageChanged(wxFlatNotebookEvent &event);

	// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()

};

#endif // LITEEDITOR_FRAME_H
