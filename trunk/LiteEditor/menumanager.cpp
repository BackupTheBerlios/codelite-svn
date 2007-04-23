#include "menumanager.h"

MenuManager::MenuManager(void)
{
	PushHandler(new CopyHandler(wxID_COPY));
	PushHandler(new CutHandler(wxID_CUT));
	PushHandler(new PasteHandler(wxID_PASTE));
	PushHandler(new UndoHandler(wxID_UNDO));
	PushHandler(new RedoHandler(wxID_REDO));
	PushHandler(new SelectAllHandler(wxID_SELECTALL));
	PushHandler(new DuplicateLineHandler(wxID_DUPLICATE));
	PushHandler(new BraceMatchHandler(XRCID("select_to_brace")));
	PushHandler(new BraceMatchHandler(XRCID("match_brace")));
	PushHandler(new FindReplaceHandler(wxID_FIND));
	PushHandler(new FindReplaceHandler(XRCID("find_next")));
	PushHandler(new FindReplaceHandler(XRCID("find_previous")));
	PushHandler(new GotoHandler(XRCID("goto_linenumber")));
	PushHandler(new BookmarkHandler(XRCID("toggle_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("next_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("previous_bookmark")));
	PushHandler(new BookmarkHandler(XRCID("removeall_bookmarks")));
	PushHandler(new GotoDefinitionHandler(XRCID("goto_definition")));
	PushHandler(new GotoDefinitionHandler(XRCID("goto_previous_definition")));
}

MenuManager::~MenuManager(void)
{
}
