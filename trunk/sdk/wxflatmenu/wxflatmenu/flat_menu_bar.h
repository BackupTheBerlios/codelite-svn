///////////////////////////////////////////////////////////////////////////////
// Name:		flat_menu_bar.h
// Purpose:     declaration of a flat menu bar 
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef WXFLAT_MENU_BAR_H
#define WXFLAT_MENU_BAR_H

#include "flat_menu.h"
#include <vector>
#include "fm_smart_ptr.h"
#include <wx/stopwatch.h>
#include <wx/panel.h>
#include <wx/timer.h>
#include <wx/sizer.h>

#if wxUSE_AUI
//#include "manager.h"
#endif

class wxFMCustomizeDlg;

enum {
	MenuItem,
	DropDownArrowButton,
	ToolbarItem,
	NoWhere,
};

enum ToolBarIconSize {
	LargeIcons = 32,
	SmallIcons = 16
};

// Interal class which holds information about menu 
class wxMenuEntryInfo
{
	wxString m_title;
	wxFlatMenu *m_menu;
	wxRect m_rect;
	ControlState m_state;
	wxBitmap m_textBmp;
	int m_cmd;				//< the menu itself accelerator id
	
public:
	wxMenuEntryInfo() : m_title(wxEmptyString), m_menu(NULL), m_state(ControlNormal), m_cmd( wxNewId() ) {
	}

	wxMenuEntryInfo(const wxString& title, wxFlatMenu* menu) : m_title(title), m_menu(menu), m_state(ControlNormal), m_cmd(wxNewId())
	{
	}
	
	wxMenuEntryInfo(const wxMenuEntryInfo& rhs) { 
		*this = rhs; 
	}
	virtual ~wxMenuEntryInfo(){
	}

	wxMenuEntryInfo& operator=(const wxMenuEntryInfo& rhs) {
		m_menu = rhs.m_menu;
		m_title = rhs.m_title;
		m_rect = rhs.m_rect;
		m_state = rhs.m_state;
		m_cmd = rhs.m_cmd;
		return *this;
	}

	const wxString& GetTitle() const { 
		return m_title; 
	}

	wxFlatMenu* GetMenu() const { 
		return m_menu; 
	}

	void SetRect( const wxRect &rect) {
		m_rect = rect;
	}

	const wxRect& GetRect() const {
		return m_rect;
	}

	void SetState( ControlState state ) {
		m_state = state;
	}

	ControlState GetState() const { 
		return m_state;
	}

	void SetTextBitmap(const wxBitmap &bmp) {
		m_textBmp = bmp;
	}

	const wxBitmap& GetTextBitmap() const {
		return m_textBmp;
	}

	int GetCmdId() const { 
		return m_cmd;
	}
};

typedef fmSmartPtr<wxMenuEntryInfo> wxMenuEntryInfoPtr;

class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenuBar : public wxPanel
{
	friend class wxFlatMenu;
	friend class wxFMCustomizeDlg;

	// A class containing the button position and information
	class ToolBarItem
	{
	public:
		wxFlatToolbarItem *m_tbItem;
		wxRect m_rect;
		int m_state;
		bool m_visible;

	public:
		ToolBarItem(wxFlatToolbarItem *tbItem, const wxRect& rect, int state)
			: m_tbItem(tbItem)
			, m_rect(rect)
			, m_state(state)
			, m_visible(true)
		{
		}
		
		virtual ~ToolBarItem()
		{
			if(m_tbItem){
				delete m_tbItem;
				m_tbItem = NULL;
			}
		}
	};
	typedef fmSmartPtr<ToolBarItem> ToolBarItemPtr;

protected:
	std::vector<wxMenuEntryInfoPtr> m_items;
	int m_curretHiliteItem;
	wxRect m_dropDownButtonArea;
	ControlState m_dropDownButtonState;
	wxFlatMenu *m_moreMenu;
	int m_popupDlgCmdId;
	wxFMCustomizeDlg *m_dlg;
	bool m_showToolbar;
	int  m_barHeight;
	ToolBarIconSize m_tbIconSize;
	std::vector<ToolBarItemPtr> m_tbButtons;
	wxStopWatch m_watch;
	long m_interval;
	wxFlatMenu *m_tbMenu;
	wxBitmap m_moreMenuBgBmp;
	
protected:
	int GetActiveToolbarItem();
	void ResetToolbarItems();
	
	/**
	 * Return a rectangle surrounding the menu button 
	 * \return rect
	 */
	wxRect GetMoreMenuButtonRect();

	/**
	 * Perform a toolbar button pressed
	 * \param idx 
	 */
	void DoToolbarAction(size_t idx);

	/**
	 * handle mouse move event
	 * \param pos mouse current pos
	 */
	void DoMouseMove(wxPoint &pt, bool leftIsDown);
public:
	/**
	 * Construct a flat menu bar
	 * \param parent menu bar parent
	 * \param id window id
	 */
	wxFlatMenuBar(wxWindow* parent, wxWindowID id = wxID_ANY, bool showToobar = false, ToolBarIconSize iconSize = SmallIcons);

	/**
	 * Destructor
	 */
	virtual ~wxFlatMenuBar()
	{}

	/**
	 * Adds the item to the end of the menu bar
	 * \param menu The menu to add. Do not deallocate this menu after calling Append
	 * \param title The title of the menu
	 * \return true on success, false if an error occurred
	 */
	bool Append(wxFlatMenu* menu, const wxString &title);

#if wxUSE_AUI
	/**
	 * Position the menu bar in the AUI library, call this function if you want 
	 * wxFlatMenuBar to work with AUI library
	 * \param *mgr 
	 */
//	void PositionAUI(wxFrameManager *mgr);
#endif

	/**
	 * Test a given point
	 * \param pt mouse current position
	 * \param idx [output] item index
	 * \return NoWhere or MenuItem
	 */
	int HitTest( const wxPoint &pt, size_t &idx );

	/**
	 * Return an wxFlatMenuItem according to its ID
	 * \param id menu item command ID
	 * \return item or NULL
	 */
	wxFlatMenuItem *FindMenuItem(int id);

	/**
	 * Returns the index of the menu with the given title or wxNOT_FOUND if no such menu exists in this menubar. 
	 * The title parameter may specify either the menu title (with accelerator characters, i.e. "&File") or just the menu label ("File") indifferently.
	 * \param title menu title
	 * \return menu idex, zero based
	 */
	int FindMenu(const wxString& title) const;

	/**
	 * Returns the menu at menuIndex (zero-based), user must not release this menu!
	 * \param menuIdx menu index
	 * \return menu or NULL
	 */
	wxFlatMenu* GetMenu(int menuIdx) const;

	/**
	 * Inserts the menu at the given position into the menu bar. Inserting menu at position 0 will insert it in the very beginning of it, 
	 * inserting at position GetMenuCount() is the same as calling Append()
	 * \param pos The position of the new menu in the menu bar
	 * \param menu The menu to add. wxFlatMenuBar owns the menu and will free it
	 * \param title The title of the menu
	 * \return true on success, false if an error occurred
	 */
	bool Insert(size_t pos, wxFlatMenu *menu, const wxString& title);

	/**
	 * Removes the menu from the menu bar and returns the menu object - the caller is responsible for deleting it. 
	 * This function may be used together with wxFlatMenuBar::Insert to change the menubar dynamically
	 * \param pos menu position
	 * \return removed menu or NULL if index is out of bounds
	 */
	wxFlatMenu *Remove(size_t pos);

	/**
	 * Append toolbar item to this menu bar
	 * \param item 
	 */
	void AppendToolbarItem(wxFlatToolbarItem *item);

	/**
	 * Set the updateUI interval for toolbar items. All UpdateUI events are sent from within
	 * OnIdle() handler, the default is 20 milliseconds
	 * \param interval in milliseconds
	 */
	void SetUpdateInterval(long interval) { m_interval = interval; }

protected:
	// Event handlers
	/**
	 * Connect wxEvents to our menu bar
	 */
	void ConnectEvents();

	/**
	 * Handle paint evevt
	 * \param event Unused
	 */
	void OnPaint(wxPaintEvent &event);

	/**
	 * To reduce flickering, override erase background and leave it empty
	 * \param event Unused
	 */
	void OnEraseBg(wxEraseEvent &event);
	/**
	 * React to resize events
	 * \param event unused
	 */
	void OnSize(wxSizeEvent &event);
	/**
	 * Handle mouse movement events
	 * \param event mouse event
	 */
	void OnMouseMove(wxMouseEvent &event);
	/**
	 * Handle cursor leaving the window
	 * \param event unused
	 */
	void OnLeaveWindow(wxMouseEvent &event);

	/**
	 * Handle mouse left click down
	 * \param event mouse event
	 */
	void OnLeftDown(wxMouseEvent &event);
	
	/**
	 * Handle mouse left  up
	 * \param event mouse event
	 */
	void OnLeftUp(wxMouseEvent &event);

	/**
	 * Single function to handle any accelerator key used inside the menubar
	 * \param &event 
	 */
	void OnAccelCmd(wxCommandEvent &event);
	
	/**
	 * handle menu dismiss event
	 */
	void OnMenuDismissed(wxCommandEvent &e);

	/**
	 * handle the custimize dialog here
	 */
	void OnCustimizeDlg(wxCommandEvent &event);

	/**
	 * Handle idle event
	 */
	void OnIdle(wxIdleEvent &event);

	// Private methods
	/**
	 * Erase cached bitmaps. To increase performance of the drawings, the text drawing is done into
	 * a bitmaps and used later for next drawings. 
	 * \param start bitmap to start deleting from
	 */
	void ClearBitmaps(size_t start);
	/**
	 * Update the parent accelerator table
	 */
	void UpdateAcceleratorTable();
	/**
	 * Activate menu which its info it menuInfo
	 * \param menuInfo menu info
	 */
	void ActivateMenu(wxMenuEntryInfoPtr menuInfo);
	/**
	 * This function is called from child menus, this allow a child menu 
	 * to pass the mouse movement event to the menu bar
	 * \param pt mouse position
	 */
	void ProcessMouseMoveFromMenu( wxPoint &pt );
	/**
	 * Activate next menu and make sure all other are non-activated
	 */
	void ActivateNextMenu();
	/**
	 * Activate previous menu and make sure all other are non-activated
	 */
	void ActivatePreviousMenu();
	/**
	 * Return the index of the last visible menu on the menu bar
	 * \return index of the last visible menu on the menu bar
	 */
	size_t GetLastVisibleMenu();

	/**
	 * Drow 'more' button to the right side of the menu bar
	 * \param dc device context to use
	 * \param from unused 
	 * \param state the state of the button (focus, pressed, normal)
	 */
	void DrawMoreButton(wxDC& dc, int from, ControlState state = ControlNormal);

	/**
	 * Draw a toolbar item button
	 * \param dc device context
	 * \param button index 
	 * \param state button state
	 */
	void DrawToolbarItem(wxDC& dc, int idx, ControlState state = ControlNormal);

	/**
	 * Popup the 'more' menu 
	 */
	void PopupMoreMenu();
	
	/**
	 * Create the drop down menu and populate it
	 */
	void CreateMoreMenu();

	/**
	 * Draw the toolbar with the given dc & rect
	 * \param dc device context
	 * \param rect toolbar rectangle
	 */
	void DrawToolbar(wxDC& dc, const wxRect& rect);

	/**
	 * An item with ID was modified. This function is called by wxFlatMenu incase a 
	 * an item was modified directly and not via updateUI event
	 * \param item menu item that was modified
	 */
	void UpdateItem(wxFlatMenuItem *item);
};

#endif // WXFLAT_MENU_BAR_H

