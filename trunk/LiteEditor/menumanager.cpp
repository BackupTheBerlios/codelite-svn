#include "menumanager.h"
#include "menu_event_handlers.h"

MenuManager::MenuManager(void)
{
	PushHandler(new EditHandler(wxID_COPY));
	PushHandler(new EditHandler(wxID_CUT));
	PushHandler(new EditHandler(wxID_PASTE));
	PushHandler(new EditHandler(wxID_UNDO));
	PushHandler(new EditHandler(wxID_REDO));
	PushHandler(new EditHandler(wxID_SELECTALL));
	PushHandler(new EditHandler(wxID_DUPLICATE));
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
	PushHandler(new ViewAsHandler(XRCID("view_as_cpp")));
	PushHandler(new ViewAsHandler(XRCID("view_as_text")));
	PushHandler(new WordWrapHandler(XRCID("word_wrap")));
}

MenuManager::~MenuManager(void)
{
}
