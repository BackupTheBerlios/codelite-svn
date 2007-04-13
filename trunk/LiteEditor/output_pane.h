#ifndef OUTPUT_PANE_H
#define OUTPUT_PANE_H

#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "wx/panel.h"

class OutputPane : public wxPanel 
{
public:
	static const wxString FIND_IN_FILES_WIN;
	static const wxString BUILD_WIN;

	wxFlatNotebook *m_book;
	wxString m_caption;

private:
	void CreateGUIControls();
	int CaptionToIndex(const wxString &caption);

public:
	OutputPane(wxWindow *parent, const wxString &caption);
	virtual ~OutputPane();

	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void AppendText(const wxString &winName, const wxString &text);

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxFlatNotebook *GetNotebook() { return m_book; }
	const wxString &GetCaption() const { return m_caption; }
};

#endif // OUTPUT_PANE_H
