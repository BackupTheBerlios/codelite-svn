///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __depend_dlg_page__
#define __depend_dlg_page__

#include <wx/wx.h>

#include <wx/checklst.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DependenciesPage
///////////////////////////////////////////////////////////////////////////////
class DependenciesPage : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxCheckListBox* m_checkListProjectList;
		wxStaticText* m_staticText2;
		wxListBox* m_listBoxBuildOrder;
	
	public:
		DependenciesPage( wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );
	
};

#endif //__depend_dlg_page__
