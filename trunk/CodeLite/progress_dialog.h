///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __progress_dialog__
#define __progress_dialog__

#include <wx/wx.h>
#include <wx/progdlg.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProgressDialog
///////////////////////////////////////////////////////////////////////////////
class ProgressDialog : public wxProgressDialog {
public:
	ProgressDialog(const wxString &title, const wxString &message, int maximum, wxWindow *parent);
	virtual ~ProgressDialog();
};

#endif //__progress_dialog__
