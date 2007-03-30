#ifndef MANAGER_H
#define MANAGER_H

#include "singleton.h"

class Manager
{
	friend class Singleton<Manager>;
public:
	bool IsWorkspaceOpen() const;

private:
	Manager(void);
	virtual ~Manager(void);
};

typedef Singleton<Manager> ManagerST;
#endif // MANAGER_H