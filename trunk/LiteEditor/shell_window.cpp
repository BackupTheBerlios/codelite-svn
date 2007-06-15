#include "shell_window.h"
#include "manager.h"
#include "macros.h"

BEGIN_EVENT_TABLE(ShellWindow, wxScintilla)
EVT_SCI_CHARADDED(wxID_ANY, ShellWindow::OnCharAdded)
END_EVENT_TABLE()

ShellWindow::ShellWindow(wxWindow *parent)
: wxScintilla(parent)
{
	int caretSlop = 1;
	int caretZone = 20;
	int caretStrict = 0;
	int caretEven = 0;
	int caretJumps = 0;

	SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	caretSlop = 1;
	caretZone = 1;
	caretStrict = 4;
	caretEven = 8;
	caretJumps = 0;
	SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

	SetMarginWidth(1, 0);
	SetMarginWidth(2, 0);

	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	StyleSetFont(wxSCI_STYLE_DEFAULT, font);
}

ShellWindow::~ShellWindow()
{

}

void ShellWindow::AppendLine(const wxString &text)
{
	AddText( text );						
	// the next 4 lines make sure that the caret is at last line
	// and is visible
	SetSelectionEnd(GetLength());
	SetSelectionStart(GetLength());
	SetCurrentPos(GetLength());
	EnsureCaretVisible();
}

void ShellWindow::Clear()
{
	ClearAll();
}

void ShellWindow::OnCharAdded(wxScintillaEvent &event)
{
	switch(event.GetKey())
	{
	case wxT('\n'):
		{
			wxString line = GetCurLine();
			ManagerST::Get()->WriteProgram(line);
		}
		break;
	default:
		break;
	}
	event.Skip();
}
