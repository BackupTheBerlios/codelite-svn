#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "singleton.h"
#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"
#include "project.h"
#include <map>
#include "ctags_manager.h"
#include "configuration_mapping.h"

/*!
 * \brief
 * Workspace manager class
 * 
 */
class Workspace 
{
	friend class Singleton<Workspace>;
	wxXmlDocument m_doc;
	wxFileName m_fileName;
	std::map<wxString, ProjectPtr> m_projects;

private:

	/// Constructor
	Workspace();

	/// Destructor
	virtual ~Workspace();

public:
	/**
	 * Returns the workspace file name
	 */
	const wxFileName& GetWorkspaceFileName() const { return m_fileName; }

	/**
	 * \brief
	 * Create a new workspace
	 * 
	 * \param name
	 * workspace name
	 * 
	 * \param path
	 * workspace path
	 *
	 * \returns
	 * true on success false otherwise
	 */
	bool CreateWorkspace(const wxString &name, const wxString &path, const CtagsOptions &options, wxString &errMsg);

	/**
	 * \brief
	 * Open an existing workspace
	 * 
	 * \param fileName
	 * Workspace file name (including extesion)
	 * 
	 * \returns
	 * true on success false otherwise
	 */
	bool OpenWorkspace(const wxString &fileName, wxString &errMsg);

	/**
	 * \brief
	 * Create a new project with given name, path and type
	 * 
	 * \param name
	 * project name
	 * 
	 * \param path
	 * project path
	 *
	 * \param type
	 * Can be Project::EXECUTABLE, Project::STATIC_LIBRARY or Project::DYNAMIC_LIBRARY
	 * \returns
	 * true on success false otherwise
	 */
	bool CreateProject(const wxString &name, const wxString &path, const wxString &type, wxString &errMsg);

	/**
	 * \brief get a string property from the worksapce file
	 * \returns property value or wxEmptyString 
	 */
	wxString GetStringProperty(const wxString &propName, wxString &errMsg);

	/**
	 * Find a project by name
	 * \param projName project name
	 * \param errMsg [output] incase an error, report the error to the caller
	 * \returns ProjectPtr or NULL if no match was found
	 */
	ProjectPtr FindProjectByName(const wxString &projName, wxString &errMsg) const;

	/**
	 * Return all project names under this workspace
	 */
	void GetProjectList(wxArrayString &list);
	
	/**
	 * Add an existing project to the workspace. If no workspace is open,
	 * this function does nothing
	 * \param path project file path name to add
	 * \param errMsg [output] incase an error, report the error to the caller
	 */
	bool AddProject(const wxString & path, wxString &errMsg);

	/**
	 * Remove project from the workspace. This function does not delete
	 * any file related to the project but simply removes it from the workspace
	 * \param name project name to remove
	 * \param errMsg [output] incase an error, report the error to the caller
	 * \return true on success false otherwise
	 */
	bool RemoveProject(const wxString &name, wxString &errMsg);

	/**
	 * \return The active project name or wxEmptyString
	 */
	wxString GetActiveProjectName();

	/**
	 * Set project as active
	 * \param name  project name
	 * \param active state
	 */
	void SetActiveProject(const wxString &name, bool active);

	/**
	 * Add new virtual directoy to workspace
	 * \param vdFullPath virtual directory full path
	 * \param errMsg [output] incase an error, report the error to the caller
	 * \return true on success false otherwise
	 */
	bool CreateVirtualDirectory(const wxString &vdFullPath, wxString &errMsg);

	/**
	 * Remove virtual directoy to workspace
	 * \param vdFullPath virtual directory full path
	 * \param errMsg [output] incase an error, report the error to the caller
	 * \return true on success false otherwise
	 */
	bool RemoveVirtualDirectory(const wxString &vdFullPath, wxString &errMsg);

	/**
	 * Add new file to project. The project name is taken from the virtual directory full path
	 * \param vdFullPath vritual directory full path including project
	 * \param fileName file name to add
	 * \param errMsg output
	 * \return true on success, false otherwise
	 */
	bool AddNewFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg);

	/**
	 * Remove file from a project. The project name is taken from the virtual directory full path
	 * \param vdFullPath vritual directory full path including project
	 * \param fileName file name to remove
	 * \param errMsg output
	 * \return true on success, false otherwise
	 */
	bool RemoveFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg);

	/**
	 * Save workspace & projects settings
	 */
	void Save();

	/**
	 * Load ctags options from the configuration
	 */
	CtagsOptions LoadCtagsOptions() const;

	/**
	 * Save ctags options to the configuration
	 */
	void SaveCtagsOptions(const CtagsOptions &options);

	/**
	 * Return the configuration mapping for the workspace. 'Configuration Mapping' is 
	 * the build matrix that should be used for this workspace, it contains all possible
	 * configurations and which project specific configuration should be built in that
	 * configuration
	 */
	ConfigurationMappingPtr GetConfigurationMapping() const;

	/**
	 * Store configuration mapping for the workspace, overriding the current one
	 * \param mapping new mapping to store
	 */
	void SetConfigurationMapping(ConfigurationMappingPtr mapping);

private:
	/**
	 * Do the actual add project 
	 * \param path project file path
	 * \param errMsg [output] incase an error, report the error to the caller
	 */
	bool DoAddProject(const wxString &path, wxString &errMsg);
};

typedef Singleton<Workspace> WorkspaceST;

#endif // WORKSPACE_H
