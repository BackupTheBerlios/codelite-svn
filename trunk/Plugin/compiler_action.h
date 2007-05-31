#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "worker_thread.h"
#include "wx/event.h"
#include "cl_process.h"

class CompilerAction : public ThreadRequest {
protected:
	clProcess *m_proc;
public:
	CompilerAction() : m_proc(NULL)
	{};
	
	virtual ~CompilerAction()
	{};
	
	virtual void Process() = 0;
};
#endif 
