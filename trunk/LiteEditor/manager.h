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
	 * Return the installation path of CodeLite
	 * 
	 */
	const wxString &GetInstallPath() const;
	
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

protected:
	Manager(void);
	virtual ~Manager(void);
};

typedef Singleton<Manager> ManagerST;

#endif // MANAGER_H

