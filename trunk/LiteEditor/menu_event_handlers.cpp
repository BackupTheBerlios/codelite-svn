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


//------------------------------------
// Find / Replace
//------------------------------------
void FindReplaceHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);

	if( event.GetId() == wxID_FIND ) 
	{
		editor->DoFindAndReplace();
	}
	else 
	{
		FindReplaceDialog *dlg = editor->GetFindReplaceDialog();
		if(event.GetId() == XRCID("find_next") && dlg)
		{
			FindReplaceData data = dlg->GetData();
			// set search direction down
			data.SetFlags(data.GetFlags() & ~(wxFRD_SEARCHUP));
			editor->FindNext( data );
		}
		else if( event.GetId() == XRCID("find_previous") && dlg)
		{
			FindReplaceData data = dlg->GetData();
			// set search direction up
			data.SetFlags(data.GetFlags() | wxFRD_SEARCHUP);
			editor->FindNext( data );
		}
	}
}

void FindReplaceHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(event);
	wxUnusedVar(owner);
}

//----------------------------------
// goto linenumber
//----------------------------------

void GotoHandler::ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event)
{
	wxUnusedVar(event);
	LEditor *editor = static_cast<LEditor*>(owner);

	wxString msg;
	msg.Printf("Go to line number (1 - %ld):", editor->GetLineCount()); 

	while( 1 ) 
	{
		wxTextEntryDialog dlg(editor, msg, wxT("Go To Line"));
		if(dlg.ShowModal() == wxID_OK)
		{
			wxString val = dlg.GetValue();
			long line;
			if(!val.ToLong(&line))
			{
				wxString err;
				err.Printf("%s is not a valid line number", val.GetData());
				wxMessageBox (err, _("Go To Line"), wxOK | wxICON_INFORMATION);
				continue;
			}

			if(line > editor->GetLineCount())
			{
				wxString err;
				err.Printf("Please insert a line number in the range of (1 - %ld)", editor->GetLineCount());
				wxMessageBox (err, _("Go To Line"), wxOK | wxICON_INFORMATION);
				continue;
			}

			if(line > 0)
			{
				editor->GotoLine(line - 1);
				break;
			}
			else
			{
				editor->GotoLine(0);
				break;
			}
		}
		else
		{
			// wxID_CANCEL
			return;
		}
	}

	editor->SetSCIFocus(true);
    editor->SetFocus ();
}

void GotoHandler::ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event)
{
	wxUnusedVar(event);
	wxUnusedVar(owner);
}
