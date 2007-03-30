#include "manager.h"
#include "ctags_manager.h"

Manager::Manager(void)
{
}

Manager::~Manager(void)
{
}

bool Manager::IsWorkspaceOpen() const 
{
	return TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().IsOk();
}
