#include "compiler_thread.h"
#include "compiler_action.h"

CompilerThread::CompilerThread()
{
}

CompilerThread::~CompilerThread()
{
}

void CompilerThread::ProcessRequest(ThreadRequest *request)
{
	CompilerAction *req = dynamic_cast<CompilerAction *>(request);
	if(req){
		req->Process();
	}
}


