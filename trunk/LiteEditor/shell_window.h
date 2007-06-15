#ifndef SHELL_WINDOW_H
#define SHELL_WINDOW_H

#include "wx/wxscintilla.h"

class ShellWindow : public wxScintilla 
{
	DECLARE_EVENT_TABLE()
public:
	ShellWindow(wxWindow *parent);
	virtual ~ShellWindow();

	void AppendLine(const wxString &text);
	void Clear();
	void OnCharAdded(wxScintillaEvent& event);
};
#endif // SHELL_WINDOW_H
