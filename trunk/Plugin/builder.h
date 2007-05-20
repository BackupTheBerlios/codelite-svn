#ifndef BUILDER_H
#define BUILDER_H

#include "wx/string.h"
#include "smart_ptr.h"
#include "wx/event.h"

/**
 * \ingroup SDK
 * this class defines the interface of a build system
 *
 * \version 1.0
 * first version
 *
 * \date 05-19-2007
 *
 * \author Eran
 */
class Builder {
protected:
	wxString m_name;

public:
	Builder(const wxString &name) ;
	virtual ~Builder();

	/**
	 * \return the builder name 
	 */
	const wxString &GetName() const { return m_name; }
	
	/**
	 * Build the workspace selected configuration.
	 * \param target target configuration to build
	 * \return true on success false otherwise
	 */
	virtual bool BuildWorkspace(const wxString &target) = 0;

	/**
	 * Build a single project. The project dependencies are built as well
	 * \param project project name to build
	 * \param target target configuration to build
	 * \return true on success, false otherwise
	 */
	virtual bool BuildProject(const wxString &project, const wxString &target) = 0;

	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. If left empty, the file is exported for the whole
	 *			workspace. Anyways, the file will always include its dependencies
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project) = 0;
};

typedef SmartPtr<Builder> BuilderPtr;

#endif // BUILDER_H
