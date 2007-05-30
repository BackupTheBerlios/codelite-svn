#ifndef CLEAN_REQUEST_H
#define CLEAN_REQUEST_H
#include "compiler_action.h"

class CleanRequest : public CompilerAction {
	wxString m_project;
	wxString m_configuration;

public:
	/**
	 * Construct a compilation clean request. The compiler thread will clean the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CleanRequest(const wxString &projectName, const wxString &configurationName);

	///dtor
	virtual ~CleanRequest();

	//process the request
	virtual void Process();

	//setters/getters
	const wxString &GetProjectName() const { return m_project; }
	const wxString &GetConfigurationName() const { return m_configuration; }
};

#endif // CLEAN_REQUEST_H
