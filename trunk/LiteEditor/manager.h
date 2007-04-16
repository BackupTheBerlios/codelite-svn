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

class wxFrame;

/*!
 * \brief
 * Manager class, a global class that provides access to many 
 * commands of CodeLite
 */
class Manager 
{
	friend class Singleton<Manager>;

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
	 * Open a file using TagEntry as input
	 * 
	 * \param tag TagEntry 
	 */
	void OpenFile(const TagEntry &tag);

	/*!
	 * \brief
	 * Open a file using file name and line number
	 * 
	 * \param fileName full path of the file name
	 * \param projectName project name, can be wxEmptyString
	 * \param lineno the cursor will be placed at lineno
	 */
	void OpenFile(const wxString &file_name, const wxString &projectName, const int lineno);

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
	void SaveAll();

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
	void CreateWorkspace(const wxString& name, const wxString &path);
	 
	/**
	 * \brief create an empty project
	 * \param name project name
	 * \param path project file path
	 * \param type project type, Project::STATIC_LIBRARY, Project::DYNAMIC_LIBRARY or Project::EXECUTABLE
	 */
	void CreateProject(const wxString &name, const wxString &path, const wxString &type);

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

protected:
	Manager(void);
	virtual ~Manager(void);

private:
	/**
	 * Update the symbol & file tress
	 */
	void DoUpdateGUITrees();
};

typedef Singleton<Manager> ManagerST;

#endif // MANAGER_H

