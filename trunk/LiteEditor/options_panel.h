///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __options_panel__
#define __options_panel__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/choice.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class OptionsPanel
///////////////////////////////////////////////////////////////////////////////
class OptionsPanel : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_macros;
		wxButton* m_addButton;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_fileTypes;
		wxStaticText* m_staticText4;
		wxChoice* m_languages;
		wxCheckBox* m_parseComments;
	
	public:
		OptionsPanel( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,206 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__options_panel__
