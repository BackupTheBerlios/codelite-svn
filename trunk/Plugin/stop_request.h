#ifndef STOP_REQUEST_H
#define STOP_REQUEST_H
#include "compiler_action.h"

class StopRequest : public CompilerAction {
public:
	/**
	 * Construct a compilation stop request
	 */
	StopRequest();

	///dtor
	virtual ~StopRequest();

	//process the request
	virtual void Process();

};

#endif // STOP_REQUEST_H
