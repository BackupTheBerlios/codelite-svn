#ifndef COMPILE_REQUEST_H
#define COMPILE_REQUEST_H

#include "compiler_action.h"

class CompileReqeust : public CompilerAction {
	wxString m_project;
	wxString m_configuration;

public:
	/**
	 * Construct a compilation request. The compiler thread will build the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CompileReqeust(const wxString &projectName, const wxString &configurationName);

	///dtor
	virtual ~CompileReqeust();

	//process the request
	virtual void Process();

	//setters/getters
	const wxString &GetProjectName() const { return m_project; }
	const wxString &GetConfigurationName() const { return m_configuration; }

};

#endif // COMPILE_REQUEST_H
