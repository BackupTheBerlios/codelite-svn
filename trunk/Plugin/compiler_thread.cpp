#include "compiler_thread.h"
#include "read_proc_input_request.h"

CompilerThread::CompilerThread()
{
}

CompilerThread::~CompilerThread()
{
}

void CompilerThread::ProcessRequest(ThreadRequest *request)
{
	ReadProcInputRequest *req = dynamic_cast<ReadProcInputRequest*>(request);
	if(req){
		req->Process();
	}
}


