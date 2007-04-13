///////////////////////////////////////////////////////////////////////////////
// Name:		flat_menu.h
// Purpose:     Generic declaration of flat popup menu
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#pragma once
	   
#ifdef _DEBUG
#	include <crtdbg.h>
#	define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#	define DEBUG_NEW new
#endif

#include "wx/popupwin.h"
#include <wx/dynarray.h>
#include <wx/evtloop.h>
#include "fm_resources.h"
#include "art_manager.h"
#include "fm_smart_ptr.h"
#include <vector>
#include <wx/timer.h>

#ifndef WXMAKINGDLL_FLATMENU_EXPIMP
#ifdef WXMAKINGDLL_FLATMENU
#    define WXMAKINGDLL_FLATMENU_EXPIMP WXEXPORT
#elif defined(WXUSINGDLL)
#    define WXMAKINGDLL_FLATMENU_EXPIMP WXIMPORT
#else /* not making nor using DLL */
#    define WXMAKINGDLL_FLATMENU_EXPIMP
#endif // WXMAKINGDLL_FLATMENU
#endif 

DECLARE_EVENT_TYPE(wxEVT_FLAT_MENU_DISMISSED, -1)

class wxFlatMenu;
class wxFlatMenuBar;
class wxFlatMenuItem;
class wxFlatMenuButton;



#if 0
// On windows platforms, we override
/**
 * \ingroup wxflatcontrols
 * \brief a base class for a shdowed menu (Windows Only)
 *
 * \par requirements
 * win2k or later\n
  *
 * \version 1.0
 * first version
 *
 * \date 05-01-2006
 *
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxShadowPopupWindow : public wxPopupWindow
{
public:
	/**
	 * \brief constructor
	 */
	wxShadowPopupWindow();
	/**
	 * \brief Destructor
	 */
	virtual ~wxShadowPopupWindow(){};

protected:
	/**
	 * \brief Create a new handle (m_hWnd) and override the existing one created by base class.
	 * by doing so, we also register new class to windows, with the CS_DROPSHDOW style enabled, this
	 * allows us to provide a shdowed window
	 */
	void CreateMSW();
};
#else
	#define wxShadowPopupWindow wxPopupWindow
#endif

/**
 * \ingroup wxflatcontrols
 * \brief wxFlatMenuItemGroup - a class that manages a group
 * of radio menu items
 * \version 1.0
 * first version
 *
 * \date 03-20-2006
 *
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenuItemGroup
{
	std::vector<wxFlatMenuItem*> m_items;
public:
	wxFlatMenuItemGroup();
	virtual ~wxFlatMenuItemGroup();
	wxFlatMenuItem* GetSelectedItem() const;
	void Add(wxFlatMenuItem* item);
	void Remove(wxFlatMenuItem* item);
	void SetSelection(wxFlatMenuItem* item);

protected:
	bool Exist(wxFlatMenuItem *item) const;
};

typedef fmSmartPtr<wxFlatMenuItemGroup> wxFlatMenuItemGroupPtr;

enum ToolbarItemKind
{
	ToolbarItemButton,
	ToolbarItemSeparator,
	ToolbarItemCustom
};

/**
 * \ingroup wxflatmenu
 * \version 1.0
 * first version
 *
 * \date 10-30-2006
 *
 * \author Eran
 * This class represents a toolbar item
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatToolbarItem
{
	wxBitmap m_normalBmp;
	int		 m_id;	//< Command ID
	wxString m_label;
	wxBitmap m_disabledImg;
	wxWindow *m_customCtrl;
	ToolbarItemKind m_kind;
	bool m_enabled;

public:
	wxFlatToolbarItem(const wxBitmap& normalBmp, int id, const wxString &label = wxEmptyString, const wxBitmap &disabledBmp = wxNullBitmap, ToolbarItemKind kind = ToolbarItemButton)
		: m_normalBmp(normalBmp)
		, m_id(id)
		, m_label(label)
		, m_disabledImg(disabledBmp)
		, m_customCtrl(NULL)
		, m_kind(kind)
		, m_enabled(true)
	{
		if( !m_disabledImg.Ok() ) {
			// Create a grey bitmap from the normal bitmap
			m_disabledImg = wxArtManagerST::Get()->CreateGreyBitmap(m_normalBmp);
		}
	}

	wxFlatToolbarItem(wxWindow* customCtrl, int id, ToolbarItemKind kind = ToolbarItemCustom)
		: m_normalBmp(wxNullBitmap)
		, m_id(id)
		, m_label(wxEmptyString)
		, m_disabledImg(wxNullBitmap)
		, m_customCtrl(customCtrl)
		, m_kind(kind)
		, m_enabled(true)
	{
	}

	wxFlatToolbarItem(ToolbarItemKind kind = ToolbarItemSeparator)
		: m_normalBmp(wxNullBitmap)
		, m_id(wxNewId())
		, m_label(wxEmptyString)
		, m_disabledImg(wxNullBitmap)
		, m_customCtrl(NULL)
		, m_kind(kind)
		, m_enabled(true)
	{
	}

	virtual ~wxFlatToolbarItem()
	{
	}

	const wxString& GetLabel() const { return m_label; }
	const wxBitmap& GetBitmap() const { return m_normalBmp;}
	const wxBitmap& GetDisabledBitmap() const { return m_disabledImg; }
	int GetId() const { return m_id; }
	bool IsSeparator() const { return m_kind == ToolbarItemSeparator; }
	bool IsCustomControl() const { return m_kind == ToolbarItemCustom; }
	bool IsRegularItem() const { return m_kind == ToolbarItemButton; }
	wxWindow *GetCustomControl() { return m_customCtrl; }
	bool IsEnabled() const { return m_enabled; }
	
	void SetLabel(const wxString& label) { m_label = label; }
	void Enable(bool enable = true) { m_enabled = enable; }
	void SetBitmap(const wxBitmap &bmp) { m_normalBmp = bmp;}
	void SetDisabledBitmap(const wxBitmap &bmp) { m_disabledImg = bmp;}
	
	
};

/**
 * \ingroup wxflatcontrols
 * \brief wxFlatMenuItem - A menu item represents an item in a menu
 * \version 1.0
 * first version
 *
 * \date 03-20-2006
 *
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenuItem 
{
	friend class wxFlatMenu;
	friend class wxFlatMenuItemGroup;

	// Members
	wxString m_text;
	wxString m_label;
	wxItemKind m_kind;
	wxString m_helpString;
	int      m_id;
	wxFlatMenu *m_parentMenu, *m_subMenu;
	wxBitmap m_normalBmp, m_disabledBmp, m_hotBmp;
	bool m_bIsChecked, m_bIsEnabled;
	wxRect m_rect;
	wxString m_accelStr;
	int m_mnemonicIdx;
	bool m_isAttachedToMenu;	//< Will be updated by the menu
	wxFlatMenuItemGroupPtr m_groupPtr;
	bool m_visible;

public:
	/**
	 * \brief parameterized constructor
	 * \param parent Menu that the menu item belongs to 
	 * \param label Text for the menu item, as shown on the menu, An accelerator key can be specified using the ampersand '&' character. In order to embed an ampersand character in the menu item text, the ampersand must be doubled
	 * \param kind May be wxITEM_SEPARATOR, wxITEM_NORMAL, wxITEM_CHECK or wxITEM_RADIO
	 * \param helpString Optional help string that will be shown on the status bar
	 * \param nomralBmp normal bitmap to draw to the side of the text, this bitmap is used when the menu is enabled
	 * \param disabledBmp 'greyed' bitmap to draw to the side of the text, this bitmap is used when the menu is disabled, if non supplied, normal is used
	 * \param hotBmp hot bitmap to draw to the side of the text, this bitmap is used when the menu is hovered, if non supplied, normal is used.
	 */
	wxFlatMenuItem(wxFlatMenu* parent, 
				 int id = wxID_SEPARATOR,
				 const wxString& text = wxEmptyString, 
				 const wxString& helpString = wxEmptyString, 
				 wxItemKind kind = wxITEM_NORMAL, 
				 wxFlatMenu* subMenu = NULL, 
				 const wxBitmap& normalBmp = wxNullBitmap, 
				 const wxBitmap& disabledBmp = wxNullBitmap, 
				 const wxBitmap& hotBmp = wxNullBitmap)
		: m_text(text)
		, m_kind(kind)
		, m_helpString(helpString)
		, m_id(id)
		, m_parentMenu(parent)
		, m_subMenu(subMenu)
		, m_normalBmp(normalBmp)
		, m_disabledBmp(disabledBmp)
		, m_hotBmp(hotBmp)
		, m_bIsChecked(false)
		, m_bIsEnabled(true)
		, m_mnemonicIdx(wxNOT_FOUND)
		, m_isAttachedToMenu(false)
		, m_visible(false)
	{
		SetLabel(m_text);
		if( m_id == wxID_ANY )
			m_id = wxNewId();
	};

	/**
	 * \brief destructor
	 */
	virtual ~wxFlatMenuItem() {};
	
	/**
	 * \brief Checks or unchecks the menu item
	 * \param check can be true or false
	 */
	void Check(bool check = true); 
	
	/**
	 * \brief Enable/disable the menu item
	 * \param enable can be true of false
	 */
	void Enable(bool enable = true);

	/**
	 * \brief Sets the text for this menu item (including accelerators)
	 * \param text menu item text, including accelerator
	 */
	void SetText(const wxString& text);

	/**
	 * return the normal bitmap or wxNullBitmap if non supplied
	 * \return normal bitmap
	 */
	const wxBitmap& GetBitmap() const { return m_normalBmp; }

	/**
	 * return the disabled bitmap or wxNullBitmap if non supplied
	 * \return disabled bitmap
	 */
	const wxBitmap& GetDisabledBitmap() const { return m_disabledBmp; }

	/**
	 * return the hot bitmap or wxNullBitmap if non supplied
	 * \return hot bitmap
	 */
	const wxBitmap& GetHotBitmap() const { return m_hotBmp; }

	/**
	 *	\brief Returns the help string associated with the menu item
	 * \return help string
	 */
	wxString GetHelp() const { return m_helpString; }

	/**
	 * \brief Return the menu item id
	 * \return menu item id
	 */
	int GetId() const { return m_id; }

	/**
	 * \brief return the menu item kind, can be one of wxITEM_SEPARATOR, wxITEM_NORMAL, wxITEM_CHECK or wxITEM_RADIO
	 * \return menu item kind
	 */
	wxItemKind GetKind() const { return m_kind; }

	/**
	 * \brief return the menu item label (without the accelerator if is part of the string)
	 * \return menu item lable
	 */
	wxString GetLabel() const { return m_label; }

	/**
	 * \brief return the menu which this item belongs to
	 * \return the menu which this item belongs to
	 */
	wxFlatMenu* GetMenu() const { return m_parentMenu; }

	/**
	 * \brief Returns the text associated with the menu item including accelerator
	 * \return menu text
	 */
	wxString GetText() const { return m_text; }

	/**
	 * \brief Return the sub menu
	 * \return sub menu or NULL
	 */
	wxFlatMenu* GetSubMenu() const { return m_subMenu;}

	/**
	 * \brief return true if this item is of type wxITEM_CHECK, false otherwise
	 */
	bool IsCheckable() const { return m_kind == wxITEM_CHECK; }
	
	/**
	 * \brief return true if this item is of type wxITEM_RADIO, false otherwise
	 */
	bool IsRadioItem() const { return m_kind == wxITEM_RADIO; }

	/**
	 * \brief return true if this item is checked, false otherwise
	 */
	bool IsChecked() const { return m_bIsChecked; }
	/**
	 * \brief return true if this item is enabled, false otherwise
	 */
	bool IsEnabled() const { return m_bIsEnabled; }
	/**
	 * \brief return true of this item is a separator
	 */
	bool IsSeparator() const { return m_id == wxID_SEPARATOR; }
	/**
	 * \brief return true of this item is a sub menu
	 */
	bool IsSubMenu() const { return m_subMenu != NULL; }
	/**
	 * \brief set the normal bitmap 
	 * \param bmp bitmap to use 
	 */
	void SetNormalBitmap(const wxBitmap& bmp) { m_normalBmp = bmp; }
	/**
	 * \brief set the disabled bitmap 
	 * \param bmp bitmap to use 
	 */
	void SetDisabledBitmap(const wxBitmap& bmp) { m_disabledBmp = bmp; }
	/**
	 * \brief set the hot (hovered) bitmap 
	 * \param bmp bitmap to use 
	 */
	void SetHotBitmap(const wxBitmap& bmp) { m_hotBmp = bmp; }
	/**
	 * \brief sets the help string for this menu item (displayed on the status bar)
	 * \param helpString help string to set
	 */
	void SetHelp(const wxString& helpString) { m_helpString = helpString; }
	/**
	 * \brief sets the menu that this item belongs to
	 * \param menu parent menu
	 */
	void SetMenu(wxFlatMenu* menu) { m_parentMenu = menu; }
	/**
	 * \brief sets the sub menu for this item
	 * \param menu sub menu to set
	 */
	void SetSubMenu(wxFlatMenu* menu) { m_subMenu = menu; }

	/**
	 * \brief Return the accelerator string
	 */
	wxString GetAccelString() const { return m_accelStr; }
	
	/**
	 * \brief return the accelerator entry associated to this menu item. Note that returned value is allocated on the 
	 * heap and must be freed by caller
	 * \return accelerator entry or NULL
	 */
	wxAcceleratorEntry* GetAcceleratorEntry();

	/**
	 * \brief return the shortcut char for this menu item
	 * \return wxChar represents the mnemonic for this menu item or 0 if non
	 */
	wxChar GetMnemonicChar();
	
	/**
	 * Return true if this item is visible
	 */
	bool IsShown() const { return m_visible; }

	/**
	 * Set the item visibility
	 */
	void Show(bool show = true) { m_visible = show; }

private:
	void DrawSelf(wxDC& dc, int yCoord, int imageMarginX, int markerMaringX, int textX, int rightMaringX, bool selected = false);
	void SetRect(const wxRect& rect) { m_rect = rect; }
	wxRect GetRect() const { return m_rect; }
	int  GetHeight() const;
	/**
	 * \brief the bitmap that should be used based on the item state
	 */
	wxBitmap GetSuitableBitmap(bool selected) const;

	/**
	 * \brief sets the label text for this item from the text (excluding the accelerator)
	 * \param text item menu item text (including accelerator)
	 */
	void SetLabel(const wxString& text); 
};

/**
 * \ingroup wxflatcontrols
 *	\brief Base class for generic flat menu derived from wxPopupWindow
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 *
 * \version 1.0
 * first version
 *
 * \date 03-15-2006
 *
 * \author wxWindows
 *
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenuBase :
	public wxShadowPopupWindow
{
	friend class wxFlatMenu;
	friend class wxFlatMenuBar;

protected:
	wxFlatMenuBase *m_parentMenu;
	wxFlatMenuBase *m_openedSubMenu;
	wxWindow		*m_owner;
	int m_popupPtOffset;			// An additional offset that will be used for menu popup position calculation
	bool m_showScrollButtons;		//< Do we need scrollbuttons?
	wxFlatMenuButton *m_upButton;
	wxFlatMenuButton *m_downButton;

#ifdef __WXMSW__
	wxCursor m_oldCur;
#endif
	
public:
	/**
	 * \brief Constructor
	 */
	wxFlatMenuBase() ;

	/**
	 * \brief Destructor
	 */
	virtual ~wxFlatMenuBase(void);

	/**
	 * \brief Popup menu at point 'pt', 'pt' assumed to be in screen coordinates. However, 
	 *  If parent is not NULL, 'pt' is translated into the screen coordiantes using parent->ClientToScreen()
	 * \param parent when the menu is sub-menu, parent is the parent menu
	 * \param pt point to popup the menu
	 */
	virtual void Popup(const wxPoint& pt, wxFlatMenuBase* parent = NULL);

	/**
	 * \brief get the top level menu
	 * \return root menu
	 */
	virtual wxFlatMenuBase* GetRootMenu();
	
	/**
	 * Set the menu owner height, this will be used to position the menu
	 * below or above the owner 
	 * \param height owner height in pixels
	 */
	virtual void SetOwnerHeight(int height) { m_popupPtOffset = height; }

	/**
	 * Scroll one unit up
	 * by default this function is empty, let derived class do something
	 * intersting with it
	 */
	virtual bool ScrollUp();

	/**
	 * Scroll one unit down
	 * by default this function is empty, let derived class do something
	 * intersting with it
	 */	
	virtual bool ScrollDown();
protected:
	/**
	* \brief Fire an event wxEVT_FLAT_MENU_DISMISSED and handle menu dismiss
	*/
	virtual void OnDismiss();

	/**
	 * \brief handle child dismiss
	 */
	virtual void OnChildDismiss();

	/**
	 * \brief Dismiss the popup window
	 * \param dismissParent dismisses parent menu (e.g. when dismissing menu using the ESC key, this value is set to false)
	 */
	virtual void Dismiss(bool dismissParent, bool resetOwner);

	/**
	 * \brief Adjust position so the menu will be fully visible on screen
	 * \param pos menu popup position 
	 */
	virtual void AdjustPosition(wxPoint& pos);

	/**
	 * \brief return the menu logical owner, the owner does not necessarly mean the menu parent, 
	 *  but can be the window that poped up it
	 * \return menu owner
	 */
	virtual wxWindow* GetMenuOwner() const { return m_owner; }
};

class wxMenuKbdRedirector;
//-------------------------------
// HitTest results for flat menu
//-------------------------------
enum wxFlatMenuHT
{
	wx_MENU_HT_NONE,
	wx_MENU_HT_ITEM,
	wx_MENU_HT_SCROLL_UP,
	wx_MENU_HT_SCROLL_DOWN
};

/**
 * \ingroup wxflatcontrols
 * \brief flat popup menu
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 *
 * \version 1.0
 * first version
 *
 * \date 03-22-2006
 *
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenu : public wxFlatMenuBase
{
	DECLARE_CLASS(wxFlatMenu)
	
	friend class wxMenuKbdRedirector;
	friend class wxFocusHandler;
	friend class wxFlatMenuItem;
	friend class wxFlatMenuBar;

	std::vector<wxFlatMenuItem*> m_itemsArr;
	int m_menuWidth;
	int m_leftMarginWidth;
	int m_rightMarginWidth;
	int m_borderXWidth;
	int m_borderYWidth;
	wxWindow* m_activeWin;
	wxWindow* m_focusWin;
	int m_imgMarginX;
	int m_markerMarginX;
	int m_textX;
	int m_rightMarginPosX;
	int m_itemHeight;
	int m_selectedItem;
	bool m_clearCurrentSelection;
	int m_textPadding;
	int m_marginHeight;
	int m_marginWIdth;
	int m_accelWidth;
	std::vector<wxAcceleratorEntry> m_accelArray;
	wxFlatMenuBar *m_mb;
	bool m_resizeMenu;
	wxPoint m_decPt;
	wxString m_menuBarFullTitle;	// For internal usage by menu bar !
	wxString m_menuBarLabelOnly;	// For internal usage by menu bar !
	int m_first;

#ifdef __WXMSW__
	wxPoint m_mousePtAtStartup;
#endif

private:
	void ResizeMenu();

protected:
	virtual wxRect GetMenuRect () const;
	virtual int GetMenuWidth() const  { return m_menuWidth; }
	virtual int GetLeftMarginWidth()  const { return m_leftMarginWidth; }
	virtual int GetRightMarginWidth()  const { return m_rightMarginWidth; }
	virtual int GetBorderXWidth()  const { return m_borderXWidth; }
	virtual int GetBorderYWidth()  const { return m_borderYWidth; }
	virtual int GetMenuItemWidth(wxFlatMenuItem *menuItem) ;
	virtual int GetItemHeight() const { return m_itemHeight; }
	virtual bool OnChar(int key);
	virtual void AdvanceSelection(bool down = true);
	virtual void CloseSubMenu(int itemIdx);
	virtual	void SendUIEvent(int itemIdx);
	virtual void SendCmdEvent(int itemIdx);
	virtual int  FindMenuItemPos(int id, wxFlatMenu** menu) const;
	virtual int  FindMenuItemPos(wxFlatMenuItem *item) const;
	virtual wxFlatMenuItem *GetSiblingGroupItem(wxFlatMenuItem *item) const;
	virtual void UpdateRadioGroup(wxFlatMenuItem *item);
	virtual bool TryScrollButtons(wxMouseEvent &e);

	// An item was modified directly by user,
	// we have nothing to do with this, but if a menu bar
	// is attached to us, it might intrest it...
	virtual  void UpdateItem(wxFlatMenuItem* item);

	//-----------------------------
	// Drawing
	//-----------------------------
	virtual void DrawLeftMargin(wxDC& dc, wxRect &menuRect);
	virtual void DrawSelection(wxDC& dc, int oldSelection = -1);
	virtual void DoDrawMenu(wxDC& dc);

	/**
	 * \brief In some cases, we need to perform a recursive refresh for all 
	 * opened submenu from this
	 */
	virtual void RefreshChilds();

	//-----------------------------
	// Misc
	//-----------------------------
	/**
	 * \brief test if pos is pointing on either a menu item or anywhere else
	 * \param pos mouse position
	 * \param itemIdx item index 
	 * \return wx_MENU_HT_NONE or wx_MENU_HT_ITEM
	 */
	virtual  wxFlatMenuHT HitTest(wxPoint& pos, int& itemIdx);

	/**
	 * \brief react on a left down on an item
	 * \param itemIdx item that was left clicked
	 */
	virtual void DoAction(int itemIdx);

	/**
	 * \brief if itemIdx is an item with submenu, open it
	 */
	virtual bool TryOpenSubMenu(int itemIdx, bool selectFirst = false);

	/**
	 * \brief handle mouse left click on a menu
	 */
	virtual void ProcessMouseLClick(wxPoint& pos);

	/**
	 * \brief handle mouse right click on a menu
	 */
	virtual void ProcessMouseRClick(wxPoint& pos);

	/**
	 * \brief process a mouse move
	 * \param pos current mouse move
	 */
	virtual void ProcessMouseMove(wxPoint& pos);

	/**
	 * \brief Dismiss the popup window
	 * \param dismissParent dismisses parent menu (e.g. when dismissing menu using the ESC key, this value is set to false)
	 */
	virtual void Dismiss(bool dismissParent, bool resetOwner);

	/**
	 * Attache this menu to a menubar
	 * \param mb wxFlatMenuBar
	 */
	void SetMenuBar( wxFlatMenuBar *mb );

	/**
	 * Internal function to help recurse thru all over the menu items
	 */
	void GetAllItems(wxFlatMenu* menu, std::vector<wxFlatMenuItem*>& items);

	/**
	 * Handle scroll up
	 */
	virtual bool ScrollUp();

	/**
	 * Handle scroll down
	 */
	virtual bool ScrollDown();

public:
	/**
	 * \brief constructor
	 */
	wxFlatMenu();

	/**
	 * \brief destructor
	 */
	virtual ~wxFlatMenu();

	/**
	 * \brief append new item to the menu
	 * \param id menu item id
	 * \param item menu item text (including any accelerator)
	 * \param helpString help string
	 * \param kind menu item kind
	 * \return the new added menu item
	 */
	wxFlatMenuItem* Append(int id, const wxString& item, const wxString& helpString = wxEmptyString, wxItemKind kind = wxITEM_NORMAL);

	/**
	 * \brief Adds a pull-right submenu to the end of the menu 
	 * \param id menu item id
	 * \param item menu item text (including any accelerator) 
	 * \param *subMenu submenu 
	 * \param helpString help string 
	 * \return the new added menu item
	 */
	wxFlatMenuItem* Append(int id, const wxString& item, wxFlatMenu *subMenu, const wxString& helpString = wxEmptyString);

	/**
	 * \brief append new item to the menu
	 * \param menuItem menu item to add
	 * \return the new added menu item
	 */
	wxFlatMenuItem* Append(wxFlatMenuItem* menuItem);

	/**
	 * \brief add a radio-check menu item to the end of the menu
	 * \param id menu item id
	 * \param item menu item text (including any accelerator) 
	 * \param helpString 
	 * \return the new added menu item
	 */
	wxFlatMenuItem* AppendCheckItem(int id, const wxString& item, const wxString& helpString = wxEmptyString);

	/**
	 * \brief add a checkable menu item to the end of the menu
	 * \param id menu item id
	 * \param item menu item text (including any accelerator) 
	 * \param helpString 
	 * \return the new added menu item
	 */
	wxFlatMenuItem* AppendRadioItem(int id, const wxString& item, const wxString& helpString = wxEmptyString);

	/**
	 * \brief append separator to the end of the menu
	 */
	wxFlatMenuItem* AppendSeparator();

	/**
	 * \brief insert menu item at pos
	 * \param pos zero based position to insert the menu item
	 * \param item item to insert
	 * \return added item 
	 */
	wxFlatMenuItem* Insert(size_t pos, wxFlatMenuItem *item);

	/**
	 * \brief insert new item to the menu
	 * \param pos zero based position to insert the menu item
	 * \param id menu item id
	 * \param item menu item text (including any accelerator)
	 * \param helpString help string
	 * \param kind menu item kind (one of wxITEM_NORMAL, wxITEM_CHECK or wxITEM_RADIO)
	 * \return the new inserted menu item
	 */
	wxFlatMenuItem* Insert(size_t pos, int id, const wxString& item, const wxString& helpString = wxEmptyString, wxItemKind kind = wxITEM_NORMAL);

	/**
	 * \brief Removes the menu item from the menu by id but doesn't delete the associated C++ object
	 * \param id menu item id
	 * \return removed menu item or NULL
	 */
	wxFlatMenuItem* Remove(int id);

	/**
	 * \brief Removes the menu item from the menu by pointer but doesn't delete the associated C++ object
	 * \param item item pointer to be removed
	 * \return removed menu item or NULL
	 */
	wxFlatMenuItem* Remove(wxFlatMenuItem *item);

	/**
	 * \brief Deletes the menu item from the menu. If the item is a submenu (i.e. item->IsSubMenu() == true), 
	 *  it will be deleted
	 * \param id 
	 */
	void Destroy(int id);

	/**
	 * \brief Deletes the menu item from the menu. If the item is a submenu (i.e. item->IsSubMenu() == true), it will be deleted
	 * \param item item to be deleted
	 */
	void Destroy(wxFlatMenuItem *item);

	/**
	 * \brief Finds the menu item object associated with the given menu item identifier and, optionally, the (sub)menu it belongs to.
	 * \param itemId menu item id
	 * \param menu submenu
	 * \return menu item
	 */
	wxFlatMenuItem * FindItem(int itemId, wxFlatMenu **menu = NULL) const;

	/**
	 * \brief Popup menu at point 'pt', 'pt' assumed to be in screen coordinates. However, 
	 *  If parent is not NULL, 'pt' is translated into the screen coordiantes using parent->ClientToScreen()
	 * the second argument (owner) is used for event processing, if owner is NULL, all events will be 
	 * processed using this->GetEventHandler(), else, the owner will be used (owner->GetEventHandler())
	 * \param parent when the menu is sub-menu, parent is the parent menu
	 * \param owner menu owner, used for event processing
	 * \param pt point to popup the menu, see above
	 */
	virtual void Popup(const wxPoint& pt, wxWindow* owner = NULL, wxFlatMenuBase* parent = NULL);

	/**
	 * \brief get the accelerator table for this menu
	 * \return accelerator table
	 */
	wxAcceleratorTable GetAccelTable();

	/**
	 * Return the interal array that holds the accel array
	 * \return accel array
	 */
	std::vector<wxAcceleratorEntry>& GetAccelArray() ;

	/**
	 * Get a vector of all menu items (including sub-menus) 
	 * NOTE: the list of vectors returned are owned by this menu use then for
	 * read-only!
	 */
	void GetAllItems(std::vector<wxFlatMenuItem*>& items) ;

	/**
	 * Remove all items from this menu and delete them
	 */
	void Clear();

	/**
	 * Return an iterator to the first item in this menu
	 * \return std iterator
	 */
	std::vector<wxFlatMenuItem*>::iterator Begin() { return m_itemsArr.begin(); }

	/**
	 * Return an iterator to the end of the this menu items' vector.  
	 * NOTE: this iterator can not be used to access items, but only for comparison 
	 * to check whether we reached to the end of this menu items
	 * \return std end iterator
	 */
	std::vector<wxFlatMenuItem*>::iterator End() { return m_itemsArr.end(); }

	// Event handlers
	DECLARE_EVENT_TABLE()	
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBg(wxEraseEvent& WXUNUSED(event)) {};
	virtual void OnKeyDown(wxKeyEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseLeaveWindow(wxMouseEvent& event);
	virtual void OnMouseEnterWindow(wxMouseEvent& event);
	virtual void OnMouseLeftDown(wxMouseEvent& event);
	virtual void OnMouseRightDown(wxMouseEvent& event);
	virtual void OnMouseLeftUp(wxMouseEvent &event);
	virtual void OnKillFocus(wxFocusEvent &event);
	virtual void OnTimer(wxTimerEvent &event);
};

//----------------------------------------------
// Keyboard event handler
//----------------------------------------------

class WXMAKINGDLL_FLATMENU_EXPIMP wxMenuKbdRedirector : public wxEvtHandler
{
public:
	void SetMenu(wxFlatMenu *menu) { m_menu = menu; }
	wxMenuKbdRedirector(wxFlatMenu *menu, wxEvtHandler* oldHandler)
		: m_oldHandler( oldHandler ) 
	{ SetMenu(menu); };

    virtual bool ProcessEvent(wxEvent& event);

private:
    wxFlatMenu *m_menu;
	wxEvtHandler* m_oldHandler;
};

//----------------------------------------------
// Focus event handler
//----------------------------------------------

class WXMAKINGDLL_FLATMENU_EXPIMP wxFocusHandler : public wxEvtHandler
{
public:
	void SetMenu(wxFlatMenu *menu) { m_menu = menu; }
	virtual ~wxFocusHandler(){}
	wxFocusHandler(wxFlatMenu *menu){ SetMenu(menu); };

	DECLARE_EVENT_TABLE()
	void OnKeyDown(wxKeyEvent& event);
	void OnKillFocus(wxFocusEvent &event);

private:
    wxFlatMenu *m_menu;
};
