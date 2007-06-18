///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __open_resouce_dlg__
#define __open_resouce_dlg__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "vector"
#include "wx/filename.h"
#include "wx/timer.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OpenResourceDlg
///////////////////////////////////////////////////////////////////////////////
class OpenResourceDlg : public wxDialog 
{
	std::vector<wxFileName> m_files;
	wxTimer *m_timer;

protected:
	void OnButtonOK(wxCommandEvent &event);
	void OnButtonCancel(wxCommandEvent &event);
	void OnItemActivated(wxCommandEvent &event);
	void ConnectEvents();
	bool DoOpenFile();
	void OnTimer(wxTimerEvent &event);
	void GetFilesAsWxArray(wxArrayString &files);
	void OnComboEnter(wxCommandEvent &event);

protected:
	wxPanel* mainPanel;
	wxStaticText* m_staticTitle;
	wxComboBox* m_comboResourceName;
	wxListBox* m_listShortNames;
	wxStaticLine* m_staticline1;
	wxButton* m_btnOk;
	wxButton* m_button2;

public:
	OpenResourceDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Open Resource:"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 653,393 ), int style = wxDEFAULT_DIALOG_STYLE );
	virtual ~OpenResourceDlg();

};

#endif //__open_resouce_dlg__
