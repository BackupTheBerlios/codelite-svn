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
	 * Normalize the configuration name, this is done by removing any trailing and leading 
	 * spaces from the string, and replacing any space character with underscore.
	 */
	static wxString NormalizeConfigName(const wxString &confgName);

	/**
	 * \return the builder name 
	 */
	const wxString &GetName() const { return m_name; }
	
	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. 
	 * \param errMsg output
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project, wxString &errMsg) = 0;

	/** 
	 * Return the command that should be executed for performing the clean
	 * task
	 */
	virtual wxString GetCleanCommand(const wxString &project) = 0;

	/** 
	 * Return the command that should be executed for performing the build
	 * task for a given project
	 */
	virtual wxString GetBuildCommand(const wxString &project) = 0;
};

typedef SmartPtr<Builder> BuilderPtr;

#endif // BUILDER_H
