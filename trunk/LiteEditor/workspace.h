#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "singleton.h"
#include "wx/string.h"

/*!
 * \brief
 * Workspace manager class
 * 
 */
class Workspace 
{
private:

	/// Constructor
	Workspace();

	/// Destructor
	virtual ~Workspace();

public:

	/*!
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

	/*!
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
#endif // WORKSPACE_H
