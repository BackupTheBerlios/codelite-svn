#include "menumanager.h"

MenuManager::MenuManager(void)
{
	PushHandler(new CopyHandler());
	PushHandler(new CutHandler());
	PushHandler(new PasteHandler());
	PushHandler(new UndoHandler());
	PushHandler(new RedoHandler());
}

MenuManager::~MenuManager(void)
{
}
