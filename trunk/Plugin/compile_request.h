#ifndef COMPILE_REQUEST_H
#define COMPILE_REQUEST_H

#include "compiler_action.h"
#include "wx/timer.h"

class CompileReqeust : public wxEvtHandler, public CompilerAction {
	wxString m_project;
	wxTimer *m_timer;

public:
	/**
	 * Construct a compilation request. The compiler thread will build the selected project and all
	 * its dependencies as appeard in the build order dialog
	 * \param projectName the selected project to build
	 * \param configurationName the workspace selected configuration
	 */
	CompileReqeust(const wxString &projectName);

	///dtor
	virtual ~CompileReqeust();

	//process the request
	virtual void Process();
};

#endif // COMPILE_REQUEST_H
