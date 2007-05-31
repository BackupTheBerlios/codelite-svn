#ifndef PROCUTILS_H
#define PROCUTILS_H

#include "wx/string.h"
#include "cl_process.h"

class ProcUtils
{
public:
	static clProcess *ExecuteProcess(const wxString &cmd);
};
#endif // PROCUTILS_H
