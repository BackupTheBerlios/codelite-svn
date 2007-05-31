#ifndef COMPILER_THREAD_H
#define COMPILER_THREAD_H

#include "singleton.h"
#include "worker_thread.h"
#include "wx/string.h"
class CompilerThread;
class CompilerThread : public WorkerThread {
	friend class Singleton<CompilerThread>;

private:
	//ctor
	CompilerThread();
	//dtor
	virtual ~CompilerThread();

public:
	virtual void ProcessRequest(ThreadRequest *request);
};

typedef Singleton<CompilerThread> CompilerThreadST;
#endif // COMPILER_THREAD_H

