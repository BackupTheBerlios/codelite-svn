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
#include "cl_process.h"
#include "envvar_table.h"
#include "wx/choice.h"
#include "wx/timer.h"

// forward decls
class TagEntry;


/**
 * The main frame class
 * \author Eran Ifrah
 */
class Frame : public wxFrame
{
	bool m_restartCtags;
	wxFlatNotebook *m_notebook;
	static Frame* m_theFrame;
	wxAuiManager m_mgr;
	OutputPane *m_outputPane;
	FindInFilesDialog *m_findInFilesDlg;
	FindReplaceData m_data;
	WorkspacePane *m_workspacePane;
	wxArrayString m_files;
	wxChoice *m_workspaceConfig;
	wxTimer *m_timer;

public:
	// the access method to the singleton frame is by using the Get method
	static Frame* Get();
	virtual ~Frame(void);

	/**
	 * Return the main editor notebook
	 */
	wxFlatNotebook *GetNotebook() { return m_notebook; }


	/**
	 * Return the workspace configuration choice control
	 */
	wxChoice *GetConfigChoice() { return m_workspaceConfig; }

	/**
	 * Close the current file
	 */
	void CloseActiveFile();
	
	/**
	 * \return the output pane (the bottom pane)
	 */
	OutputPane *GetOutputPane() { return m_outputPane; }

	/**
	 * \return the workspace pane (the one that contained the Symbol view & class view)
	 */
	WorkspacePane *GetWorkspacePane() { return m_workspacePane; }

	/**
	 * \return return AUI docking manager
	 */
	wxAuiManager& GetDockingManager() { return m_mgr; }

private:
	// make our frame's constructor private
	Frame(wxWindow *pParent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX | wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER | wxCLIP_CHILDREN, const wxString& name = wxT("Main Frame"));

private:	
	/**
	 * Construct all the GUI controls of the main frame. this function is called
	 * at construction time
	 */
	void CreateGUIControls(void);
	/**
	 * Helper function that prompt user with a simple wxTextEntry dialog
	 * \param msg message to display to user
	 * \return user's string or wxEmptyString if 'Cancel' pressed.
	 */
	wxString GetStringFromUser(const wxString& msg);
	void ClosePage(LEditor *editor, bool notify, int index, bool doDelete, bool &veto);
	void DispatchCommandEvent(wxCommandEvent &event);
	void DispatchUpdateUIEvent(wxUpdateUIEvent &event);
	void CreateToolbars();
	void ViewPaneUI(const wxString &paneName, wxUpdateUIEvent&event);
	void ViewPane(const wxString &paneName, wxCommandEvent &event);

protected:
	//----------------------------------------------------
	// event handlers
	//----------------------------------------------------
	void OnSearchThread(wxCommandEvent &event);
	void OnBuildEvent(wxCommandEvent &event);
	void OnQuit(wxCommandEvent& WXUNUSED(event));
	void OnClose(wxCloseEvent &event);
	void OnAddSourceFile(wxCommandEvent& event);
	void OnBuildFromDatabase(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnFileReload(wxCommandEvent& event);
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
	void OnViewWorkspacePane(wxCommandEvent &event);
	void OnViewOutputPane(wxCommandEvent &event);
	void OnViewToolbar(wxCommandEvent &event);
	void OnViewToolbarUI(wxUpdateUIEvent &event);
	void OnViewWorkspacePaneUI(wxUpdateUIEvent &event);
	void OnViewOutputPaneUI(wxUpdateUIEvent &event);
	void OnViewOptions(wxCommandEvent &event);
	void OnTogglePanes(wxCommandEvent &event);
	void OnNewDlgCreate(wxCommandEvent &event);
	void OnProjectNewWorkspace(wxCommandEvent &event);
	void OnProjectNewProject(wxCommandEvent &event);
	void OnCreateWorkspace(wxCommandEvent &event);
	void OnSwitchWorkspace(wxCommandEvent &event);
	void OnProjectAddProject(wxCommandEvent &event);
	void OnWorkspaceOpen(wxUpdateUIEvent &event);
	void OnConfigurationManager(wxCommandEvent &event);
	void OnAddEnvironmentVariable(wxCommandEvent &event);
	void OnAdvanceSettings(wxCommandEvent &event);
	void OnCtagsOptions(wxCommandEvent &event);
	void OnBuildProject(wxCommandEvent &event);
	void OnBuildProjectUI(wxUpdateUIEvent &event);
	void OnStopBuild(wxCommandEvent &event);
	void OnStopBuildUI(wxUpdateUIEvent &event);
	void OnCleanProject(wxCommandEvent &event);
	void OnCleanProjectUI(wxUpdateUIEvent &event);
	void OnExecuteNoDebug(wxCommandEvent &event);
	void OnExecuteNoDebugUI(wxUpdateUIEvent &event);
	void OnWorkspaceConfigChanged(wxCommandEvent &event);
	void OnTimer(wxTimerEvent &event);

	// this event is sent from the notebook container to the frame
	void OnFileClosing(wxFlatNotebookEvent &event);
	void OnPageChanged(wxFlatNotebookEvent &event);
	void OnPageClosed(wxFlatNotebookEvent &event);

	// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // LITEEDITOR_FRAME_H
