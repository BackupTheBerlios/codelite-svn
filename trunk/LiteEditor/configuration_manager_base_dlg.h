///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __configuration_manager_base_dlg__
#define __configuration_manager_base_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/button.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ConfigManagerBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ConfigManagerBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText2;
		wxChoice* m_choiceConfigurations;
		wxButton* m_buttonNewConfiguration;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText20;
		wxScrolledWindow* m_scrolledWindow;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
	
	public:
		ConfigManagerBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Configuration Manager"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 726,425 ), int style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	
};

#endif //__configuration_manager_base_dlg__
