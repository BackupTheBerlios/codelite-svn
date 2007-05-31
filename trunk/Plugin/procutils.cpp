#include "procutils.h"

clProcess *ProcUtils::ExecuteProcess(const wxString &cmd)
{
	clProcess *proc = new clProcess(wxNewId(), cmd);
	proc->Start();
	return proc;
}
