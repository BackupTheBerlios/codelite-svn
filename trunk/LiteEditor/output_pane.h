#ifndef OUTPUT_PANE_H
#define OUTPUT_PANE_H


#include "wx/panel.h"
#include "wx/wxscintilla.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"

class OutputPane;
class ShellWindow;

/**
 * \ingroup LiteEditor
 * This class represents the default bottom pane in the editor
 *
 * \date 04-14-2007
 *
 * \author Eran
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \todo 
 *
 * \bug 
 *
 */
class OutputPane : public wxPanel 
{
public:
	static const wxString FIND_IN_FILES_WIN;
	static const wxString BUILD_WIN;
	static const wxString OUTPUT_WIN;
	static const wxString OUTPUT_DEBUG;

	wxFlatNotebook *m_book;
	wxString m_caption;
	bool m_canFocus;
	wxFlatNotebookImageList m_images;
	wxArrayString m_buildLineInfo;
	wxString m_project;
	ShellWindow *m_outputWind;

private:
	void CreateGUIControls();
	wxScintilla *CreateScintillaPage();
	void OnFindInFilesDClick(const wxString &line);
	void OnBuildWindowDClick(const wxString &line, int lineno);

public:
	/**
	 * Return the index of a given window by its caption
	 */
	int CaptionToIndex(const wxString &caption);

	/**
	 * Destructor
	 * \param parent parent window for this pane
	 * \param caption the caption
	 */
	OutputPane(wxWindow *parent, const wxString &caption);

	/**
	 * Destructor
	 */
	virtual ~OutputPane();

	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void AppendText(const wxString &winName, const wxString &text);
	void Clear();

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxFlatNotebook *GetNotebook() { return m_book; }
	const wxString &GetCaption() const { return m_caption; }
	void CanFocus(bool can) { m_canFocus = can; }

	//-----------------------------------------------
	// Event handlers
	//-----------------------------------------------
	void OnClearAll(wxCommandEvent &event);
	void OnWordWrap(wxCommandEvent &event);
	void OnSetFocus(wxFocusEvent &event);
	void OnMouseDClick(wxScintillaEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnEraseBg(wxEraseEvent &){};
};

#endif // OUTPUT_PANE_H
