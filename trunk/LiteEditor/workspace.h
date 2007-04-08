#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "singleton.h"
#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"

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
	 * \param name
	 * Workspace name
	 * 
	 * \param path
	 * Workspace location 
	 * 
	 * \returns
	 * true on success false otherwise
	 */
	bool OpenWorkspace(const wxString &name, const wxString &path);
};

typedef Singleton<Workspace> WorkspaceST;

#endif // WORKSPACE_H
