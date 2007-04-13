// Name:		flat_menu.h
// Purpose:     Generic declaration of flat popup menu
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#include "flat_menu.h"
#include <wx/dcbuffer.h>
#include <wx/arrimpl.cpp>
#include <wx/tooltip.h>
#include "fm_renderer.h"
#include "fm_resources.h"
#include "flat_menu_bar.h"
#include <map>
#include <wx/app.h>
#include <wx/settings.h>
#include <wx/image.h>
#include "flat_button.h"
#include <wx/utils.h>

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif

#define SCROLL_BTN_HEIGHT 20

DEFINE_EVENT_TYPE(wxEVT_FLAT_MENU_DISMISSED)

#ifdef DEVELOPMENT
#include <wx/log.h>
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; wxLogMessage( logmsg ); }
#else
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; }
#endif


#define VALIDATE_IDX(itemIdx) { if(itemIdx<0 || itemIdx>(int)(m_itemsArr.size()-1))	return; }
#define VALIDATE_IDX_BOOL(itemIdx) { if(itemIdx<0 || itemIdx>(int)(m_itemsArr.size()-1))	return false; }

//-------------------------------------------------------------------
// A custome class for MSW that provides shadow
//-------------------------------------------------------------------
#if 0

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif 

LRESULT WXDLLEXPORT APIENTRY wxWndProc(HWND, UINT, WPARAM, LPARAM);
wxShadowPopupWindow::wxShadowPopupWindow()
{
	wxWindow* parent = wxTheApp->GetTopWindow();
	wxASSERT_MSG(parent, wxT("Can't create menu without parent!"));

	// popup windows are created hidden by default
	wxPopupWindow::Create(parent);//, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, wxT("flat_menu"));
	// We need to override the creation of the window with our
	// so we provide other class to this window, with shadow style enabled
	CreateMSW();
}

void wxShadowPopupWindow::CreateMSW()
{
	static bool classRegistered = false;
	static wxString className( wxT("wxMenuNR") );
	if( !classRegistered )
	{
		// Register class 
		WNDCLASS wndclass;
		wxZeroMemory(wndclass);

		// for each class we register one with CS_(V|H)REDRAW style and one
		// without for windows created with wxNO_FULL_REDRAW_ON_REPAINT flag
		static long styleNoRedraw = CS_DBLCLKS;

		int major, minor;
		wxGetOsVersion(&major, &minor);
		
		if(major == 5 && minor == 1)
		{
			// XP 
			// This style is available for XP version
			styleNoRedraw |= CS_DROPSHADOW; 
		}

		// the fields which are common to all classes
		wndclass.lpfnWndProc   = (WNDPROC)wxWndProc;
		wndclass.hInstance     = wxhInstance;
		wndclass.hCursor       = ::LoadCursor((HINSTANCE)NULL, IDC_ARROW);

		// register the class for all normal windows
		wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
		wndclass.lpszClassName = className.GetData();
		wndclass.style         = styleNoRedraw;

		if ( !RegisterClass(&wndclass) )
		{
			wxLogLastError(wxT("RegisterClass"));
		}
		classRegistered = true;
	}

	// choose the position/size for the new window
    int x, y, w, h;
    (void)MSWGetCreateWindowCoords(GetPosition(), GetSize(), x, y, w, h);

    // controlId is menu handle for the top level windows, so set it to 0
    // unless we're creating a child window
    unsigned long controlId = (unsigned long)GetWindowStyleFlag() & WS_CHILD ? GetId() : 0;
	
	WXDWORD exstyle;
	DWORD msflags = MSWGetCreateWindowFlags(&exstyle);

#ifdef __WXUNIVERSAL__
	// no borders, we draw them ourselves
	exstyle &= ~(WS_EX_DLGMODALFRAME |
		WS_EX_STATICEDGE |
		WS_EX_CLIENTEDGE |
		WS_EX_WINDOWEDGE);
	msflags &= ~WS_BORDER;
#endif // wxUniversal

    // do create the window
    wxWindowCreationHook hook(this);
	::DestroyWindow((HWND)m_hWnd);

    m_hWnd = (WXHWND)::CreateWindowEx
                       (
                        exstyle,
						className.GetData(),
                        wxT("ShadowWindow"),
						msflags,
                        x, y, w, h,
                        (HWND)MSWGetParent(),
                        (HMENU)controlId,
                        wxGetInstance(),
                        NULL                        // no extra data
                       );

    if ( !m_hWnd )
    {
        wxLogSysError(wxT("Can't create window of class wxShadowPopupWindow"));
        return;
    }
	SetHWND( m_hWnd );
}
#endif 

//-------------------------------------------------------------------
// Shadow menu base
//-------------------------------------------------------------------

wxFlatMenuBase::wxFlatMenuBase() 
: m_parentMenu(NULL)
, m_openedSubMenu(NULL)
, m_owner(NULL)
, m_popupPtOffset(0)
, m_showScrollButtons(false)
, m_upButton(NULL)
, m_downButton(NULL)
{
	wxWindow* parent = wxTheApp->GetTopWindow();
	wxASSERT_MSG(parent, wxT("Can't create menu without parent!"));
	wxPopupWindow::Create(parent, wxBORDER_NONE );
}

wxFlatMenuBase::~wxFlatMenuBase(void)
{
}

void wxFlatMenuBase::OnDismiss()
{
	// Release mouse capture if needed
	if(HasCapture())
	{
		ReleaseMouse();
	}

	// send an event about our dismissal to the parent (unless we are a sub menu)
	if(IsShown() && !m_parentMenu)
	{
		wxCommandEvent event(wxEVT_FLAT_MENU_DISMISSED, GetId());
		event.SetEventObject(this);

		// Send it
		if(GetMenuOwner())
			GetMenuOwner()->GetEventHandler()->ProcessEvent( event );
		else
			GetEventHandler()->ProcessEvent( event );
	}
}

void wxFlatMenuBase::Popup(const wxPoint& pt, wxFlatMenuBase* parent)
{
	// some controls update themselves from OnIdle() call - let them do it
	wxTheApp->ProcessIdle();

	// The mouse was pressed in the parent coordinates, 
	// e.g. pressing on the left top of a text ctrl
	// will result in (1, 1), these coordinates needs
	// to be converted into screen coords
	m_parentMenu = parent;

	// If we are topmost menu, we use the given pt
	// else we use the logical 
	// parent (second argument provided to this function)
	wxPoint pos;
	if(m_parentMenu)
		pos = m_parentMenu->ClientToScreen(pt);
	else
		pos = pt;

	// Fit the menu into screen
	AdjustPosition(pos);
	if(m_showScrollButtons) 
	{
		// For some reason, which I cant seem to understand, this code
		// is working only for windows, On GTK it crashes!
		wxSize sz = GetSize();
		// Get the screen height
		int scrHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
		
		if(!m_upButton) {
			m_upButton = new wxFlatMenuButton(this, true, *wxArtManagerST::Get()->GetStokBitmap(wxT("arrow_up")));
		}

		if(!m_downButton) {
			m_downButton= new wxFlatMenuButton(this, false, *wxArtManagerST::Get()->GetStokBitmap(wxT("arrow_down")));
		}

		// position the scrollbar
		m_upButton->SetSize(SCROLL_BTN_HEIGHT, SCROLL_BTN_HEIGHT);
		m_downButton->SetSize(SCROLL_BTN_HEIGHT, SCROLL_BTN_HEIGHT);

		m_upButton->Move(sz.x - SCROLL_BTN_HEIGHT - 4, 4);
		m_downButton->Move(sz.x - SCROLL_BTN_HEIGHT - 4, scrHeight - pos.y - 2 - SCROLL_BTN_HEIGHT );
	}
	
	Move(pos);
	Show();

	// Capture mouse event and direct them to us
	CaptureMouse();
}

void wxFlatMenuBase::AdjustPosition(wxPoint& pos)
{
	// Check that the menu can fully appear in the screen
	int scrWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	int scrHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	wxSize size = GetSize();

	// always assume that we have scrollbuttons on
	m_showScrollButtons = false;
	pos.y += m_popupPtOffset;
	if(size.y + pos.y > scrHeight) {
		// the menu will be truncated
		if(m_parentMenu == NULL) {
			// try to flip the menu
			int flippedPosy = pos.y - size.y;
			flippedPosy -= m_popupPtOffset;

			if(flippedPosy >= 0 && flippedPosy + size.y < scrHeight){
				pos.y = flippedPosy;
				return;
			} else {
				// We need to popup scrollbuttons!
				m_showScrollButtons = true;
			}
		} else {
			// we are a submenu
			// try to decrese the y value of the menu position
			int newy = pos.y;
			newy -= (size.y + pos.y) - scrHeight;
			if(newy + size.y > scrHeight){
				// probably the menu size is too high to fit
				// the screen, we need scrollbuttons
				m_showScrollButtons = true;
			} else {
				pos.y = newy;
			}
		}
	}

	wxCoord menuMaxX = pos.x + size.x;
	if(menuMaxX > scrWidth)
	{
		if(m_parentMenu)
		{
			// We are submenu
			pos.x -= (size.x + m_parentMenu->GetSize().x);
			pos.x += 10;
		}
		else
		{
			pos.x -= ((size.x + pos.x) - scrWidth);
		}
	}
	else
	{
		if(m_parentMenu)
			pos.x += 5;
	}
}

void wxFlatMenuBase::Dismiss(bool dismissParent, bool resetOwner)
{
	// Check if child menu is poped, if so, dismiss it
	if(m_openedSubMenu)
		m_openedSubMenu->Dismiss(false, resetOwner);

	OnDismiss();

	// Reset menu owner
	if(resetOwner)
		m_owner = NULL;

	Show(false);

	if(m_parentMenu && dismissParent)
	{
		m_parentMenu->OnChildDismiss();
		m_parentMenu->Dismiss(dismissParent, resetOwner);
	}
	m_parentMenu = NULL;
}

void wxFlatMenuBase::OnChildDismiss()
{
	m_openedSubMenu = NULL;
}

wxFlatMenuBase* wxFlatMenuBase::GetRootMenu()
{
	wxFlatMenuBase* root = this;
	while( root->m_parentMenu )
		root = root->m_parentMenu;
	return root;
}

// by defualt do nothing
bool wxFlatMenuBase::ScrollDown()
{
	return false;
}

// by defualt do nothing
bool wxFlatMenuBase::ScrollUp()
{
	return false;
}

//--------------------------------------------------------------------------------
// Menu Item
//--------------------------------------------------------------------------------

void wxFlatMenuItem::DrawSelf(wxDC& dc, int yCoord, int imageMarginX, int WXUNUSED(markerMarginX), int textX, int rightMarginX,	bool selected /** false **/)
{ 
	wxBitmap bmp;
	int borderXSize = m_parentMenu->GetBorderXWidth();
	int itemHeight = m_parentMenu->GetItemHeight();
	int menuWidth  = m_parentMenu->GetMenuWidth();
	wxControlTheme theme = wxArtManagerST::Get()->GetMenuTheme();
	wxColour disableColor = wxArtManagerST::Get()->LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT), 30);

	// Define the item actual rectangle area
	wxRect itemRect(0, yCoord, m_parentMenu->GetMenuWidth(), itemHeight);

	// Define the drawing area 
	wxRect rect(2, yCoord, m_parentMenu->GetMenuWidth() - 4, itemHeight);

	// Draw the background
	wxColor backColor = wxArtManagerST::Get()->GetMenuFaceColour();
	wxColor penColor  = wxArtManagerST::Get()->GetMenuFaceColour();
	wxBrush backBrush( backColor );
	wxPen   pen( penColor );
	dc.SetPen( pen );
	dc.SetBrush( backBrush );
	dc.DrawRectangle( rect );

	// Draw the left margin gradient
	m_parentMenu->DrawLeftMargin(dc, itemRect);

	// check if separator
	if(IsSeparator())
	{
		// Separator is a small grey line separating between 
		// menu item. the separator height is 3 pixels
		int sepWidth = menuWidth - textX - 1;
		wxRect sepRect1(textX, yCoord + 1, sepWidth/2, 1);
		wxRect sepRect2(textX + sepWidth/2, yCoord + 1, sepWidth/2-1, 1);
	
		wxArtManagerST::Get()->PaintStraightGradientBox(dc, sepRect1, wxArtManagerST::Get()->GetMenuFaceColour(), wxT("LIGHT GREY"), false);
		wxArtManagerST::Get()->PaintStraightGradientBox(dc, sepRect2, wxT("LIGHT GREY"), wxArtManagerST::Get()->GetMenuFaceColour(), false);
		return;
	}

	// Keep the item rect
	m_rect = itemRect;

	// Get the bitmap base on the item state (disabled, selected ..)
	bmp = GetSuitableBitmap(selected);
	
	// First we draw the selection rectangle
	if(selected)
	{
		wxArtManagerST::Get()->SetMS2007ButtonSunken(false);
		wxArtManagerST::Get()->DrawButton(dc, rect, theme, ControlFocus, false);
	}
	
	if(bmp.Ok())
	{
		// Calculate the postion to place the image
		int xx, yy;
		int imgHeight = bmp.GetHeight();
		int imgWidth  = bmp.GetWidth();

		xx = rect.x + imageMarginX - 1;
		yy = rect.y + 1;
		wxRect rr(xx, yy, rect.height-2, rect.height-2);
		dc.DrawBitmap(bmp, rr.x + (rr.width - imgWidth)/2, rr.y + (rr.height - imgHeight)/2, true);
	}
	
	if(wxITEM_CHECK == GetKind())
	{
		// Checkable item
		if(IsChecked())
		{
			// Draw check 
			wxBitmap checkMarkBmp(check_mark_xpm);
			checkMarkBmp.SetMask(new wxMask(checkMarkBmp, *wxWHITE));

			// Draw surrounding rectangle around the selection box
			int xx, yy;
			xx = rect.x + 1;
			yy = rect.y + 1;
			wxRect rr(xx, yy, rect.height-2, rect.height-2);
			if( !selected ) {
				wxArtManagerST::Get()->SetMS2007ButtonSunken(false);
				wxArtManagerST::Get()->DrawButton(dc, rr, theme, ControlFocus, false);
			}
			dc.DrawBitmap(checkMarkBmp, rr.x + (rr.width - 16)/2, rr.y + (rr.height - 16)/2, true);
		}
	}
	
	if(wxITEM_RADIO == GetKind())
	{
		// Checkable item
		if(IsChecked())
		{
			// Draw check 
			wxBitmap checkMarkBmp(radio_item_xpm);
			checkMarkBmp.SetMask(new wxMask(checkMarkBmp, *wxWHITE));

			// Draw surrounding rectangle around the selection box
			int xx, yy;
			xx = rect.x + 1;
			yy = rect.y + 1;
			wxRect rr(xx, yy, rect.height-2, rect.height-2);
			if( !selected ) {
				wxArtManagerST::Get()->SetMS2007ButtonSunken(false);
				wxArtManagerST::Get()->DrawButton(dc, rr, theme, ControlFocus, false);
			}
			dc.DrawBitmap(checkMarkBmp, rr.x + (rr.width - 16)/2, rr.y + (rr.height - 16)/2, true);
		}
	}

	// Draw text - without accelerators
	wxString text = GetLabel();
	if( !text.IsEmpty() )
	{
		int w, h;
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		//wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("MIRIAM"), wxFONTENCODING_ISO8859_8);
		wxColour enabledTxtColour = wxArtManagerST::Get()->GetMenuTheme() == StyleXP ? *wxBLACK : wxColour(wxT("MIDNIGHT BLUE"));
		wxColour textColor = IsEnabled() ? enabledTxtColour : disableColor;

		dc.SetFont( font );
		dc.GetTextExtent(text, &w, &h);
		dc.SetTextForeground( textColor );

		if(wxNOT_FOUND != m_mnemonicIdx)
		{
			// We divide the drawing to 3 parts
			wxChar text1[512], text2[512], text3[512];
	
			memset(text1, 0, sizeof(text1));			
			memset(text2, 0, sizeof(text1));
			memset(text3, 0, sizeof(text1));

			int w1, w2, w3, dummy;
			wxStrcpy(text1, text.Mid(0, (size_t)m_mnemonicIdx).GetData());
			wxStrcpy(text2, text.Mid(m_mnemonicIdx, 1));
			wxStrcpy(text3, text.Mid((size_t)m_mnemonicIdx+1));

			dc.GetTextExtent(text1, &w1, &dummy);
			dc.GetTextExtent(text2, &w2, &dummy);
			dc.GetTextExtent(text3, &w3, &dummy);

			int posx = textX + borderXSize, posy = (itemHeight - h)/2 + yCoord;

			// Draw first part 
			dc.SetFont( font );
			dc.DrawText(text1, posx, posy);

			// mnemonic 
#ifndef __WXGTK__
			font.SetUnderlined( true );
			dc.SetFont( font );
#endif
			posx += w1;
			dc.DrawText(text2, posx, posy);

			// last part
			font.SetUnderlined( false );
			dc.SetFont( font );
			posx += w2;
			dc.DrawText(text3, posx, posy);
		}
		else
		{
			dc.SetFont( font );
			dc.GetTextExtent(text, &w, &h);
			dc.DrawText(text, textX + borderXSize, (itemHeight - h)/2 + yCoord);
		}
	}

	// Now draw accelerator
	// Accelerators are aligned to the right
	if( !GetAccelString().IsEmpty() )
	{
		int accelWidth, accelHeight;
		
		//wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("MIRIAM"), wxFONTENCODING_ISO8859_8);
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

		wxColour enabledTxtColour = wxArtManagerST::Get()->GetMenuTheme() == StyleXP ? *wxBLACK : wxColour(wxT("MIDNIGHT BLUE"));
		wxColour textColor = IsEnabled() ? enabledTxtColour : disableColor;
		dc.SetFont( font );
		dc.SetTextForeground( textColor );
		dc.GetTextExtent(GetAccelString(), &accelWidth, &accelHeight);
		dc.DrawText(GetAccelString(), rightMarginX - accelWidth, (itemHeight - accelHeight)/2 + yCoord);
	}

	// Check if this item has sub-menu - if it does, draw 
	// right arrow on the right margin
	if( NULL != GetSubMenu())
	{
		// Draw check 
		wxBitmap rightArrowBmp(menu_right_arrow_xpm);
		rightArrowBmp.SetMask(new wxMask(rightArrowBmp, *wxWHITE));

		int xx;
		xx = rightMarginX + borderXSize ;
		wxRect rr(xx, rect.y + 1, rect.height-2, rect.height-2);
		dc.DrawBitmap(rightArrowBmp, rr.x + 4, rr.y +(rr.height-16)/2, true);
	}
}

int wxFlatMenuItem::GetHeight() const 
{
	if(IsSeparator())
		return 3;
	else
		return m_parentMenu->m_itemHeight;
}

wxBitmap wxFlatMenuItem::GetSuitableBitmap(bool selected) const
{
	wxBitmap gBmp, normalBmp, hotBmp;
	normalBmp = m_normalBmp;
	gBmp = m_disabledBmp.Ok() ? m_disabledBmp : m_normalBmp;
	hotBmp = m_hotBmp.Ok() ? m_hotBmp : m_normalBmp;

	if(!IsEnabled())
		return gBmp;
	else if(selected)
		return hotBmp;
	else
		return normalBmp;
}
void wxFlatMenuItem::Enable(bool enable)
{
	m_bIsEnabled = enable; 
	if( m_parentMenu )
		m_parentMenu->UpdateItem(this);

}

void wxFlatMenuItem::SetLabel(const wxString& text)
{ 
	if(!text.IsEmpty())
	{
		wxString dummy;
		m_accelStr = text.AfterFirst(wxT('\t')); // Can be empty
		wxString label = text.BeforeFirst(wxT('\t')); 
		m_mnemonicIdx = wxArtManagerST::Get()->GetAccelIndex(label, m_label);
	}
	else
	{
		m_mnemonicIdx = wxNOT_FOUND;
		m_label = wxT("");
	}

	if( m_parentMenu )
		m_parentMenu->UpdateItem(this);
}

void wxFlatMenuItem::SetText(const wxString& text)
{ 
	m_text = text;
	SetLabel(m_text);
}

wxAcceleratorEntry* wxFlatMenuItem::GetAcceleratorEntry()
{
	return wxAcceleratorEntry::Create(GetText());
}

wxChar wxFlatMenuItem::GetMnemonicChar()
{
	if( m_mnemonicIdx == wxNOT_FOUND )
		return 0;
	wxChar mnemonic = m_label[m_mnemonicIdx];
	return wxTolower(mnemonic);
}

void wxFlatMenuItem::Check(bool check) 
{ 
	if(IsRadioItem() && !m_isAttachedToMenu)
	{
		// radio items can be checked only after they are attached to menu
		return;
	}

	m_bIsChecked = check; 

	// update group 
	if( IsRadioItem() && check)
	{
		m_groupPtr->SetSelection(this);
	}

	// Our parent menu might want to do something with this change
	if( m_parentMenu )
		m_parentMenu->UpdateItem(this);
}

//-----------------------------------------------------------------------------
// flat menu
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxFlatMenu, wxFlatMenuBase)

BEGIN_EVENT_TABLE(wxFlatMenu, wxFlatMenuBase)
EVT_PAINT(wxFlatMenu::OnPaint)
EVT_ERASE_BACKGROUND(wxFlatMenu::OnEraseBg)
EVT_MOTION(wxFlatMenu::OnMouseMove)
EVT_ENTER_WINDOW(wxFlatMenu::OnMouseEnterWindow)
EVT_LEAVE_WINDOW(wxFlatMenu::OnMouseLeaveWindow)
EVT_LEFT_DOWN(wxFlatMenu::OnMouseLeftDown)
EVT_LEFT_DCLICK(wxFlatMenu::OnMouseLeftDown)
EVT_LEFT_UP(wxFlatMenu::OnMouseLeftUp)
EVT_RIGHT_DOWN(wxFlatMenu::OnMouseRightDown)
EVT_KILL_FOCUS(wxFlatMenu::OnKillFocus)
EVT_TIMER(wxID_ANY, wxFlatMenu::OnTimer)
END_EVENT_TABLE()

wxFlatMenu::wxFlatMenu()
: wxFlatMenuBase()
, m_menuWidth( 2 * 26 )
, m_leftMarginWidth( 26 )
, m_rightMarginWidth( 30 )
, m_borderXWidth( 1 )
, m_borderYWidth( 2 )
, m_activeWin(NULL)
, m_focusWin(NULL)
, m_imgMarginX( 0 )
, m_markerMarginX( 0 )
, m_textX( 26 )
, m_rightMarginPosX( -1 )
, m_itemHeight( 20 )
, m_selectedItem( -1 )
, m_clearCurrentSelection(true)
, m_textPadding( 8 )
, m_marginHeight( 20 )
, m_marginWIdth( 26 )
, m_accelWidth( 0 )
, m_mb(NULL)
, m_resizeMenu(true)
, m_first(0)
{
	SetSize(wxSize(m_menuWidth, m_itemHeight+4));
}

wxFlatMenu::~wxFlatMenu()
{
	int size = (int)m_itemsArr.size(), i =0;
	
	for(; i<size; i++)
	{
		wxFlatMenuItem* item = (wxFlatMenuItem*)m_itemsArr[i];
		delete item;
	}

	if(m_upButton) 
	{
		delete m_upButton;
		m_upButton = NULL;
	}
	
	if(m_downButton) 
	{
		delete m_downButton;
		m_downButton = NULL;
	}
	m_itemsArr.clear();
}

void wxFlatMenu::SetMenuBar(wxFlatMenuBar *mb)
{
	m_mb = mb;
}

void wxFlatMenu::Popup(const wxPoint& pt, wxWindow* owner, wxFlatMenuBase* parent)
{
	// Loop over this menu and send update UI event for
	// every item in the menu
	int numEvents = (int)m_itemsArr.size();
	int cc = 0;

	// each time we popup, need to reset the starting index
	m_first = 0;

#ifdef __WXMSW__
	m_mousePtAtStartup = ::wxGetMousePosition();
#endif

	// Set the owner of the menu. All events will be directed to it.
	// If owner is NULL, the Default GetParent() is used as the owner
	m_owner = owner;

	for(; cc<numEvents; cc++)
		SendUIEvent(cc);

	// make sure that the menu has the right size before showing it
	ResizeMenu();

	// Adjust menu position and show it
	wxFlatMenuBase::Popup(pt, parent);

	// Replace the event handler of the active window to direct
	// all keyboard events to us and the focused window to direct char events to us
	m_activeWin = wxGetActiveWindow();
	if( m_activeWin )
	{
		wxEvtHandler* oldHandler = m_activeWin->GetEventHandler();
		wxMenuKbdRedirector* newEvtHandler = new wxMenuKbdRedirector(this, oldHandler);
		m_activeWin->PushEventHandler( newEvtHandler );
	}

#ifdef __WXMSW__
	m_focusWin = FindFocus();
#elif defined(__WXGTK__)
	m_focusWin = this;
#else
	m_focusWin = NULL;
#endif
	if( m_focusWin )
	{
		wxFocusHandler* newEvtHandler = new wxFocusHandler(this);
		m_focusWin->PushEventHandler( newEvtHandler );
	}
}

wxFlatMenuItem* wxFlatMenu::Append(int id, const wxString& item, const wxString& helpString, wxItemKind kind)
{
	wxFlatMenuItem* newItem = new wxFlatMenuItem(this, id, item, helpString, kind);
	return Append(newItem);
}

wxFlatMenuItem* wxFlatMenu::Append(int id, const wxString& item, wxFlatMenu *subMenu, const wxString& helpString)
{
	wxFlatMenuItem* newItem = new wxFlatMenuItem(this, id, item, helpString, wxITEM_NORMAL, subMenu);
	return Append(newItem);
}

// The main Append function
wxFlatMenuItem *wxFlatMenu::Append(wxFlatMenuItem *menuItem)
{
	wxASSERT_MSG(menuItem, wxT("Adding NULL item?"));
	// Reparent to us
	menuItem->SetMenu(this); 
	m_itemsArr.push_back( menuItem );
	menuItem->m_isAttachedToMenu = true;

	/// Update the menu width if necessary
	int menuItemWidth = GetMenuItemWidth(menuItem);
	m_menuWidth = m_menuWidth > menuItemWidth + m_accelWidth ? m_menuWidth : menuItemWidth + m_accelWidth;

	int menuHeight = 0, i = 0;
	for(; i<(int)m_itemsArr.size(); i++)
	{
		wxFlatMenuItem* item = m_itemsArr[i];
		if(item->IsSeparator())
			menuHeight += 3;
		else
			menuHeight += m_itemHeight;
	}

	SetSize(wxSize(m_menuWidth, menuHeight+4));

	// Add accelerator entry to the menu if needed
	wxAcceleratorEntry* accel = menuItem->GetAcceleratorEntry();
	if( accel )
	{
		accel->Set(accel->GetFlags(), accel->GetKeyCode(), menuItem->GetId());
		m_accelArray.push_back( *accel );
		delete accel;
	}

	UpdateRadioGroup(menuItem);
	return menuItem;
}

int wxFlatMenu::GetMenuItemWidth(wxFlatMenuItem *menuItem)
{
	int menuItemWidth = 0;
	wxString text = menuItem->GetLabel(); // Without accelerator
	wxString accel = menuItem->GetAccelString();

	wxClientDC dc(this);

//	wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("MIRIAM"), wxFONTENCODING_ISO8859_8);
	wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	dc.SetFont(font);
	int width, height, dummy, itemHeight, accelWidth = 0, accelHeight, filler;
	wxString stam = wxT("Tp");
	wxString accelFiller = wxT("XXXX"); // 4 spaces betweem text and accel column

	// Calc text length/height
	dc.GetMultiLineTextExtent(stam, &dummy, &itemHeight);
	dc.GetMultiLineTextExtent(text, &width, &height);
	dc.GetMultiLineTextExtent(accel, &accelWidth, &accelHeight);
	dc.GetMultiLineTextExtent(accelFiller, &filler, &dummy);
	
	int bmpHeight = 0, bmpWidth = 0;
	if(menuItem->GetBitmap().Ok())
	{
		bmpHeight = menuItem->GetBitmap().GetHeight();
		bmpWidth  = menuItem->GetBitmap().GetWidth();
	}

	if( itemHeight < m_marginHeight )
		itemHeight = m_marginHeight;

	itemHeight = bmpHeight > m_itemHeight ? bmpHeight : itemHeight;
	itemHeight += 2*m_borderYWidth;

	// Update the global menu item height if needed
	m_itemHeight = m_itemHeight > itemHeight ? m_itemHeight : itemHeight;
	m_marginWIdth = m_marginWIdth > bmpWidth ? m_marginWIdth : bmpWidth;

	// Update the accel width
	accelWidth += filler;
	if(!accel.IsEmpty())
        m_accelWidth = m_accelWidth > accelWidth ? m_accelWidth : accelWidth;

	// Incase the item has image & is type radio or check, we need double size
	// left margin
	int factor = ((menuItem->GetBitmap().Ok()) && (menuItem->IsCheckable() || (menuItem->GetKind() == wxITEM_RADIO))) ? 2 : 1;
	if(factor == 2)
	{
		m_imgMarginX = m_marginWIdth + 2*m_borderXWidth;
		m_leftMarginWidth = 2 * m_marginWIdth + 2*m_borderXWidth;
	}
	else	
		m_leftMarginWidth = (m_leftMarginWidth > m_marginWIdth + 2*m_borderXWidth) ? m_leftMarginWidth : m_marginWIdth + 2*m_borderXWidth;
	
	menuItemWidth = GetLeftMarginWidth() + GetBorderXWidth() + width + GetBorderXWidth() + GetRightMarginWidth();
	m_textX = m_imgMarginX + m_marginWIdth + m_textPadding;

	// update the rightMargin X position
	m_rightMarginPosX = (m_textX + width + m_accelWidth> m_rightMarginPosX) ? m_textX + width + m_accelWidth : m_rightMarginPosX;
	
	return menuItemWidth;
}

wxFlatMenuItem* wxFlatMenu::AppendCheckItem(int id, const wxString& item, const wxString& helpString)
{
	wxFlatMenuItem* newItem = new wxFlatMenuItem(this, id, item, helpString, wxITEM_CHECK);
	return Append(newItem);
}

wxFlatMenuItem* wxFlatMenu::AppendRadioItem(int id, const wxString& item, const wxString& helpString)
{
	wxFlatMenuItem* newItem = new wxFlatMenuItem(this, id, item, helpString, wxITEM_RADIO);
	return Append(newItem);
}

wxFlatMenuItem* wxFlatMenu::AppendSeparator()
{
	wxFlatMenuItem* newItem = new wxFlatMenuItem(this);
	return Append(newItem);
}

void wxFlatMenu::Dismiss(bool dismissParent, bool resetOwner)
{
	if( m_activeWin )
	{
		m_activeWin->PopEventHandler( true );
		m_activeWin = NULL;
	}

	if( m_focusWin )
	{
		m_focusWin->PopEventHandler( true );
		m_focusWin = NULL;
	}

	m_selectedItem = -1;
	wxFlatMenuBase::Dismiss(dismissParent, resetOwner);
}

void wxFlatMenu::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxPaintDC dc(this);
	DoDrawMenu( dc );

	// We need to redraw all our child menus
	RefreshChilds();
}

void wxFlatMenu::UpdateItem(wxFlatMenuItem* item)
{
	// notify menu bar that an item was modified directly
	if(item && m_mb)
		m_mb->UpdateItem(item);
}

void wxFlatMenu::DoDrawMenu(wxDC& dc)
{
	wxRect menuRect = GetMenuRect();
	wxBitmap menuBmp(menuRect.width, menuRect.height);

	wxMemoryDcEx mem_dc;
	mem_dc.SelectObject( menuBmp );

	// color the menu face with background color
	wxColor backColor = wxArtManagerST::Get()->GetMenuFaceColour();
	wxColor penColor  = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);

	wxBrush backBrush( backColor );
	wxPen   pen( penColor );

	mem_dc.SetPen( pen );
	mem_dc.SetBrush( backBrush );
	mem_dc.DrawRectangle( menuRect );
	
	// draw items
	int posy = 2;
	int nItems = (int)m_itemsArr.size();
	int nCount = 0;

	// make all items as non-visible first
	for(nCount=0; nCount < nItems; nCount++)
	{
		m_itemsArr[nCount]->Show(false);
	}

	int visibleItems = 0;
	for(nCount=m_first; nCount < nItems; nCount++)
	{
		visibleItems++;
		wxFlatMenuItem* item = m_itemsArr[nCount];
		item->DrawSelf(	mem_dc,			
						posy,		
						m_imgMarginX,
						m_markerMarginX,
						m_textX, 
						m_rightMarginPosX,
						nCount == m_selectedItem 
						);
		posy += item->GetHeight();
		item->Show();

		// make sure we draw only visible items
		wxPoint pp = ClientToScreen(wxPoint(0, posy));
		static int screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);

		if(pp.y > screenHeight) 
			break;
	}

	if(m_showScrollButtons)
	{
		if(m_upButton)
			m_upButton->Draw(mem_dc);
		if(m_downButton)
			m_downButton->Draw(mem_dc);
	}
	dc.Blit(0, 0, menuBmp.GetWidth(), menuBmp.GetHeight(), &mem_dc, 0, 0);
}

void wxFlatMenu::DrawSelection(wxDC& WXUNUSED(dc), int WXUNUSED(oldSelection))
{
	Refresh();
}

void wxFlatMenu::RefreshChilds()
{
	// Draw all childs menus of this menu as well
	wxFlatMenu* child = (wxFlatMenu*)m_openedSubMenu;
	while( child )
	{
		wxClientDC dc(child);
		child->DoDrawMenu( dc );
		child = (wxFlatMenu*)child->m_openedSubMenu;
	}
}

void wxFlatMenu::DrawLeftMargin(wxDC& dc, wxRect& menuRect)
{
	// Construct the margin rectangle
	wxRect marginRect(menuRect.x+1, menuRect.y, GetLeftMarginWidth(), menuRect.height);

	// Set the gradient colors
	if( Style2007 == wxArtManagerST::Get()->GetMenuTheme())
	{
		wxDCSaver dcsaver(&dc);
		wxColor marginColor = wxArtManagerST::Get()->DarkColour(wxArtManagerST::Get()->GetMenuFaceColour(), 5);
		dc.SetPen( wxPen(marginColor) );
		dc.SetBrush( wxBrush(marginColor) );
		dc.DrawRectangle(marginRect);
		dc.SetPen( *wxWHITE_PEN );
		dc.DrawLine(marginRect.x + marginRect.width, marginRect.y, marginRect.x + marginRect.width, marginRect.y + marginRect.height);

		wxColor borderColor = wxArtManagerST::Get()->DarkColour(wxArtManagerST::Get()->GetMenuFaceColour(), 10);
		dc.SetPen( wxPen(borderColor) );
		dc.DrawLine(marginRect.x + marginRect.width-1, marginRect.y, marginRect.x + marginRect.width-1, marginRect.y + marginRect.height);
	}
	else
	{
		static wxColor startColor = wxArtManagerST::Get()->DarkColour(wxArtManagerST::Get()->GetMenuFaceColour(), 20);
		static wxColor endColor   = wxArtManagerST::Get()->GetMenuFaceColour();
		wxArtManagerST::Get()->PaintStraightGradientBox(dc, marginRect, endColor, startColor, false);
	}
}

wxRect wxFlatMenu::GetMenuRect() const 
{
	wxRect menuRect( GetClientRect() );
	return menuRect;
}

void wxFlatMenu::OnKeyDown(wxKeyEvent& event)
{
	OnChar( event.GetKeyCode() );
}

bool wxFlatMenu::OnChar(int key)
{
	bool processed = true;
	switch(key)
	{
	case WXK_ESCAPE:
		{
			if( m_parentMenu )
				((wxFlatMenu*)m_parentMenu)->CloseSubMenu(-1);
			else
				Dismiss(true, true);
			break;
		}
	case WXK_LEFT:
		{
			if(m_parentMenu)
			{
				// We are a submenu, dismiss us.
				((wxFlatMenu*)m_parentMenu)->CloseSubMenu(-1);
				break;
			}

			// try to find our root menu, if we are attached to menubar,
			// let it try and open the previous menu
			wxFlatMenu* root = static_cast<wxFlatMenu*>(GetRootMenu());
			if( root ) {
				if (root->m_mb) {
					root->m_mb->ActivatePreviousMenu();
				}
			}
		}
		break;
	case WXK_RIGHT:
		{
			if( TryOpenSubMenu(	m_selectedItem, // Item to try
											true))			// Select first item of the sub-menu
				break;

			// try to find our root menu, if we are attached to menubar,
			// let it try and open the previous menu
			wxFlatMenu* root = static_cast<wxFlatMenu*>(GetRootMenu());
			if( root ) {
				if (root->m_mb) {
					root->m_mb->ActivateNextMenu();
				}
			}
		}
		break;
	case WXK_UP:
		{
			AdvanceSelection(false);
			break;
		}
	case WXK_DOWN:
		{
			AdvanceSelection();
			break;
		}
	case WXK_RETURN:
	case WXK_NUMPAD_ENTER:
		{
			DoAction(m_selectedItem);
			break;
		}
	case WXK_HOME:
		{
			// Select first item of the menu
			if(m_selectedItem == 0)
				break;

			int oldSel = m_selectedItem;
			m_selectedItem = 0;
			wxClientDC dc(this);
			DrawSelection(dc, oldSel);
			break;
		}
	case WXK_END:
		{
			// Select last item of the menu
			if(m_selectedItem == (int)m_itemsArr.size()-1)
				break;

			int oldSel = m_selectedItem;
			m_selectedItem = (int)m_itemsArr.size()-1;
			wxClientDC dc(this);
			DrawSelection(dc, oldSel);
			break;
		}
	case WXK_CONTROL:
	case WXK_ALT:
		{
			// Alt was pressed
			wxFlatMenuBase* root = GetRootMenu();
			root->Dismiss(false, true);
			break;
		}
	default:
		{
			if ( wxIsalnum((wxChar)key) )
			{
				wxChar ch = (wxChar)key;
				ch = wxTolower(ch);

				// Iterate over all the menu items 
				int i=0, itemIdx = -1, occur = 0;
				for(; i<(int)m_itemsArr.size(); i++)
				{
					wxFlatMenuItem* item = m_itemsArr[i];
					wxChar mnemonic = item->GetMnemonicChar();
					if(mnemonic == ch)
					{
						if(itemIdx == -1)
						{
							itemIdx = i;
							// We keep the index of only 
							// the first occurence
						}
						occur++;

						// Keep on looping until no more items for this menu
					}
				}

				if( itemIdx != -1 )
				{
					if(occur > 1)
					{
						// We select the first item
						if(m_selectedItem == itemIdx)
							break;

						int oldSel = m_selectedItem;
						m_selectedItem = itemIdx;
						wxClientDC dc(this);
						DrawSelection(dc, oldSel);
					}
					else if(occur == 1)
					{
						// Activate the item, if this is a submenu item we first select it
						wxFlatMenuItem* item = m_itemsArr[itemIdx];
						if(item->IsSubMenu() && m_selectedItem != itemIdx)
						{
							int oldSel = m_selectedItem;
							m_selectedItem = itemIdx;
							wxClientDC dc(this);
							DrawSelection(dc, oldSel);
						}
						DoAction(itemIdx);
					}
					
				}
				else
				{
					processed = false;
					break;
				}
			}
		}
		
		break;
	}
	return processed;
}

void wxFlatMenu::AdvanceSelection(bool down)
{
	// make sure we have at least two items in the menu (which are not 
	// separators)
	int i=0, num=0;
	int singleItemIdx( -1 );

	for(; i<(int)m_itemsArr.size(); i++)
	{
		wxFlatMenuItem* item = m_itemsArr[i];
		if(item->IsSeparator())
			continue;
		num++;
		singleItemIdx = i;
	}

	if(num < 1)
		return;

	if( num == 1 ) {
		// Select the current one
		m_selectedItem = singleItemIdx;
		wxClientDC dc(this);
		DrawSelection(dc, -1);
		return;
	}

	int oldSelection = m_selectedItem;
	if(!down)
	{
		// find the next valid item
		while(true)
		{
			m_selectedItem--;
			if(m_selectedItem < 0)
				m_selectedItem = (int)m_itemsArr.size()-1;
			if(!m_itemsArr[m_selectedItem]->IsSeparator())
				break;
		}
	}
	else
	{
		// find the next valid item
		while(true)
		{
			m_selectedItem++;
			if(m_selectedItem > (int)m_itemsArr.size()-1)
				m_selectedItem = 0;
			if(!m_itemsArr[m_selectedItem]->IsSeparator())
				break;
		}
	}
	wxClientDC dc(this);
	DrawSelection(dc, oldSelection);
}

wxFlatMenuHT wxFlatMenu::HitTest(wxPoint& pos, int& itemIdx)
{
	itemIdx = -1;
	if(m_showScrollButtons)
	{
		if(m_upButton && m_upButton->GetClientRect().Contains(pos))
			return wx_MENU_HT_SCROLL_UP;
	
		if(m_downButton && m_downButton->GetClientRect().Contains(pos))
			return wx_MENU_HT_SCROLL_DOWN;
	}

	for(int n=0; n<(int)m_itemsArr.size(); n++)
	{
		wxFlatMenuItem* item = m_itemsArr[n];
		if(item->GetRect().Contains(pos) && item->IsEnabled() && item->IsShown())
		{
			itemIdx = n;
			return wx_MENU_HT_ITEM;
		}
	}
	return wx_MENU_HT_NONE;
}

void wxFlatMenu::OnMouseMove(wxMouseEvent& event)
{
#ifdef __WXMSW__
	// Ignore dummy mouse move events
	wxPoint pt = ::wxGetMousePosition();
	if(m_mousePtAtStartup == pt)
		return;
#endif

	wxPoint pos = event.GetPosition();
	// we need to ignore extra mouse events: example when this happens is when
	// the mouse is on the menu and we open a submenu from keyboard - Windows
	// then sends us a dummy mouse move event, we (correctly) determine that it
	// happens in the parent menu and so immediately close the just opened
	// submenu!
#ifdef __WXMSW__
	static wxPoint s_ptLast;
	wxPoint ptCur = ClientToScreen(pos);
	if ( ptCur == s_ptLast )
	{
		return;
	}

	s_ptLast = ptCur;
#endif // __WXMSW__

	// first let the scrollbar handle it
	TryScrollButtons(event);

	ProcessMouseMove(pos);
}

void wxFlatMenu::OnMouseLeftDown(wxMouseEvent& event)
{
	if(TryScrollButtons(event))
		return;

	FNB_LOG_MSG(wxT("EVENT: ") << event.GetEventType());
	wxPoint pos = event.GetPosition();
	ProcessMouseLClick(pos);
}

void wxFlatMenu::OnMouseLeftUp(wxMouseEvent& event)
{
	if(TryScrollButtons(event))
		return;

	wxPoint pos = event.GetPosition();
	wxRect rect(GetClientRect());
	if(!rect.Contains(pos))
	{
		// The event is not in our coords, 
		// so we try our parent
		wxFlatMenu* win = (wxFlatMenu*)m_parentMenu;
		while(win)
		{
			// we need to translate our client coords to the client coords of the
			// window we forward this event to
			wxPoint ptScreen = ClientToScreen(pos);

			wxPoint p = win->ScreenToClient(ptScreen);
			if(win->GetClientRect().Contains(p))
			{
				event.m_x = p.x;
				event.m_y = p.y;
				FNB_LOG_MSG(wxT("EVENT: ") << event.GetEventType());
				win->OnMouseLeftUp(event);
				return;
			}
			else
				// try the grandparent
				win = (wxFlatMenu*)win->m_parentMenu;
		}
	}
	if(m_showScrollButtons)
	{
		if(m_upButton) m_upButton->ProcessLeftUp(pos);
		if(m_downButton) m_downButton->ProcessLeftUp(pos);
	}
}

void wxFlatMenu::OnMouseRightDown(wxMouseEvent& event)
{
	if(TryScrollButtons(event))
		return;

	wxPoint pos = event.GetPosition();
	ProcessMouseRClick(pos);
}

void wxFlatMenu::ProcessMouseRClick(wxPoint& pos)
{	
	wxRect rect(GetClientRect());
	if(!rect.Contains(pos))
	{
		// The event is not in our coords, 
		// so we try our parent

		wxFlatMenu* win = (wxFlatMenu*)m_parentMenu;
		while(win)
		{
			// we need to translate our client coords to the client coords of the
			// window we forward this event to
			wxPoint ptScreen = ClientToScreen(pos);

			wxPoint p = win->ScreenToClient(ptScreen);
			if(win->GetClientRect().Contains(p))
			{
				win->ProcessMouseRClick(p);
				return;
			}
			else
				// try the grandparent
				win = (wxFlatMenu*)win->m_parentMenu;
		}
		// At this point we can assume that the event was not 
		// processed, so we dismiss the menu and its children
		Dismiss(true, true);
		return;
	}
	

	// else we do nothing ...
}

void wxFlatMenu::ProcessMouseLClick(wxPoint& pos)
{
	wxRect rect(GetClientRect());
	if(!rect.Contains(pos))
	{
		// The event is not in our coords, 
		// so we try our parent

		wxFlatMenu* win = (wxFlatMenu*)m_parentMenu;
		while(win)
		{
			// we need to translate our client coords to the client coords of the
			// window we forward this event to
			wxPoint ptScreen = ClientToScreen(pos);

			wxPoint p = win->ScreenToClient(ptScreen);
			if(win->GetClientRect().Contains(p))
			{
				win->ProcessMouseLClick(p);
				return;
			}
			else
				// try the grandparent
				win = (wxFlatMenu*)win->m_parentMenu;
		}
		// At this point we can assume that the event was not 
		// processed, so we dismiss the menu and its children
		Dismiss(true, true);
		return;
	}

	// test if we are on a menu item
	int itemIdx;
	wxFlatMenuHT res = HitTest(pos, itemIdx);
	switch( res )
	{
	case wx_MENU_HT_ITEM:
		DoAction(itemIdx);
		break;
	case wx_MENU_HT_SCROLL_UP:
		if(m_upButton)
			m_upButton->ProcessLeftDown(pos);
		break;
	case wx_MENU_HT_SCROLL_DOWN:
		if(m_downButton) 
			m_downButton->ProcessLeftDown(pos);
		break;
	default:
		m_selectedItem = -1;
		break;
	}
}

void wxFlatMenu::ProcessMouseMove(wxPoint& pos)
{
	wxRect rect(GetClientRect());
	if(!rect.Contains(pos))
	{
		// The event is not in our coords, 
		// so we try our parent

		wxFlatMenu* win = (wxFlatMenu*)m_parentMenu;
		while(win)
		{
			// we need to translate our client coords to the client coords of the
			// window we forward this event to
			wxPoint ptScreen = ClientToScreen(pos);

			wxPoint p = win->ScreenToClient(ptScreen);
			if(win->GetClientRect().Contains(p))
			{
				win->ProcessMouseMove(p);
				return;
			}
			else
				// try the grandparent
				win = (wxFlatMenu*)win->m_parentMenu;
		}

		// If we are attached to a menu bar, 
		// let him process the event as well
		if( m_mb )
		{
			wxPoint ptScreen = ClientToScreen(pos);
			wxPoint p = m_mb->ScreenToClient( ptScreen );
			if( m_mb->GetClientRect().Contains(p) )
			{
				// let the menu bar process it
				m_mb->ProcessMouseMoveFromMenu( p );
				return;
			}
		}
		return;
	}

	// test if we are on a menu item
	int itemIdx;
	wxFlatMenuHT res = HitTest(pos, itemIdx);
	switch( res )
	{
	case wx_MENU_HT_SCROLL_DOWN:
		{
			if(m_downButton)
				m_downButton->ProcessMouseMove(pos);
		}
		break;
	case wx_MENU_HT_SCROLL_UP:
		{
			if(m_upButton)
				m_upButton->ProcessMouseMove(pos);
		}
		break;
	case wx_MENU_HT_ITEM:
		{
			if(m_downButton)
				m_downButton->ProcessMouseMove(pos);

			if(m_upButton)
				m_upButton->ProcessMouseMove(pos);

			if(m_selectedItem == itemIdx)
				break;

			int oldSelection = m_selectedItem;
			m_selectedItem = itemIdx;
			CloseSubMenu(m_selectedItem);			

			wxClientDC dc(this);
			DrawSelection(dc, oldSelection);

			TryOpenSubMenu(m_selectedItem);
		}
		break;
	default:
		{
			int oldSelection = m_selectedItem;
			m_selectedItem = -1;
			wxClientDC dc(this);
			DrawSelection(dc, oldSelection);
		}
		break;
	}
}

void wxFlatMenu::OnMouseLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
	if(m_clearCurrentSelection)
	{
		int oldSelection = m_selectedItem;
		m_selectedItem = -1;
		wxClientDC dc(this);
		DrawSelection(dc, oldSelection);
	}
	m_clearCurrentSelection = true;

#ifdef __WXMSW__
	SetCursor(m_oldCur);
#endif
}

void wxFlatMenu::OnMouseEnterWindow(wxMouseEvent& event)
{
#ifdef __WXMSW__
	m_oldCur = GetCursor();
	SetCursor(wxCURSOR_ARROW);
#endif
	event.Skip();
}

void wxFlatMenu::OnKillFocus(wxFocusEvent &event)
{
	wxUnusedVar(event);
	Dismiss(true, true);
}

void wxFlatMenu::CloseSubMenu(int itemIdx)
{
	wxFlatMenuItem* item = NULL;
	wxFlatMenu* subMenu = NULL;

	if( itemIdx >= 0 && itemIdx < (int)m_itemsArr.size())
		item = m_itemsArr[itemIdx];

	// Close sub-menu first
	if( item )
		subMenu = item->GetSubMenu();

	if(m_openedSubMenu && m_openedSubMenu != subMenu)
	{
		// We have another sub-menu open, close it 
		m_openedSubMenu->Dismiss(false, true);
		m_openedSubMenu = NULL;
	}
}

void wxFlatMenu::DoAction(int itemIdx)
{
	VALIDATE_IDX(itemIdx);

	wxFlatMenuItem* item = m_itemsArr[itemIdx];
	
	if(!item->IsEnabled() || item->IsSeparator())
		return;

	// Close sub-menu if needed
	CloseSubMenu(itemIdx);
	if(item->IsSubMenu() && !item->GetSubMenu()->IsShown())
	{
		// Popup child menu
		TryOpenSubMenu(itemIdx);
		return;
	}

	if(item->IsRadioItem())
	{
		// if the radio item is already checked, 
		// just send command event. Else, check it, uncheck the current
		// checked item in the radio item group, and send command event
		if(!item->IsChecked())
		{
			item->m_groupPtr->SetSelection(item);
		}
	}
	else if(item->IsCheckable())
	{
		item->Check(!item->IsChecked());
		wxClientDC dc(this);
		DrawSelection(dc);
	}

	if(!item->IsSubMenu())
	{
		Dismiss(true, false);

		// Send command event
		SendCmdEvent( itemIdx );
	}
}

bool wxFlatMenu::TryOpenSubMenu(int itemIdx, bool selectFirst)
{
	VALIDATE_IDX_BOOL(itemIdx);

	wxFlatMenuItem* item = m_itemsArr[itemIdx];
	if(item->IsSubMenu() && !item->GetSubMenu()->IsShown())
	{
		// Popup child menu
		wxPoint pos;
		pos.x = item->GetRect().GetWidth()+ item->GetRect().GetX() - 5;
		pos.y = item->GetRect().GetY();
		m_clearCurrentSelection = false;
		m_openedSubMenu = item->GetSubMenu();
		item->GetSubMenu()->Popup(pos, m_owner, this);
		
		// Select the first child
		if(selectFirst)
		{
			wxClientDC dc(item->GetSubMenu());
			item->GetSubMenu()->m_selectedItem = 0;
			item->GetSubMenu()->DrawSelection(dc);
		}
		return true;
	}
	return false;
}

wxFlatMenuItem* wxFlatMenu::Remove(int id)
{
	// First we search for the menu item (recursivley)
	wxFlatMenu* menuParent = NULL;
	wxFlatMenuItem* item = NULL;
	int idx = wxNOT_FOUND;
	idx = FindMenuItemPos(id, &menuParent);
	if( wxNOT_FOUND != idx )
	{
		// Remove the menu item
		item = menuParent->m_itemsArr[idx];
		menuParent->m_itemsArr.erase(menuParent->m_itemsArr.begin()+ idx);
		
		// update group
		if( item->m_groupPtr.Ok() && item->IsRadioItem())
			item->m_groupPtr->Remove(item);

		// Resize the menu
		menuParent->ResizeMenu();
	}
	return item;
}

wxFlatMenuItem* wxFlatMenu::Remove(wxFlatMenuItem *item)
{
	wxASSERT_MSG(item, wxT("Removing NULL item?"));
	return Remove(item->GetId());
}

void wxFlatMenu::Destroy(int id)
{
	wxFlatMenuItem* item = NULL;
	item = Remove(id);
	if( item )
		delete item;
}

void wxFlatMenu::Destroy(wxFlatMenuItem *item)
{
	wxASSERT_MSG(item, wxT("Removing NULL item?"));
	Destroy(item->GetId());
}

wxFlatMenuItem* wxFlatMenu::Insert(size_t pos, int id, const wxString& item, const wxString& helpString, wxItemKind kind)
{
	wxFlatMenuItem* newitem = new wxFlatMenuItem(this, id, item, helpString, kind);
	return Insert(pos, newitem);
}

wxFlatMenuItem* wxFlatMenu::Insert(size_t pos, wxFlatMenuItem *item)
{
	wxASSERT_MSG(item, wxT("Inserting NULL item?"));
	wxASSERT_MSG((pos > m_itemsArr.size()), wxT("pos is out of bounds"));

	if(pos == m_itemsArr.size())
		// Append it
		return Append(item);

	// Insert the menu item 
	m_itemsArr.insert(m_itemsArr.begin() + pos, item);
	item->m_isAttachedToMenu = true;

	// Recalcualte the menu gemotry
	ResizeMenu();

	// update radio group
	UpdateRadioGroup(item);
	return item;
}

void wxFlatMenu::UpdateRadioGroup(wxFlatMenuItem *item)
{
	if(item->IsRadioItem())
	{
		// Udpate radio groups incase this item is a radio item
		wxFlatMenuItem *sibling = GetSiblingGroupItem(item);
		if( sibling )
		{
			item->m_groupPtr = sibling->m_groupPtr;
			item->m_groupPtr->Add(item);

			if(item->IsChecked())
			{
				item->m_groupPtr->SetSelection(item);
			}
		}
		else
		{
			// first item in group
			item->m_groupPtr = wxFlatMenuItemGroupPtr(new wxFlatMenuItemGroup());
			item->m_groupPtr->Add(item);
			item->m_groupPtr->SetSelection(item);
		}
	}
}

void wxFlatMenu::ResizeMenu()
{
	// can we do the resize?
	if( !m_resizeMenu )
		return;

	std::vector<wxFlatMenuItem*> items( m_itemsArr );
	m_itemsArr.clear();

	// Clear accelerator table
	m_accelArray.clear();

	// Reset parameters and menu size
	m_menuWidth =  2 * m_marginWIdth;
	m_imgMarginX = 0;
	m_markerMarginX = 0;
	m_textX = m_marginWIdth;
	m_rightMarginPosX = -1;
	m_itemHeight = m_marginHeight;
	SetSize(wxSize(m_menuWidth, m_itemHeight+4));

	// Now we simply adding the items 
	int i=0;
	for(; i<(int)items.size(); i++)
		Append(items[i]);
}

wxFlatMenuItem * wxFlatMenu::FindItem(int itemId, wxFlatMenu **menu) const
{
	int idx = wxNOT_FOUND;
	if( menu )
	{
		idx = FindMenuItemPos(itemId, menu);
		if( wxNOT_FOUND != idx )
			return (*menu)->m_itemsArr[idx];
		else
			return NULL;
	}
	else
	{
		wxFlatMenu *parentMenu = NULL;
		idx = FindMenuItemPos(itemId, &parentMenu);
		if( wxNOT_FOUND != idx )
			return parentMenu->m_itemsArr[idx];
		else
			return NULL;
	}
}

int wxFlatMenu::FindMenuItemPos(int itemId, wxFlatMenu **menu) const
{
	wxASSERT_MSG(menu, wxT("menu is NULL"));
	*menu = NULL;

    wxFlatMenuItem *item = NULL;
	int i = 0;
	int idx = wxNOT_FOUND;

	for (; i < (int)m_itemsArr.size(); i++ )
    {
		item = m_itemsArr[i];
        if (item->GetId() == itemId)
        {
            *menu = (wxFlatMenu *)this;
			idx = i;
			break;
        }
        else if ( item->IsSubMenu() )
        {
            idx = item->GetSubMenu()->FindMenuItemPos(itemId, menu);
			if( wxNOT_FOUND != idx )
				break;
        }
        else
        {
            item = NULL;
        }
    }

	return idx;
}

wxAcceleratorTable wxFlatMenu::GetAccelTable()
{
	int n = (int)m_accelArray.size();
	if( n == 0 )
		return wxNullAcceleratorTable;
	wxAcceleratorEntry *entries = new wxAcceleratorEntry[ n ];
	int counter = 0;
	for(; counter < n; counter++)
		entries[counter] = m_accelArray[counter];
	
	wxAcceleratorTable table(n, entries);
	delete [] entries;

	return table;
}

std::vector<wxAcceleratorEntry>& wxFlatMenu::GetAccelArray()
{
	return m_accelArray;
}

// events 
void wxFlatMenu::SendCmdEvent(int itemIdx)
{
	VALIDATE_IDX(itemIdx);
	wxFlatMenuItem* item = m_itemsArr[itemIdx];

	// Create the event
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, item->GetId());

	// For checkable item, set the IsChecked() value
	if(item->IsCheckable())
		event.SetInt( item->IsChecked() ? 1 : 0 );
	
	event.SetEventObject(this);

	if(m_owner)
		m_owner->GetEventHandler()->ProcessEvent(event);
	else
		GetEventHandler()->ProcessEvent(event);
}

void wxFlatMenu::SendUIEvent(int itemIdx)
{
	VALIDATE_IDX(itemIdx);
	wxFlatMenuItem* item = m_itemsArr[itemIdx];
	wxUpdateUIEvent event(item->GetId());

	event.Check(item->IsChecked());
	event.Enable(item->IsEnabled());
	event.SetText(item->GetText());
	event.SetEventObject(this);

	if(m_owner)
		m_owner->GetEventHandler()->ProcessEvent(event);
	else
		GetEventHandler()->ProcessEvent(event);

	item->Check( event.GetChecked() ) ;
	item->SetLabel( event.GetText() );
	item->Enable( event.GetEnabled() );
}

void wxFlatMenu::Clear()
{
	// since Destroy() call ResizeMenu(), we turn this flag on
	// to avoid resizing the menu for every item removed
	m_resizeMenu = false;

	std::vector<wxFlatMenuItem*>::iterator iter = Begin();
	std::vector<wxFlatMenuItem*>::size_type total = m_itemsArr.size();

	for(std::vector<wxFlatMenuItem*>::size_type i=0; i<total; i++){
		Destroy(m_itemsArr[0]->GetId());
	}

	// Now we can resize the menu
	m_resizeMenu = true;
	ResizeMenu();
}

int wxFlatMenu::FindMenuItemPos(wxFlatMenuItem *item) const
{
	if (item == NULL || m_itemsArr.empty())
		return wxNOT_FOUND;

	for (size_t i = 0; i<m_itemsArr.size(); i++)
	{
		if(m_itemsArr[i] == item)
			return (int)i;
	}
	return wxNOT_FOUND;
}

void wxFlatMenu::GetAllItems(std::vector<wxFlatMenuItem*>& items) 
{
	GetAllItems(this, items);
}

void wxFlatMenu::GetAllItems(wxFlatMenu *menu, std::vector<wxFlatMenuItem*>& items) 
{
	// first copy the current menu items
	items.insert(items.end(), menu->m_itemsArr.begin(), menu->m_itemsArr.end());

	// if any item in this menu has sub-menu, copy them as well
	std::vector<wxFlatMenuItem*>::iterator iter = menu->m_itemsArr.begin();
	for(size_t i=0; i<menu->m_itemsArr.size(); i++)
	{
		if(menu->m_itemsArr[i]->IsSubMenu())
		{
			GetAllItems(menu->m_itemsArr[i]->GetSubMenu(), items);
		}
	}
}

wxFlatMenuItem *wxFlatMenu::GetSiblingGroupItem(wxFlatMenuItem *item) const
{
	int pos = FindMenuItemPos(item);
	if(pos == wxNOT_FOUND)
		return NULL;

	if(pos == 0)
		return NULL;

	if(m_itemsArr[pos-1]->IsRadioItem())
		return m_itemsArr[pos-1];
	return NULL;
}

bool wxFlatMenu::ScrollDown()
{
	// increase the m_from index
	if(!m_itemsArr[m_itemsArr.size()-1]->IsShown())
	{
		FNB_LOG_MSG(wxT("Scrolling Down"));
		m_first++;
		Refresh();
		return true;
	}
	else
	{
		if(m_downButton ) m_downButton->GetTimer()->Stop();
		FNB_LOG_MSG(wxT("Cant scroll down anymore!"));
		return false;
	}
}

bool wxFlatMenu::ScrollUp()
{
	if(m_first == 0)
	{
		FNB_LOG_MSG(wxT("Cant scroll up anymore!"));
		if( m_upButton ) m_upButton->GetTimer()->Stop();
		return false;
	}	
	else
	{
		m_first--;
		FNB_LOG_MSG(wxT("Scrolling up!"));
		Refresh();
		return true;
	}
}   

// Not used anymore
bool wxFlatMenu::TryScrollButtons(wxMouseEvent & WXUNUSED(event))
{
	return false;
}

void wxFlatMenu::OnTimer(wxTimerEvent &event)
{
	if(m_upButton && m_upButton->GetTimerId() == event.GetId())
	{
		ScrollUp();
	}
	else if(m_downButton && m_downButton->GetTimerId() == event.GetId())
	{
		ScrollDown();
	}
	else
		event.Skip();
}

//----------------------------------------------------------
// wxMenuKbdRedirector
//----------------------------------------------------------

bool wxMenuKbdRedirector::ProcessEvent(wxEvent& event)
{
	if ( event.GetEventType() == wxEVT_KEY_DOWN ||
		event.GetEventType() == wxEVT_CHAR || event.GetEventType() == wxEVT_CHAR_HOOK) 
	{
#ifdef DEVELOPMENT
		static std::map<int, wxString> keyMap;
		static bool first = true;
		if( first )
		{
			keyMap[wxEVT_KEY_DOWN] = wxT("wxEVT_KEY_DOWN");
			keyMap[wxEVT_CHAR] = wxT("wxEVT_CHAR");
			keyMap[wxEVT_CHAR_HOOK] = wxT("wxEVT_CHAR_HOOK");
		}
		FNB_LOG_MSG(wxT("Received keyboard event type: ") << keyMap[event.GetEventType()] );
#endif
		return m_menu->OnChar(((wxKeyEvent &)event).GetKeyCode());
	}
	else
	{
		return m_oldHandler->ProcessEvent(event);
	}
}
//---------------------------------------------------
// wxFoucsHandler
//---------------------------------------------------
BEGIN_EVENT_TABLE(wxFocusHandler, wxEvtHandler)
EVT_KEY_DOWN(wxFocusHandler::OnKeyDown)
EVT_KILL_FOCUS(wxFocusHandler::OnKillFocus)
END_EVENT_TABLE()

void wxFocusHandler::OnKeyDown(wxKeyEvent& event)
{
	// Let parent process it
	FNB_LOG_MSG(wxT("Received event of type: ") << event.GetEventType() );
	m_menu->OnKeyDown(event);
}

void wxFocusHandler::OnKillFocus(wxFocusEvent &event)
{
	FNB_LOG_MSG(wxT("Received kill focus event"));
	wxPostEvent(m_menu, event);
}

//--------------------------------------------------------
// wxFlatMenuItemGroup
//--------------------------------------------------------
wxFlatMenuItemGroup::wxFlatMenuItemGroup()
{
}

wxFlatMenuItemGroup::~wxFlatMenuItemGroup()
{
}

wxFlatMenuItem *wxFlatMenuItemGroup::GetSelectedItem() const
{
	for(size_t i=0; i<m_items.size(); i++) {
		if(m_items[i]->IsChecked())
			return m_items[i];
	}
	return NULL;
}

void wxFlatMenuItemGroup::Add(wxFlatMenuItem *item)
{
	if(item->IsChecked())
	{
		// uncheck all other items
		for(size_t i=0; i<m_items.size(); i++){
			m_items[i]->m_bIsChecked = false;
		}
	}
	m_items.push_back(item);
}

bool wxFlatMenuItemGroup::Exist(wxFlatMenuItem* item) const
{
	// uncheck all other items
	for(size_t i=0; i<m_items.size(); i++){
		if(m_items[i] == item)
			return true;
	}
	return false;
}

void wxFlatMenuItemGroup::SetSelection(wxFlatMenuItem* item)
{
	// make sure this item exist in our group
	if( !Exist(item) )
		return;

	// uncheck all other items
	for(size_t i=0; i<m_items.size(); i++){
		m_items[i]->m_bIsChecked = false;
	}
	item->m_bIsChecked = true;
}

void wxFlatMenuItemGroup::Remove(wxFlatMenuItem *item)
{
	for(size_t i=0; i<m_items.size(); i++) {
		if (m_items[i] == item) {
			m_items.erase(m_items.begin() + i);
			if(item->IsChecked()) {
				if(!m_items.empty()){
					//if the removed item was the selected one,
					// select the first one in the group
					m_items[0]->m_bIsChecked = true;
				}
			}
		}
	}
}
