#ifndef CODELITE_TAGSPROCESS_H
#define CODELITE_TAGSPROCESS_H

#include "wx/process.h"
#include "wx/string.h"

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE

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
class WXDLLIMPEXP_CL clProcess : public wxProcess
{
	long m_pid;
	int m_uid;
	int m_type;
	wxString m_cmd;

public:
	/**
	 * Constructs a process object. id is only used in the case you want to use wxWidgets events. 
	 * It identifies this object, or another window that will receive the event.
	 */
	clProcess(int id, const wxString &cmdLine);

	virtual ~clProcess();

	/**
	 * \return return the process id
	 */
	long GetPid();

	/**
	 * \param set the process ID
	 */
	void SetPid(long pid);

	//int GetId() { return m_id; } 

	/**
	 * Kill the process
	 */
	void Terminate();

	/** 
	 * Start the process
	 * \return the process id
	 */
	long Start(bool hide = true);

	int GetUid() { return m_uid; }
	void SetType(int type) { m_type = type; }
	int GetType() const { return m_type; }

	bool HasInput(wxString &input, wxString &errors);
};

#endif // CODELITE_TAGSPROCESS_H
