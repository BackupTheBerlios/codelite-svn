#ifndef COMPILE_REQUEST_H
#define COMPILE_REQUEST_H

#include "compiler_action.h"

class CompileRequest : public CompilerAction {
	wxString m_project;

public:
	/**
	 * Construct a compilation request. The compiler thread will build the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CompileRequest(wxEvtHandler *owner, const wxString &projectName);

	///dtor
	virtual ~CompileRequest();

	//process the request
	virtual void Process();

	//setters/getters
	const wxString &GetProjectName() const { return m_project; }
};

#endif // COMPILE_REQUEST_H
