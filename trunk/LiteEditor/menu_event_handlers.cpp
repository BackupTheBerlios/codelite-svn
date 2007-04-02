#include "menu_event_handlers.h"
#include "editor.h"

//------------------------------------
// Handle copy events
//------------------------------------
void CopyHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->Copy();
}

void CopyHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 ));
}

//------------------------------------
// Handle cut events
//------------------------------------
void CutHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->Cut();
}

void CutHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 ));
}

//------------------------------------
// Handle paste events
//------------------------------------
void PasteHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->Paste();
}

void PasteHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(editor->CanPaste());
}

//------------------------------------
// Undo
//------------------------------------
void UndoHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->Undo();
}

void UndoHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(editor->CanUndo());
}

//------------------------------------
// Redo
//------------------------------------
void RedoHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->Redo();
}

void RedoHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(editor->CanRedo());
}

//------------------------------------
// SelectAll
//------------------------------------
void SelectAllHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->SelectAll();
}

void SelectAllHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(editor->GetLength() > 0);
}

//------------------------------------
// duplicate line
//------------------------------------
void DuplicateLineHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);
	editor->LineDuplicate();
}

void DuplicateLineHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(owner);
	event.Enable(true);
}

//------------------------------------
// brace matching
//------------------------------------
void BraceMatchHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);

	if(event.GetId() == XRCID("select_to_brace")){
		editor->MatchBraceAndSelect(true);
	} else {
		editor->MatchBraceAndSelect(false);
	}
}

void BraceMatchHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	LEditor *editor = static_cast<LEditor*>(owner);
	event.Enable(editor->GetLength() > 0);
}
