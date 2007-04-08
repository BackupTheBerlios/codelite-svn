#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "singleton.h"
#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"
#include "project.h"
#include <map>

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
	bool CreateWorkspace(const wxString &name, const wxString &path);

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
	bool OpenWorkspace(const wxString &fileName);

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
	bool CreateProject(const wxString &name, const wxString &path, const wxString &type);

	/**
	 * \brief get a string property from the worksapce file
	 * \returns property value or wxEmptyString 
	 */
	wxString GetStringProperty(const wxString &propName);
};

typedef Singleton<Workspace> WorkspaceST;

#endif // WORKSPACE_H
