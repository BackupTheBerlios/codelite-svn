#ifndef DIRPICKER_H
#define DIRPICKER_H

#include "wx/panel.h"
#include "wx/button.h"
#include "wx/textctrl.h"

/**
 * This control allows the user to select a directory. with a text control on its right side
 */
class DirPicker : public wxPanel {
	wxTextCtrl *m_path;
	wxButton *m_button;
	wxString m_buttonCaption;
	wxString m_dlgCaption;

private:
	void CreateControls();
	void ConnectEvents();

protected:
	/**
	 * Handle button click event
	 * \param &event 
	 */
	virtual void OnButtonClicked(wxCommandEvent &event);
	

public:
	DirPicker(wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &message = wxT("Select a folder:"), const wxString &buttonCaption = wxT("Browse"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~DirPicker();

	/**
	 * \return the path
	 */
	wxString GetPath() const { return m_path->GetValue(); }

	/**
	 * set a value into the text control
	 * \param path path to set
	 */
	void SetPath(const wxString &path) { m_path->SetValue(path); }
};

#endif // DIRPICKER_H
