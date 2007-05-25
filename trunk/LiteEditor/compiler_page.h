///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __compiler_page__
#define __compiler_page__

#include <wx/wx.h>

#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class CompilerPage
///////////////////////////////////////////////////////////////////////////////
class CompilerPage : public wxPanel 
{
protected:
	wxStaticText* m_staticText5;
	wxTextCtrl* m_textErrorPattern;
	wxStaticText* m_staticText6;
	wxTextCtrl* m_textErrorFileIndex;
	wxStaticText* m_staticText7;
	wxTextCtrl* m_textErrorLineNumber;
	wxStaticText* m_staticText51;
	wxTextCtrl* m_textWarnPattern;
	wxStaticText* m_staticText61;
	wxTextCtrl* m_textWarnFileIndex;
	wxStaticText* m_staticText71;
	wxTextCtrl* m_textWarnLineNumber;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textObjectExtension;
	wxListCtrl* m_listSwitches;

	wxString m_selSwitchName ;
	wxString m_selSwitchValue;
	wxString m_cmpname;

	// Virtual event handlers, overide them in your derived class
	virtual void OnItemActivated( wxListEvent& event );
	virtual void OnItemSelected( wxListEvent& event );
	
	void EditSwitch();
	void InitSwitches();

	void ConnectEvents();	
	void AddSwitch(const wxString &name, const wxString &value, bool choose);

public:
	CompilerPage( wxWindow* parent, wxString name, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 732,409 ), int style = wxTAB_TRAVERSAL );

};

#endif //__compiler_page__
