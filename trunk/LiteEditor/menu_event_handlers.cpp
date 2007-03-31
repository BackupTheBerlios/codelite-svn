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
	if( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 )
		event.Enable(true);
	else
		event.Enable(false);
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
	if( editor->GetSelectionStart() - editor->GetSelectionEnd() != 0 )
		event.Enable(true);
	else
		event.Enable(false);
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
	if(editor->CanPaste())
		event.Enable(true);
	else
		event.Enable(false);
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
	if(editor->CanUndo())
		event.Enable(true);
	else
		event.Enable(false);
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
	if(editor->CanRedo())
		event.Enable(true);
	else
		event.Enable(false);
}
