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
#include "findinfilesdlg.h"
#include "cl_process.h"
#include "envvar_table.h"
#include "wx/choice.h"
#include "wx/timer.h"
#include "parse_thread.h"
#include "tags_options_dlg.h"

// forward decls
class TagEntry;
class WorkspacePane;

/**
 * The main frame class
 * \author Eran Ifrah
 */
class Frame : public wxFrame
{
	wxFlatNotebook *m_notebook;
	static Frame* m_theFrame;
	wxAuiManager m_mgr;
	OutputPane *m_outputPane;
	FindInFilesDialog *m_findInFilesDlg;
	FindReplaceData m_data;
	WorkspacePane *m_workspacePane;
	wxArrayString m_files;
	wxComboBox *m_workspaceConfig;
	wxTimer *m_timer;
	std::map<int, wxString> m_viewAsMap;
	wxMenu *m_tabRightClickMenu;
	TagsOptionsData m_tagsOptionsData;

public:	
	// the access method to the singleton frame is by using the Get method
	static Frame* Get();
	virtual ~Frame(void);

	/**
	 * Return language name by menu item id
	 */
	wxString GetViewAsLanguageById(int id) const;

	/**
	 * Return the main editor notebook
	 */
	wxFlatNotebook *GetNotebook() { return m_notebook; }


	/**
	 * Return the workspace configuration choice control
	 */
	wxComboBox *GetConfigChoice() { return m_workspaceConfig; }

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

	/**
	 * return the find in files dialog pointer
	 */
	FindInFilesDialog *GetFindInFilesDlg(){return m_findInFilesDlg;}
	
	/**
	 * close editor's page
	 * \param editor the notebook parent of the page
	 * \param notify send notebook page closure event
	 * \param index page index in the notebook
	 * \param doDelete delete the page from the notebook as well
	 * \param veto [output] set to true to veto the page closer
	 */
	void ClosePage(LEditor *editor, bool notify, int index, bool doDelete, bool &veto);

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
	void DispatchCommandEvent(wxCommandEvent &event);
	void DispatchUpdateUIEvent(wxUpdateUIEvent &event);
	void CreateToolbars();
	void ViewPaneUI(const wxString &paneName, wxUpdateUIEvent&event);
	void ViewPane(const wxString &paneName, wxCommandEvent &event);
	void CreateViewAsSubMenu();
    void CreateRecentlyOpenedFilesMenu();
	void CreateRecentlyOpenedWorkspacesMenu();

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
	void OnCloseExternalDatabase(wxCommandEvent& event);
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
	void OnOutputWindowEvent(wxCommandEvent &event);
	void OnFileCloseAll(wxCommandEvent &event);
	void OnFindResource(wxCommandEvent &event);
	void OnFindType(wxCommandEvent &event);
	void OnImportMakefile(wxCommandEvent &event);
	void OnImportMakefileUI(wxUpdateUIEvent &event);

	// this event is sent from the notebook container to the frame
	void OnFileClosing(wxFlatNotebookEvent &event);
	void OnPageChanged(wxFlatNotebookEvent &event);
	void OnPageClosed(wxFlatNotebookEvent &event);

	//handle symbol tree events
	void OnAddSymbols(SymbolTreeEvent &event);
	void OnDeleteSymbols(SymbolTreeEvent &event);
	void OnUpdateSymbols(SymbolTreeEvent &event);

	void OnRecentFile(wxCommandEvent &event);
	void OnRecentWorkspace(wxCommandEvent &event);

	// Any class wishing to process wxWindows events must use this macro
	DECLARE_EVENT_TABLE()
};

#endif // LITEEDITOR_FRAME_H
