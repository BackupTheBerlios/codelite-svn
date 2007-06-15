#ifndef SHELL_WINDOW_H
#define SHELL_WINDOW_H

#include "wx/wxscintilla.h"

class ShellWindow : public wxScintilla 
{
protected:
	void ConnectEvents();

public:
	ShellWindow(wxWindow *parent);
	virtual ~ShellWindow();

	void AppendLine(const wxString &text);
	void Clear();
	void OnKeyDown(wxKeyEvent &event);
};
#endif // SHELL_WINDOW_H
