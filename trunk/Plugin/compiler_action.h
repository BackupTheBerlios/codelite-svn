#ifndef COMPILER_ACTION_H
#define COMPILER_ACTION_H

#include "worker_thread.h"

class CompilerAction : public ThreadRequest {
public:
	virtual void Process() = 0;
};
#endif 
