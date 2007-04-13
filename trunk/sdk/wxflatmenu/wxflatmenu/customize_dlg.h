#ifndef FLATMENU_CUSTOMIZE_DLG_H
#define FLATMENU_CUSTOMIZE_DLG_H

#include <wx/dialog.h>
#include <map>
#include <wx/bitmap.h>

class wxFlatBookBase;
class wxCheckListBox;
class wxFlatMenu;
class wxFMTitlePanel;
class wxRadioBox;
class wxCheckBox;
class wxStaticBoxSizer;
class wxComboBox;

class wxFMCustomizeDlg : public wxDialog
{
protected:
	// add members here
	wxFlatBookBase *m_book;
	wxCheckListBox *m_checkListMenus;
	std::map<wxString, wxFlatMenu*> m_hiddenMenus;
	std::map<wxString, wxFlatMenu*> m_visibleMenus;
	int m_menuListId;
	wxFMTitlePanel *m_titlePanel;
	wxRadioBox *m_menuStyle;
	wxCheckBox *m_verticalGradient;
	wxCheckBox *m_drawBorder;
	wxStaticBoxSizer *m_sbStyle;
	wxComboBox *m_colour;
	wxCheckBox *m_shadowUnderTB;

	int m_menuStyleID;
	int m_drawVertGradID;
	int m_drawBorderID;
	int m_colourID;
	int m_shadowUnderTBID;

protected:
	void CloseDialog();

public:
	/**
	 * Parameterized constructor
	 * \param parent dialog parent window
	 */
	wxFMCustomizeDlg(wxWindow* parent);

	/**
	 * Default constructor
	 */
	wxFMCustomizeDlg();

	/**
	 * Destructor
	 */
	virtual ~wxFMCustomizeDlg();

	/**
	 * Create the dialog, usually part of the two steps construction of a 
	 * dialog
	 * \param parent dialog parent window
	 */
	void Create(wxWindow* parent);

	/**
	 * Initialise the dialog pages
	 */
	void Initialise();

protected:
	void ConnectEvents();
	void OnMenuChecked(wxCommandEvent &event);
	void OnChangeStyle(wxCommandEvent &event);
private:
	wxWindow* CreateMenusPage();
	wxWindow* CreateShortcutsPage();
	wxWindow* CreateOptionsPage();
};

class wxFMTitlePanel : public wxPanel
{
	wxString m_title;
public:
	wxFMTitlePanel(wxWindow* parent, const wxString &title);
	virtual ~wxFMTitlePanel();

protected:
	void ConnectEvents();
	void OnPaint(wxPaintEvent &event);
	void OnEraseBg(wxEraseEvent &event);
};

#endif // FLATMENU_CUSTOMIZE_DLG_H

