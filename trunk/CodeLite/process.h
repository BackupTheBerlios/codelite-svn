#ifndef CODELITE_TAGSPROCESS_H
#define CODELITE_TAGSPROCESS_H

#include <wx/process.h>
#include "smart_ptr.h"

/**
 * \ingroup CodeLite
 * \brief this class represents a cross platform process
 *
 * \version 1.0
 * first version
 *
 * \date 05-04-2007
 *
 * \author Eran
 */
class clProcess : public wxProcess
{
	long m_pid;
	int m_id;
public:
	/**
	 * Constructs a process object. id is only used in the case you want to use wxWidgets events. 
	 * It identifies this object, or another window that will receive the event.
	 * If the evtHandler parameter is different from NULL, 
	 * it will receive a wxEVT_END_PROCESS notification event (you should insert EVT_END_PROCESS macro in the event table of the parent to handle it) 
	 * with the given id.
	 */
	clProcess(wxEvtHandler* evtHandler, int id);

	virtual ~clProcess();

	/**
	 * \return return the process id
	 */
	long GetPid();

	/**
	 * \param set the process ID
	 */
	void SetPid(long pid);

	int GetId() { return m_id; }

	/**
	 * Kill the process
	 */
	void Terminate();

	void OnTerminate() {}
};

typedef SmartPtr<clProcess> clProcessPtr;

#endif // CODELITE_TAGSPROCESS_H
