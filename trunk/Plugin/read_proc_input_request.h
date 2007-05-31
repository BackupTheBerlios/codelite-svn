#ifndef READ_PROC_REQUEST_H
#define READ_PROC_REQUEST_H

#include "worker_thread.h"
#include "wx/process.h"
#include <wx/txtstrm.h>

class ReadProcInputRequest : public ThreadRequest {
	wxProcess *m_proc;
	wxInputStream &m_in, &m_err;

public:
	ReadProcInputRequest(wxProcess *proc) 
		: m_proc(proc)
		, m_in(*proc->GetInputStream())
		, m_err(*proc->GetErrorStream())
	{
	};
	
	virtual ~ReadProcInputRequest()
	{};

	void Process();
};
#endif //READ_PROC_REQUEST_H

