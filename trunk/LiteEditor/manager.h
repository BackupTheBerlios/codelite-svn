/*!
 * <File comment goes here!!>
 * 
 * Copyright (c) 2007 by Eran Ifrah <eran.ifrah@gmail.com>
 */
#ifndef MANAGER_H
#define MANAGER_H

#include "singleton.h"
#include "wx/string.h"
#include "readtags.h"
#include "entry.h"
#include "project.h"
#include "context_base.h"
#include "ctags_manager.h"
#include "workspace.h"
#include "list"
#include "compile_request.h"
#include "clean_request.h"
#include "wx/event.h"
#include "filehistory.h"

class wxFrame;
class LEditor;
class AsyncExeCmd;

/*!
 * \brief
 * Manager class, a global class that provides access to many 
 * commands of CodeLite
 */
class Manager : public wxEvtHandler
{
	friend class Singleton<Manager>;
	wxString  m_startupDir;
	CleanRequest *m_cleanRequest;
	CompileRequest *m_compileRequest;
	AsyncExeCmd *m_asyncExeCmd;
	FileHistory m_recentFiles;
	FileHistory m_recentWorkspaces;

public:
	/*!
	 * \brief
	 * check if a workspace is open
	 * 
	 * \returns
	 * true if a workspace is open
	 */
	bool IsWorkspaceOpen() const;

	/*!
	 * \brief
	 * Open a file using file name and line number
	 * 
	 * \param fileName full path of the file name
	 * \param projectName project name, can be wxEmptyString
	 * \param lineno the cursor will be placed at lineno
	 * \param position the position of the match starting from begining
	 */
	void OpenFile(const wxString &file_name, 
				  const wxString &projectName, 
				  int lineno = wxNOT_FOUND, 
				  long position = wxNOT_FOUND);

	/**
	 * Create new file on the disk and open it in the main editor
	 * \param fileName file full path (including directories)
	 * \param vdFullPath path of the virtual directory
	 */
	bool AddNewFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt = true);

	/**
	 * Add an existing file to workspace
	 * \param fileName file full path (including directories)
	 * \param vdFullPath path of the virtual directory
	 */
	bool AddFileToProject(const wxString &fileName, const wxString &vdFullPath, bool openIt = false);
	/**
	 * \brief 
	 * \param files
	 * \param vdFullPath
	 * \param actualAdded
	 * \return 
	 */
	void AddFilesToProject(const wxArrayString &files, const wxString &vdFullPath, wxArrayString &actualAdded);

	/*!
	 * \brief
	 * Set the title for the page (tab)
	 */
	void SetPageTitle(wxWindow *page, const wxString &name);

	/*!
	 * \brief
	 * Return the page title 
	 */	
	const wxString GetPageTitle(wxWindow *win);
	
	/*!
	 * \brief
	 * Save all open documents
	 */	
	void SaveAll(bool includeUntitled = true);

	/*!
	 * \brief
	 * Free all singleton objects initialised in CodeLite
	 */	
	void UnInitialize();

	/*!
	 * \brief
	 * Return a pointer to the main frame
	 */	
	wxFrame *GetMainFrame();

	/*!
	 * \brief
	 * Create a workspace with a given name and path
	 * 
	 * \param name
	 * workspace name
	 * 
	 * \param path
	 * workspace path
	 * 
	 */
	void CreateWorkspace(const wxString &name, const wxString &path);
	 
	/**
	 * \brief create an empty project
	 * \param name project name
	 * \param path project file path
	 * \param type project type, Project::STATIC_LIBRARY, Project::DYNAMIC_LIBRARY or Project::EXECUTABLE
	 */
	void CreateProject(ProjectData &data);

	/**
	 * Open an existing workspace by path
	 */
	void OpenWorkspace(const wxString &path);

	/**
	 * Return a tree for the file view of a single project
	 */
	ProjectTreePtr GetProjectFileViewTree(const wxString &projectName);

	/**
	 * Return all project names under this workspace
	 */
	void GetProjectList(wxArrayString &list);

	/**
	 * find project by name
	 */
	ProjectPtr GetProject(const wxString &name) const;

	/**
	 * Add an existing project to the workspace. If no workspace is open,
	 * this function does nothing
	 * \param path project file path name to add
	 */
	void AddProject(const wxString &path);

	/**
	 * Remove the a project from the workspace
	 * \param name project name to remove
	 * \return true on success false otherwise
	 */
	bool RemoveProject(const wxString &name) ;

	/**
	 * \return active project name
	 */
	wxString GetActiveProjectName();

	/**
	 * Set project as active
	 * \param name project name to set as active
	 */
	void SetActiveProject(const wxString &name);

	/**
	 * Add new virtual directory to the workspace.
	 * \param virtualDirFullPath a dot separated string of the new virtual directory full path up to the parent project
	 *        for example: to add a new VD name VD3 under: Project1->VD1->VD2 path should contain: Project1.VD1.VD2.VD3
	 */
	void AddVirtualDirectory(const wxString &virtualDirFullPath);

	/**
	 * Remove virtual directory from the workspace.
	 * \param virtualDirFullPath a dot separated string of the virtual directory to be removed
	 */
	void RemoveVirtualDirectory(const wxString &virtualDirFullPath);

	/**
	 * Save workspace
	 */
	void SaveWorkspace();

	/**
	 * remove file from the workspace
	 * \param fileName the full path of the file to be removed
	 * \param vdFullPath the files' virtual directory path (including project)
	 */
	bool RemoveFile(const wxString &fileName, const wxString &vdFullPath);

	/**
	 * Return a project working directory
	 * \param project project name
	 */
	wxString GetProjectCwd(const wxString &project) const;

	/**
	 * Show output pane and set focus to focusWin
	 * \param focusWin tab name to set the focus
	 */
	void ShowOutputPane(wxString focusWin = wxEmptyString );

	/**
	 * Hide the output pane
	 */
	void HideOutputPane();

	/**
	 * Show the main toolbar
	 * \param show set to true to show it, false otherwise
	 */
	void ShowMainToolbar(bool show = true);

	/**
	 * Show the workspace pane and set focus to focusWin
	 * \param focusWin tab name to set the focus
	 */
	void ShowWorkspacePane(wxString focusWin = wxEmptyString );

	/**
	 * Hide the output pane
	 */
	void HideWorkspacePane();

	/**
	 * Return the context by file name
	 * \param fileName
	 * \param parent the lexer's parent
	 * \return lexer , if no lexer is matched with the fileName, the "Default" context is returned
	 */
	ContextBasePtr NewContextByFileName(const wxFileName &fileName, LEditor *parent) const;

	/**
	 * Reload configuration changes and apply them on open documents
	 */ 
	void ApplySettingsChanges();

	/**
	 * Return project settings by name
	 * \param projectName project name
	 * \return project settings smart prt
	 */
	ProjectSettingsPtr GetProjectSettings(const wxString &projectName) const;

	/**
	 * Set project settings
	 * \param projectName project name
	 * \param settings settings to update
	 */
	void SetProjectSettings(const wxString &projectName, ProjectSettingsPtr settings);

	/** 
	 * Return the workspace build matrix
	 */
	BuildMatrixPtr GetWorkspaceBuildMatrix() const;

	/**
	 * Set or update the workspace build matrix
	 */
	void SetWorkspaceBuildMatrix(BuildMatrixPtr matrix);

	/**
	 * Hide/Show all panes. This function saves the current prespective and 
	 * then hides all panes, when called again, all panes are restored
	 */
	void TogglePanes();

	/**
	 * Set environment variables for the workspace. The new environment will override
	 * the current. To perform update, do the following:
	 * \code
	 * EnvironmentVarieblesPtr env = ManagerST::Get()->GetEnvironmentVariables();
	 * //update the environment
	 * env->SetEnv(wxT("HOME"), wxT("/home/user/eran"));
	 * //update 
	 * ManagerST::Get()->SetEnvironmentVariables(env);
	 * \endcode
	 * \param env 
	 */
	void SetEnvironmentVariables(EnvironmentVarieblesPtr env);

	/**
	 * get the environment of the workspace
	 * \return 
	 */
	EnvironmentVarieblesPtr GetEnvironmentVariables() const;

	/**
	 * create default new compiler
	 * \return true on success false when a compiler with this name already exist
	 */
	bool CreateDefaultNewCompiler(const wxString &name);

	/**
	 * delete compiler
	 * \param name compiler to delete
	 */
	void DeleteCompiler(const wxString &name);

	/**
	 * Return a list of availanle project templates
	 * \param list output
	 */
	void GetProjectTemplateList(std::list<ProjectPtr> &list);

	/**
	 * Save project as template
	 * \param proj project to duplicate
	 * \param name the template name
	 */
	void SaveProjectTemplate(ProjectPtr proj, const wxString &name);

	/**
	 * Set lite editor's startup directory
	 */
	void SetStarupDirectory(const wxString &path){ m_startupDir = path; }


	/**
	 * Get lite editor's startup directory
	 */
	const wxString &GetStarupDirectory() const { return m_startupDir; }

	/**
	 * Popup project dependencies dialog
	 * \param projectName project name
	 */
	void PopupProjectDependsDlg(const wxString &projectName);

	/**
	 * Pass a command to the compiler thread to clean the 
	 * given project
	 */
	void CleanProject(const wxString &project);
	
	/**
	 * Pass a command to the compiler thread to build the 
	 * given project
	 */
	void BuildProject(const wxString &project);

	/** 
	 * Stop current build process
	 */
	void StopBuild();

	/**
	 * return true if a compilation is in process (either clean or build)
	 */
	bool IsBuildInProgress() const;

	/**
	 * return true a child program is running
	 */
	bool IsProgramRunning() const;

	/**
	 * Kill child program which is running
	 */
	void KillProgram();

	/**
	 * Write line to child program
	 * \param line 
	 */
	void WriteProgram(const wxString &line);

	/** 
	 * Execute the project with no debugger
	 */
	void ExecuteNoDebug(const wxString &projectName);

	// Update the toolbar's configuration choice controls
	void DoUpdateConfigChoiceControl();

	/**
	 * Set the workspace active build configuration name
	 * \param name active configuration name
	 */
	void SetWorkspaceConfigurationName(const wxString &name);

	/**
	 * Return the active's project build configuration that matches 
	 * the workspace selected configuration
	 */
	BuildConfigPtr GetActiveProjectBuildConf();

	/** 
	 * use an external database
	 */
	void SetExternalDatabase(const wxFileName &dbname);

	/** 
	 * close the currenlty open extern database
	 * and free all its resources
	 */
	void CloseExternalDatabase();

	/** 
	 * return list of files that are part of the workspace
	 */
	void GetWorkspaceFiles(std::vector<wxFileName> &files, bool absPath = false);

	/**
	 * check if a file is part of the workspace
	 * \param fileName the file name in absolute path
	 */
	bool IsFileInWorkspace(const wxString &fileName);

	/**
	 * Rebuild the database by removing all entries from the database
	 * that belongs to a given project, and then re-index all files 
	 * \param projectName project to re-tag
	 */
	void RetagProject(const wxString &projectName);

	/**
	 * retag workspace
	 */
	void RetagWorkspace();

	/**
	 * Add debug message
	 * \param msg 
	 */
	void DebugMessage(wxString msg);


	/**
	 * return the project name that 'fullPathFileName' belogs to. if 2 matches were found, return
	 * the first one, or empty string if no match was found
	 */
	wxString GetProjectNameByFile(const wxString &fullPathFileName);

	/** 
	* Import a workspace from a makefile
	*/ 	
	void ImportFromMakefile(const wxString &path);

	/**
	 * Return the active editor or NULL if none
	 */
	LEditor *GetActiveEditor() const;

	/**
	 * add single file to the recently opened files
	 */ 
	void AddToRecentlyOpenedFiles(const wxString &fileName);
	
	/**
	 * return the FileHistory object that holds the recently opened
	 * files data
	 */
	FileHistory &GetRecentlyOpenedFilesClass() {return m_recentFiles;}

	/**
	 * add workspace file to the recently opened workspaces
	 */ 
	void AddToRecentlyOpenedWorkspaces(const wxString &fileName);
	
	/**
	 * return the FileHistory object that holds the recently opened
	 * workspace data
	 */
	FileHistory &GetRecentlyOpenedWorkspacesClass() {return m_recentWorkspaces;}

	/**
	 * Return list of recently opened files from the configuration file
	 * \param files [output]
	 */
	void GetRecentlyOpenedFiles(wxArrayString &files);

	/**
	 * Return list of recently opened workspaces from the configuration file
	 * \param files [output] 
	 */
	void GetRecentlyOpenedWorkspaces(wxArrayString &files);

	/**
	 * insert message to the status bar at a given column
	 * \param msg 
	 * \param col 
	 */
	void SetStatusMessage(const wxString &msg, int col);

	/**
	 * close all opened documents
	 */
	void CloseAll();

protected:
	Manager(void);
	virtual ~Manager(void);
	void OnProcessEnd(wxProcessEvent &event);
	
private:
	void RemoveProjectNameFromOpenFiles(const std::vector<wxFileName> &project_files);

	/**
	 * Update the symbol & file tress
	 */
	void RebuildFileView();
	
	/**
	 * Remove a file from the gui tree
	 */
	void RemoveFileFromSymbolTree(const wxFileName &fileName, const wxString &project);

	// Create environment variables for the workspace
	void CreateEnvironmentVars(const wxString &wpsPath);
};

typedef Singleton<Manager> ManagerST;

#endif // MANAGER_H

