///////////////////////////////////////////////////////////////////////////////
// Name:		flat_menu_bar.cpp
// Purpose:     Definition of a flat menu bar 
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#include "flat_menu_bar.h"
#include <wx/settings.h>
#include <wx/dcbuffer.h>
#include <wx/accel.h>
#include <wx/image.h>
#include "customize_dlg.h"

#if wxUSE_AUI
//#include <wx/settings.h>
#endif

#define DROP_DOWN_ARROW_WIDTH	15
#define SPACER					5
#define SEPARATOR_WIDTH			12

#ifdef DEVELOPMENT
#include <wx/log.h>
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; wxLogMessage( logmsg ); }
#else
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; }
#endif

void ConvertToMonochrome(const wxBitmap& bmp, wxBitmap &shadow)
{
	wxMemoryDC mem_dc;
	shadow = wxBitmap(bmp.GetWidth(), bmp.GetHeight());
	mem_dc.SelectObject(shadow);
	mem_dc.DrawBitmap(bmp, 0, 0, true);
	mem_dc.SelectObject(wxNullBitmap);
	wxImage img = shadow.ConvertToImage();
	img = img.ConvertToMono(0, 0, 0);
	
	// we now have black where the original bmp was drawn,
	// white elsewhere
	shadow = wxBitmap(img);
	shadow.SetMask(new wxMask(shadow, *wxBLACK));

	// Convert the black to grey
	wxBitmap tmp(bmp.GetWidth(), bmp.GetHeight());
	wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
	mem_dc.SelectObject(tmp);
	mem_dc.SetPen(wxPen(col));
	mem_dc.SetBrush(wxBrush(col));
	mem_dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());
	mem_dc.DrawBitmap(shadow, 0, 0, true);	// now contains a bitmap with grey where the image was, white elsewhere
	mem_dc.SelectObject(wxNullBitmap);
	shadow = tmp;
	shadow.SetMask(new wxMask(shadow, *wxWHITE)); 
}

wxFlatMenuBar::wxFlatMenuBar(wxWindow *parent, wxWindowID id, bool showToolbar, ToolBarIconSize iconSize)
: m_curretHiliteItem(-1)
, m_dropDownButtonState(ControlNormal)
, m_moreMenu(NULL)
, m_dlg(NULL)
, m_showToolbar(showToolbar)
, m_tbIconSize(iconSize)
, m_interval(20)	//20 milliseconds
, m_tbMenu(NULL)
{
	int dummy(-1);
	wxMemoryDcEx mem_dc;

	mem_dc.GetTextExtent( wxT("Tp"), &dummy, &m_barHeight );
	m_barHeight += 4*SPACER;

	if( m_showToolbar ) {
		// add the toolbar height to the menubar height
		m_barHeight += m_tbIconSize + SPACER ;
	}

	wxSize sz(-1, m_barHeight);
	wxPanel::Create(parent, id, wxDefaultPosition, sz);
	ConnectEvents();

	// start the stop watch
	m_watch.Start();
}

bool wxFlatMenuBar::Append(wxFlatMenu *menu, const wxString &title)
{
	menu->m_menuBarFullTitle = title;

	wxString label;

	wxArtManagerST::Get()->GetAccelIndex(title, label);
	menu->m_menuBarLabelOnly = label;
	return Insert(m_items.size(), menu, title);
}

void wxFlatMenuBar::ConnectEvents()
{
	Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(wxFlatMenuBar::OnEraseBg), NULL, this);
	Connect(wxID_ANY, wxEVT_PAINT, wxPaintEventHandler(wxFlatMenuBar::OnPaint), NULL, this);
	Connect(wxID_ANY, wxEVT_SIZE, wxSizeEventHandler(wxFlatMenuBar::OnSize), NULL, this);
	Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(wxFlatMenuBar::OnMouseMove), NULL, this);
	Connect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(wxFlatMenuBar::OnLeftDown), NULL, this);
	Connect(wxID_ANY, wxEVT_LEFT_DCLICK, wxMouseEventHandler(wxFlatMenuBar::OnLeftDown), NULL, this);
	Connect(wxID_ANY, wxEVT_LEFT_UP, wxMouseEventHandler(wxFlatMenuBar::OnLeftUp), NULL, this);
	Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(wxFlatMenuBar::OnIdle), NULL, this);
	Connect(wxID_ANY, wxEVT_FLAT_MENU_DISMISSED, wxCommandEventHandler(wxFlatMenuBar::OnMenuDismissed), NULL, this);

#ifdef __WXGTK__
	Connect(wxID_ANY, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(wxFlatMenuBar::OnLeaveWindow), NULL, this);
#endif
}

void wxFlatMenuBar::OnIdle(wxIdleEvent &e)
{
	wxUnusedVar(e);
	bool refresh = false;
	if(m_watch.Time() > m_interval)
	{
		// it is time to process UpdateUIEvents
		for(size_t i=0; i<m_tbButtons.size(); i++)
		{
			wxUpdateUIEvent event(m_tbButtons[i]->m_tbItem->GetId());
			event.Enable(m_tbButtons[i]->m_tbItem->IsEnabled());
			event.SetText(m_tbButtons[i]->m_tbItem->GetLabel());
			event.SetEventObject(this);

			GetEventHandler()->ProcessEvent(event);

			if(m_tbButtons[i]->m_tbItem->GetLabel() != event.GetText() || m_tbButtons[i]->m_tbItem->IsEnabled() != event.GetEnabled())
				refresh = true;

			m_tbButtons[i]->m_tbItem->SetLabel( event.GetText() );
			m_tbButtons[i]->m_tbItem->Enable( event.GetEnabled() );
		}
		m_watch.Start(); // Reset the timer
	}

	// we need to update the menu bar
	if( refresh )
		Refresh();
}

void wxFlatMenuBar::UpdateItem(wxFlatMenuItem *item)
{
	if( !m_showToolbar )
		return;

	// search for a tool bar with id
	bool refresh = false;
	size_t i=0;
	for(; i<m_tbButtons.size(); i++)
	{
		
		if(m_tbButtons[i]->m_tbItem->GetId() == item->GetId())
		{
			if(m_tbButtons[i]->m_tbItem->IsEnabled() != item->IsEnabled())
				refresh = true;
			m_tbButtons[i]->m_tbItem->Enable(item->IsEnabled());
			break;
		}
	}
	if( refresh )
	{
		FNB_LOG_MSG(wxT("Refreshing menu bar item!. Item is disabled=") << (int)m_tbButtons[i]->m_tbItem->IsEnabled())
		Refresh();
	}
}

void wxFlatMenuBar::OnPaint(wxPaintEvent & WXUNUSED( event))
{
// on GTK, dont use the bitmap for drawing, 
// draw directly on the DC
#ifdef __WXGTK__
	ClearBitmaps(0);
#endif 
	wxBufferedPaintDC dc(this);
	wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	dc.SetFont( fnt );

	wxRect clientRect = GetClientRect();
	wxArtManagerST::Get()->DrawMenuBarBg(dc, clientRect);
	
	// Get a backgroud image of the more menu button
	wxRect moreMenubtnBgBmpRect(GetMoreMenuButtonRect());

	m_moreMenuBgBmp = wxBitmap(moreMenubtnBgBmpRect.width, moreMenubtnBgBmpRect.height);
	wxMemoryDC memDc;
	memDc.SelectObject(m_moreMenuBgBmp);
	memDc.Blit(0, 0, m_moreMenuBgBmp.GetWidth(), m_moreMenuBgBmp.GetHeight(), &dc, moreMenubtnBgBmpRect.x, moreMenubtnBgBmpRect.y);
	memDc.SelectObject(wxNullBitmap);

	static int barHeight( m_barHeight );
	static int maxtextHeight(-1);
	static int dummy(-1);
	static int padding(-1);

	if( padding == -1 )
		dc.GetTextExtent(wxT("W"), &padding, &dummy);

	int posy;
	posy = 2 * SPACER;
	size_t i=0;

	int posx = SPACER;
	
	// ---------------------------------------------------------------------------
	// Draw as much items as we can if the screen is not wide enough, add all
	// missing items to a drop down menu
	// ---------------------------------------------------------------------------

	wxRect menuBarRect( GetClientRect() );

	// Draw the drop down arrow button
	DrawMoreButton(dc, static_cast<int>(i), m_dropDownButtonState);

	// Set the button rect
	m_dropDownButtonArea = moreMenubtnBgBmpRect;

	// mark all items as non-visibles at first
	for(i=0; i<m_items.size(); i++)
	{
		wxRect itemRect;
		m_items[i]->SetRect(itemRect);
	}

	for(i=0; i<m_items.size(); i++)
	{
		int textWidth, textHeight;

		// Handle accelerator ('&')
		wxString title = m_items[i]->GetTitle();
		wxString fixedText = title;;
		wxString labelOnly;
		
		// Get the label only from the menu title
		int location = wxArtManagerST::Get()->GetAccelIndex(fixedText, labelOnly);
		// Get the menu item length, add some padding to it
		dc.GetTextExtent(fixedText, &textWidth, &textHeight);
		wxRect rect(posx, posy, textWidth + SPACER + padding, textHeight);
		
		// Can we draw more??
		// the +DROP_DOWN_ARROW_WIDTH  is the width of the drop down arrow
		if( posx + rect.width + DROP_DOWN_ARROW_WIDTH >= menuBarRect.GetWidth() )
			break;

		// Keep the item rectangle, will be used later in functions such
		// as 'OnLeftDown', 'OnMouseMove'
		rect.Inflate(0, SPACER);
		m_items[i]->SetRect( rect );
		rect.Deflate(0, SPACER);
		
		if( m_items[i]->GetState() == ControlFocus ) {
			wxArtManagerST::Get()->SetMS2007ButtonSunken(true);
			wxArtManagerST::Get()->DrawButton(dc, m_items[i]->GetRect(), wxArtManagerST::Get()->GetMenuTheme(), ControlFocus, false);
		}
				
		int ww, hh;
		dc.GetTextExtent(labelOnly, &ww, &hh);
		int textOffset = (rect.width - ww) / 2;

		if( m_items[i]->GetTextBitmap().Ok() )
		{	
			dc.DrawBitmap(m_items[i]->GetTextBitmap(), rect.x, rect.y, true);
		}
		else
		{
			// Draw the text on a bitmap using memory dc, 
			// so on following calls we will use this bitmap instead
			// of calculating everything from scratch
			wxBitmap bmp(rect.width, rect.height);
			wxMemoryDcEx memDc;
			memDc.SelectObject( bmp );

			// Fill the bitmap with the maksing colour
			memDc.SetPen( wxPen(wxColour(0, 128, 128)) );
			memDc.SetBrush( wxBrush(wxColour(0, 128, 128)) );
			memDc.DrawRectangle( 0, 0, rect.width, rect.height );
			memDc.SetFont( fnt );

			if((location == wxNOT_FOUND) || (location >= (int)fixedText.Length()))
			{
				// draw the text
				memDc.DrawText(title, textOffset, 0);
				dc.DrawText(title, rect.x + textOffset, rect.y);
			}
			else
			{	
				// underline the first '&'
				wxString before = labelOnly.Mid(0, (size_t)location);
				wxString underlineLetter = labelOnly[location]; 
				wxString after = labelOnly.Mid((size_t)location+1); 

				// before
				memDc.DrawText(before, textOffset, 0);
				dc.DrawText(before, rect.x + textOffset, rect.y);

				// underlineLetter
#ifndef __WXGTK__
				int w1, h;
				dc.GetTextExtent(before, &w1, &h);
				fnt.SetUnderlined(true);
				dc.SetFont(fnt);
				memDc.SetFont(fnt);

				memDc.DrawText(underlineLetter, textOffset + w1, 0);
				dc.DrawText(underlineLetter, rect.x + w1 + textOffset, rect.y);
#else
				int w1, h;
				dc.GetTextExtent(before, &w1, &h);
				memDc.DrawText(underlineLetter, textOffset + w1, 0);
				dc.DrawText(underlineLetter, rect.x + w1 + textOffset, rect.y);

				// Draw the underline ourselves since using the Underline in GTK, 
				// causes the line to be too close to the letter
				int uderlineLetterW, uderlineLetterH;
				dc.GetTextExtent(underlineLetter, &uderlineLetterW, &uderlineLetterH);

				{
					wxDCSaver dcsaver( &dc );
					dc.SetPen( wxPen(dc.GetTextForeground()) );
					dc.DrawLine(rect.x + w1 + textOffset, rect.y + uderlineLetterH - 2, rect.x + w1 + textOffset + uderlineLetterW, rect.y + uderlineLetterH - 2);
				}

#endif 
				// after
				int w2;
				dc.GetTextExtent(underlineLetter, &w2, &h);
				fnt.SetUnderlined(false);
				dc.SetFont(fnt);
				memDc.SetFont(fnt);

				memDc.DrawText(after,  w1 + w2 + textOffset, 0);
				dc.DrawText(after, rect.x + w1 + w2 + textOffset, rect.y);

			}
			memDc.SelectObject( wxNullBitmap );

			// Set masking colour to the bitmap
			bmp.SetMask(new wxMask(bmp, wxColour(0, 128, 128)));

			m_items[i]->SetTextBitmap( bmp );
		}
		posx += rect.width;
	}

	if( m_showToolbar )
	{
		if(m_tbButtons.empty())
			return;

		int rectHeight;
		int rectWidth;
		int rectX;
		int rectY;

		if(m_items.empty())
		{
			rectHeight = clientRect.height - posy - 2*SPACER;
			rectWidth  = clientRect.width - moreMenubtnBgBmpRect.width - 3*SPACER;
			rectX      = SPACER;
			rectY	   = posy;
		}
		else
		{
			rectHeight = clientRect.height - 2*SPACER - m_items[0]->GetRect().height;
			rectWidth  = clientRect.width - moreMenubtnBgBmpRect.width - 3*SPACER;
			rectX      = SPACER;
			rectY	   = m_items[0]->GetRect().y + m_items[0]->GetRect().height;
		}

		wxRect rr(rectX, rectY, rectWidth, rectHeight);
		wxArtManagerST::Get()->DrawToolBarBg(dc, rr);
		DrawToolbar(dc, rr);
	}
}

void wxFlatMenuBar::DrawToolbar(wxDC &dc, const wxRect &rect)
{
	int width = m_tbIconSize + SPACER;
	int height = m_tbIconSize + SPACER;
	int xx = rect.x;
	int yy = rect.y +(rect.height - height)/2;
	size_t i=0;

	// by default set all toolbar items as invisible
	for(i=0; i<m_tbButtons.size(); i++)
		m_tbButtons[i]->m_visible = false;

	// Get all the toolbar items
	for(i=0; i<m_tbButtons.size(); i++)
	{
		// the button width is depends on its type
		if( m_tbButtons[i]->m_tbItem->IsSeparator() )
			width = SEPARATOR_WIDTH;
		else
			width = m_tbIconSize + SPACER; // normal bitmap's width
		
		// can we keep drawing?
		if( xx + width >= rect.width )
			break;
		
		// mark this item as visible
		m_tbButtons[i]->m_visible = true;

		// Draw the toolbar image
		wxBitmap bmp;

		//------------------------------------------
		// special handling for separator
		//------------------------------------------
		if( m_tbButtons[i]->m_tbItem->IsSeparator())
		{
			// Place a separator bitmap
			bmp = wxBitmap(12, rect.height - 2);
			wxMemoryDcEx mem_dc;
			mem_dc.SelectObject(bmp);
			mem_dc.SetPen( *wxBLACK_PEN);
			mem_dc.SetBrush(*wxBLACK_BRUSH);

			mem_dc.DrawRectangle(0, 0, bmp.GetWidth(), bmp.GetHeight());

			wxColor col = wxArtManagerST::Get()->GetMenuBarFaceColour();
			wxColor col1 = wxArtManagerST::Get()->LightColour(col, 40);
			wxColor col2 = wxArtManagerST::Get()->LightColour(col, 70);

			mem_dc.SetPen(wxPen(col2));
			mem_dc.DrawLine(5, 0, 5, bmp.GetHeight());

			mem_dc.SetPen(wxPen(col1));
			mem_dc.DrawLine(6, 0, 6, bmp.GetHeight());

			mem_dc.SelectObject(wxNullBitmap);
			bmp.SetMask(new wxMask(bmp, *wxBLACK));

			// draw the separator
			wxRect buttonRect(xx, rect.y + 1, bmp.GetWidth(), bmp.GetHeight());
			dc.DrawBitmap(bmp, xx, rect.y + 1, true);
			xx += buttonRect.width;
			m_tbButtons[i]->m_rect = buttonRect;
			
			continue;
		}
		else
		{
			m_tbButtons[i]->m_tbItem->IsEnabled() ? bmp = m_tbButtons[i]->m_tbItem->GetBitmap() : bmp = m_tbButtons[i]->m_tbItem->GetDisabledBitmap();
		}

		if(bmp.Ok())
		{
			int x = xx;
			int y = yy + (height - bmp.GetHeight())/2 - 1;
			
			wxRect buttonRect(x, y, width, height);
			
			if(i<m_tbButtons.size() && i>=0)
			{
				if(m_tbButtons[i]->m_state == ControlFocus){
					wxArtManagerST::Get()->DrawButton(dc, buttonRect, wxArtManagerST::Get()->GetMenuTheme(), ControlFocus, false);
					m_tbButtons[i]->m_state = ControlFocus;
				} else {
					m_tbButtons[i]->m_state = ControlNormal;
				}
			}


			int imgx = buttonRect.x + (buttonRect.width - bmp.GetWidth())/2;
			int imgy = buttonRect.y + (buttonRect.height - bmp.GetHeight())/2;

//			if(m_tbButtons[i]->m_state == ControlFocus)
//			{
				// incase we the button is in focus, place it 
				// once pixle up and left
				// place a dark image under the original image to provide it
				// with some shadow
//				wxBitmap shadow;
//				ConvertToMonochrome(bmp, shadow);
//				dc.DrawBitmap(bmp, imgx, imgy, true);

//				imgx -= 1;
//				imgy -= 1;
//			}

			dc.DrawBitmap(bmp, imgx, imgy, true);
			xx += buttonRect.width;
			m_tbButtons[i]->m_rect = buttonRect;
		}
	}
}

wxRect wxFlatMenuBar::GetMoreMenuButtonRect()
{
	wxRect clientRect(GetClientRect());
	wxRect rect(clientRect);

	rect.SetWidth( DROP_DOWN_ARROW_WIDTH  );
	rect.SetX( clientRect.GetWidth() + rect.GetX() - DROP_DOWN_ARROW_WIDTH - 3);
	rect.SetY( 2 );
	rect.SetHeight( rect.GetHeight() - SPACER );
	return rect;
}

// Draw a drop down menu at the right position of the menu bar
void wxFlatMenuBar::DrawMoreButton(wxDC &dc, int from, ControlState state)
{
	wxUnusedVar(from);
	wxRect rect = GetMoreMenuButtonRect();

	// Draw the bitmap
	if( state != ControlNormal ) {
		// Draw background according to state
		wxArtManagerST::Get()->SetMS2007ButtonSunken(true);
		wxArtManagerST::Get()->DrawButton(dc, rect, wxArtManagerST::Get()->GetMenuTheme(), state, false);
	} else {
		// Delete current image
		if( m_moreMenuBgBmp.Ok())
			dc.DrawBitmap(m_moreMenuBgBmp, rect.x, rect.y, true);
	}

	// Draw the drop down arrow
	static wxBitmapPtr dropArrowBmp( NULL );

	// Create the bitmap only once
	if( !dropArrowBmp.Ok() )
	{
		dropArrowBmp = wxArtManagerST::Get()->GetStokBitmap(wxT("arrow_down"));
	}

	// Calc the image coordinates
	wxCoord xx, yy;

	xx = rect.x + (DROP_DOWN_ARROW_WIDTH - dropArrowBmp->GetWidth())/2;
	yy = rect.y + (rect.height - dropArrowBmp->GetHeight())/2;
	
	dc.DrawBitmap(*dropArrowBmp, xx, yy + SPACER, true);
	m_dropDownButtonState = state;
}

int wxFlatMenuBar::HitTest(const wxPoint &pt, size_t &idx)
{
	size_t i = 0;
	if( m_dropDownButtonArea.Contains(pt) ) 
	{
		return DropDownArrowButton;
	}

	for( ; i<m_items.size(); i++)
	{

		if( m_items[i]->GetRect().Contains( pt ) )
		{
			idx = i;
			return MenuItem;
		}
	}

	// check for tool bar items
	if( m_showToolbar )
	{
		for(i=0; i<m_tbButtons.size(); i++)
		{
			// locate the corresponded menu item
			bool enabled  = m_tbButtons[i]->m_tbItem->IsEnabled();
			bool separtor = m_tbButtons[i]->m_tbItem->IsSeparator();
			bool visible  = m_tbButtons[i]->m_visible;
			if(m_tbButtons[i]->m_rect.Contains(pt) && enabled && !separtor && visible)
			{
				idx = i;
				return ToolbarItem;
			}
		}
	}
	return NoWhere;
}

wxFlatMenuItem *wxFlatMenuBar::FindMenuItem(int id)
{
	for(size_t t=0; t<m_items.size(); t++) {
		wxMenuEntryInfoPtr entry = m_items[t];
		wxFlatMenuItem *mi = entry->GetMenu()->FindItem(id);
		if( mi ){
			return mi;
		}
	}
	return NULL;
}


void wxFlatMenuBar::OnSize(wxSizeEvent &event)
{
	wxUnusedVar(event);
	ClearBitmaps(0);
	Refresh();
}

void wxFlatMenuBar::OnEraseBg(wxEraseEvent & WXUNUSED(event))
{
}

void wxFlatMenuBar::ProcessMouseMoveFromMenu(wxPoint &pt)
{
	size_t idx;
	int where = HitTest(pt, idx);
	switch( where )
	{
	case MenuItem:
		ActivateMenu( m_items[idx] );
		break;
	default:
		break;
	}
}

void wxFlatMenuBar::DoMouseMove(wxPoint &pt, bool leftIsDown)
{
	size_t idx;

	// Reset items state
	for(size_t i=0; i<m_items.size(); i++)
		m_items[i]->SetState( ControlNormal );

	int where = HitTest(pt, idx);
	switch( where )
	{
	case DropDownArrowButton:
		{
			if( m_dropDownButtonState != ControlFocus && !leftIsDown)
			{
				wxClientDC dc(this);
				DrawMoreButton(dc, -1, ControlFocus);
			}
		}
		break;
	case MenuItem:
		{
			m_dropDownButtonState = ControlNormal;

			// On Item
			m_items[idx]->SetState( ControlFocus );

			// If this item is already selected, dont draw it again
			if( m_curretHiliteItem == static_cast<int>( idx ) )
				break;

			m_curretHiliteItem = static_cast<int>( idx );
			if(m_showToolbar)
			{
				// mark all toolbar items as non-hilited
				for(size_t i=0; i<m_tbButtons.size(); i++)
					m_tbButtons[i]->m_state = ControlNormal;
			}

			Refresh();
			break;
		}
		break;
	case ToolbarItem:
		{
			if(m_showToolbar)
			{
				if(idx < m_tbButtons.size() && idx >= 0)
				{
					if(m_tbButtons[idx]->m_state == ControlFocus)
						break;

					// we need to refresh the toolbar
					int active = GetActiveToolbarItem();
					if(active != wxNOT_FOUND)
						m_tbButtons[active]->m_state = ControlNormal;
					
					// mark all toolbar items as non-hilited
					for(size_t i=0; i<m_items.size(); i++)
						m_items[i]->SetState(ControlNormal);

					m_tbButtons[idx]->m_state = ControlFocus;
					Refresh();
				}
			}
			break;
		}
	case NoWhere:
		{
			bool refresh = false;
			if( m_dropDownButtonState != ControlNormal ) {
				refresh = true;
				m_dropDownButtonState = ControlNormal;
			}
			
			if( m_showToolbar )
			{
				int tbActiveItem = GetActiveToolbarItem();
				if(tbActiveItem != wxNOT_FOUND)
				{
					m_tbButtons[tbActiveItem]->m_state = ControlNormal;
					refresh = true;
				}
			}

			if( m_curretHiliteItem != -1 )
			{
				m_items[m_curretHiliteItem]->SetState( ControlNormal );
				m_curretHiliteItem = -1;
				Refresh();
			}
			
			if (refresh) 
			{
				Refresh();
			}
		}
		break;
	default:
		break;
	}
}

void wxFlatMenuBar::OnMouseMove(wxMouseEvent &event)
{
	wxPoint pt = event.GetPosition();
	DoMouseMove(pt, event.LeftIsDown());
}

void wxFlatMenuBar::ResetToolbarItems()
{
	for(size_t i=0; i<m_tbButtons.size(); i++)
	{
		m_tbButtons[i]->m_state = ControlNormal;
	}
}

int wxFlatMenuBar::GetActiveToolbarItem()
{
	for(size_t i=0; i<m_tbButtons.size(); i++)
	{
		if(m_tbButtons[i]->m_state == ControlFocus || m_tbButtons[i]->m_state == ControlPressed)
			return (int)i;
	}
	return wxNOT_FOUND;
}

void wxFlatMenuBar::OnLeaveWindow(wxMouseEvent &event)
{
	wxUnusedVar(event);
	FNB_LOG_MSG(wxT("OnLeaveWindow: Default"));
	// The mouse has really left the client area
	m_curretHiliteItem = -1;
	size_t i=0;
	for(i=0; i<m_items.size(); i++)
		m_items[i]->SetState( ControlNormal );
	for(i=0; i<m_tbButtons.size(); i++)
		m_tbButtons[i]->m_state = ControlNormal;
	m_dropDownButtonState = ControlNormal;
	Refresh();
}

void wxFlatMenuBar::OnMenuDismissed(wxCommandEvent &e)
{
	wxUnusedVar(e);
	wxPoint pt = ::wxGetMousePosition();
	pt = ScreenToClient(pt);

	size_t idx;
	int where = HitTest(pt, idx);
	switch(where)
	{
	case MenuItem:
	case DropDownArrowButton:
		FNB_LOG_MSG(wxT("Child Menu dismissed: DropDownArrowButton OR MenuItem"));
		break;
	default:
		FNB_LOG_MSG(wxT("Child Menu dismissed: elsewhere"));
		{
			m_dropDownButtonState = ControlNormal;
			m_curretHiliteItem = -1;
			for(size_t i=0; i<m_items.size(); i++)
				m_items[i]->SetState(ControlNormal);
			Refresh();
		}
		break;
	}
}

void wxFlatMenuBar::OnLeftDown(wxMouseEvent &event)
{
	wxPoint pt = event.GetPosition();
	size_t idx;
	int where = HitTest(pt, idx);
	switch( where )
	{
	case DropDownArrowButton:
		{
			wxClientDC dc(this);
			DrawMoreButton(dc, -1, ControlPressed);
			PopupMoreMenu();
		}
		break;
	case MenuItem:
		{
			// Postion the menu, the GetPosition() return the coords
			// of the button relative to its parent, we need to translate
			// them into the screen coords
			ActivateMenu( m_items[idx] );
		}
		break;
	case ToolbarItem:
		{
			// Over a toolbar item
			wxClientDC dc(this);
			DrawToolbarItem(dc, (int)idx, ControlPressed);

			// TODO:: Do the action specified in this button
			DoToolbarAction(idx);
			break;
		}
	default:
	case NoWhere:
		break;
	}
}

void wxFlatMenuBar::OnLeftUp(wxMouseEvent &event)
{
	wxPoint pt = event.GetPosition();
	size_t idx;
	int where = HitTest(pt, idx);
	switch( where )
	{
	case ToolbarItem:
		{
			// Over a toolbar item
			wxClientDC dc(this);
			DrawToolbarItem(dc, (int)idx, ControlFocus);
			break;
		}
	default:
		break;
	}
}

void wxFlatMenuBar::DrawToolbarItem(wxDC &dc, int idx, ControlState state)
{
	if(idx >= (int)m_tbButtons.size() || idx<0)
		return;

	wxRect rect = m_tbButtons[idx]->m_rect;
	wxArtManagerST::Get()->DrawButton(dc, rect, wxArtManagerST::Get()->GetMenuTheme(), state, false);
	
	// draw the bitmap over the highlight 
	wxRect buttonRect(rect);
	int x = rect.x + (buttonRect.width - m_tbButtons[idx]->m_tbItem->GetBitmap().GetWidth())/2;
	int y = rect.y + (buttonRect.height - m_tbButtons[idx]->m_tbItem->GetBitmap().GetHeight())/2;

//	if(state == ControlFocus)
//	{
		// place a dark image under the original image to provide it
		// with some shadow
//		wxBitmap shadow;
//		ConvertToMonochrome(m_tbButtons[idx]->m_tbItem->GetBitmap(), shadow);
//		dc.DrawBitmap(shadow, x, y, true);

		// incase we the button is in focus, place it 
		// once pixle up and left
//		x -= 1;
//		y -= 1;
//	}
	dc.DrawBitmap(m_tbButtons[idx]->m_tbItem->GetBitmap(), x, y, true);
}

void wxFlatMenuBar::ActivateMenu(wxMenuEntryInfoPtr menuInfo)
{
	// first make sure all other menus are not popedup
	if( menuInfo->GetMenu()->IsShown() )
		return;

	int idx = wxNOT_FOUND;
	for( size_t i=0; i<m_items.size(); i++ )
	{
		m_items[i]->GetMenu()->Dismiss(false, true);
		if(m_items[i]->GetMenu() == menuInfo->GetMenu())
		{
			idx = (int)i;
		}
	}

	FNB_LOG_MSG(wxT("ActivateMenu: Activating menu: ") << idx);

	// Remove the popup menu as well
	if( m_moreMenu && m_moreMenu->IsShown() )
		m_moreMenu->Dismiss(false, true);

	// make sure that the menu item button is highlited
	if(idx != wxNOT_FOUND)
	{	
		m_dropDownButtonState = ControlNormal;
		m_curretHiliteItem = idx;
		for(size_t i=0; i<m_items.size(); i++)
			m_items[i]->SetState(ControlNormal);

		m_items[idx]->SetState(ControlFocus);
		Refresh();
	}

	wxRect rect(menuInfo->GetRect());
	wxPoint menuPt = ClientToScreen( wxPoint(rect.x, rect.y) );
	
	menuInfo->GetMenu()->SetOwnerHeight(rect.height);
	menuInfo->GetMenu()->Popup(wxPoint(menuPt.x, menuPt.y), this);
}

void wxFlatMenuBar::DoToolbarAction(size_t idx)
{
	// we handle only button clicks
	if( !m_tbButtons[idx]->m_tbItem->IsRegularItem() )
		return;

	// Create the event
	wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, m_tbButtons[idx]->m_tbItem->GetId());
	event.SetEventObject(this);

	// all events are handled by this control and its parents
	GetEventHandler()->ProcessEvent(event);
}

int wxFlatMenuBar::FindMenu(const wxString &title) const
{
	size_t i=0;
	for(; i<m_items.size(); i++)
	{
		wxString lableOnly;
		int accelIdx = wxArtManagerST::Get()->GetAccelIndex(m_items[i]->GetTitle(), lableOnly);
		wxUnusedVar( accelIdx );

		if( lableOnly == title || m_items[i]->GetTitle() == title )
			return static_cast<int>(i);
	}
	return wxNOT_FOUND;
}

wxFlatMenu* wxFlatMenuBar::GetMenu(int menuIdx) const
{
	if( menuIdx >= (int)m_items.size() || menuIdx < 0 )
		return NULL;
	
	return m_items[menuIdx]->GetMenu();
}

bool wxFlatMenuBar::Insert(size_t pos, wxFlatMenu *menu, const wxString &title)
{
	wxASSERT_MSG(menu != NULL, wxT("Cant add NULL menu to menu bar"));
	menu->SetMenuBar( this );
	m_items.insert(m_items.begin()+pos, new wxMenuEntryInfo(title, menu));
	UpdateAcceleratorTable();
	return true;
}

wxFlatMenu* wxFlatMenuBar::Remove(size_t pos)
{
	if(pos >= m_items.size())
		return NULL;

	wxFlatMenu *menu = m_items[pos]->GetMenu();
	m_items.erase(m_items.begin()+pos);

	UpdateAcceleratorTable();

	// Since we use bitmaps to optimize our drawings, we need
	// to reset all bitmaps from pos and until end of vector
	// to force size/position changes to the menu bar
	ClearBitmaps(pos);

	Refresh();

	// remove the connection to this menubar
	menu->SetMenuBar( NULL );
	return menu;
}

void wxFlatMenuBar::UpdateAcceleratorTable()
{
	// first get the number of items we have
	std::vector<wxAcceleratorEntry> updatedTable;
	std::vector<wxAcceleratorEntry> menubarTable;

	size_t i=0;

	for(i=0; i<m_items.size(); i++)
	{
		updatedTable.insert(updatedTable.end(), 
							m_items[i]->GetMenu()->GetAccelArray().begin(), 
							m_items[i]->GetMenu()->GetAccelArray().end());

		// create accelerator for every menu (if it exist)
		wxString title = m_items[i]->GetTitle();
		wxString labelOnly;
		int mnemonic = wxArtManagerST::Get()->GetAccelIndex(title, labelOnly);
		if( mnemonic != wxNOT_FOUND )
		{
			// Get the accelrator character
			wxString accelChar = labelOnly[mnemonic]; 
			wxString accelString( wxT("\tAlt+") );
			accelString << accelChar;
			title << accelString;

			wxAcceleratorEntry* accel = wxAcceleratorEntry::Create(title);
			if( accel )
			{
				// connect an event to this cmd
				GetParent()->Connect(m_items[i]->GetCmdId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFlatMenuBar::OnAccelCmd), NULL, this);
				accel->Set(accel->GetFlags(), accel->GetKeyCode(), m_items[i]->GetCmdId());
				updatedTable.push_back( *accel );
				delete accel;
			}
		}
	}
	wxAcceleratorEntry *entries = new wxAcceleratorEntry[ updatedTable.size() ];
				
	// Add the new menu items
	for(i=0; i<updatedTable.size(); i++ )
		entries[i] = updatedTable[i];

	wxAcceleratorTable table((int)updatedTable.size(), entries);
	delete [] entries;

	GetParent()->SetAcceleratorTable( table );
}

void wxFlatMenuBar::ClearBitmaps(size_t start)
{
	for(; start<m_items.size(); start++)
	{
		wxBitmap bmp;
		m_items[start]->SetTextBitmap( bmp );
	}
}

void wxFlatMenuBar::OnAccelCmd(wxCommandEvent &event)
{
	for(size_t i=0; i<m_items.size(); i++)
	{
		if( m_items[i]->GetCmdId() == event.GetId() )
		{
			FNB_LOG_MSG(wxT("OnAccelCmd: Activating menu =") << (int)i);
			ActivateMenu( m_items[i] );
		}
	}
}

void wxFlatMenuBar::ActivateNextMenu()
{
	size_t last_item = GetLastVisibleMenu();;
	// find the current active menu
	for(size_t i=0; i<=last_item; i++ )
	{
		if( m_items[i]->GetMenu()->IsShown() ) 
		{
			size_t nextMenu = i + 1;
			if( nextMenu >= last_item )
				nextMenu = 0;

			ActivateMenu( m_items[nextMenu] );
			return;
		}
	}
}

size_t wxFlatMenuBar::GetLastVisibleMenu()
{
	size_t last_item(0);
	size_t i(0);

	// find the last visible item
	wxRect rect;
	for(i=0; i<m_items.size(); i++)
	{
		if(m_items[i]->GetRect() == rect)
		{
			break;
		}
		last_item++;
	}
	return last_item;
}

void wxFlatMenuBar::ActivatePreviousMenu()
{
	size_t last_item = GetLastVisibleMenu();;
	size_t i=0;

	// find the current active menu
	for(i=0; i<last_item; i++ )
	{
		if( m_items[i]->GetMenu()->IsShown() ) 
		{
			int pervMenu = (int)i - 1;
			if( pervMenu < 0 )
				pervMenu = (int)last_item - 1;

			if( pervMenu < 0 )
				return;

			ActivateMenu( m_items[pervMenu] );
			return;
		}
	}
}
void wxFlatMenuBar::CreateMoreMenu()
{
	if( !m_moreMenu	) {
		// first time
		m_moreMenu = new wxFlatMenu;
		m_popupDlgCmdId = wxNewId();

		// Connect an event handler for this event
		Connect(m_popupDlgCmdId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(wxFlatMenuBar::OnCustimizeDlg), NULL, this);
	}

	// Remove all items from the popup menu
	m_moreMenu->Clear();
	
	wxFlatMenuItem *item;
	size_t i=0;
	size_t lastVisibleItem = GetLastVisibleMenu();
	for(i=lastVisibleItem; i<m_items.size(); i++ ){
		item = new wxFlatMenuItem(m_moreMenu, wxID_ANY, m_items[i]->GetTitle(), wxEmptyString, wxITEM_NORMAL, m_items[i]->GetMenu());

		// remove the decoration from the added menu
		m_moreMenu->Append( item );
	}

	if( lastVisibleItem < m_items.size() )
		m_moreMenu->AppendSeparator();

	// Add invisible toolbar items
	if( m_showToolbar )
	{
		for(i=0; i<m_tbButtons.size(); i++)
		{
			if(m_tbButtons[i]->m_visible == false)
				break;
		}

		if(i < m_tbButtons.size())
		{
			// We have hidden toolbar items, show them!
			if( !m_tbMenu)
			 m_tbMenu = new wxFlatMenu();
			m_tbMenu->Clear();
			
			for(; i<m_tbButtons.size(); i++)
			{
				if(m_tbButtons[i]->m_tbItem->IsSeparator())
					m_tbMenu->AppendSeparator();
				else
				{
					wxFlatToolbarItem *tbitem = m_tbButtons[i]->m_tbItem;
					item = new wxFlatMenuItem(m_tbMenu, tbitem->GetId(), tbitem->GetLabel(), wxEmptyString, wxITEM_NORMAL, NULL, tbitem->GetBitmap(), tbitem->GetDisabledBitmap());
					item->Enable(tbitem->IsEnabled());
					m_tbMenu->Append(item);
				}
			}
			item = new wxFlatMenuItem(m_moreMenu, wxID_ANY, wxT("ToolBar Items"), wxEmptyString, wxITEM_NORMAL, m_tbMenu);
			m_moreMenu->Append(item);
			m_moreMenu->AppendSeparator();
		}
	}

	item = new wxFlatMenuItem(m_moreMenu, m_popupDlgCmdId, wxT("Customize ..."));
	m_moreMenu->Append(item);
}

void wxFlatMenuBar::PopupMoreMenu()
{
	CreateMoreMenu();

	wxPoint pt = m_dropDownButtonArea.GetTopLeft();
	pt = ClientToScreen( pt );
	pt.y += m_dropDownButtonArea.GetHeight();
	m_moreMenu->Popup(pt, this);
}

void wxFlatMenuBar::OnCustimizeDlg(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if( !m_dlg ) {
		m_dlg = new wxFMCustomizeDlg(this);
	} else {
		// intialize the dialog
		m_dlg->Initialise();
	}

	
	if(m_dlg->ShowModal() == wxID_OK)
	{
		// Handle customize requests here
	}

#ifdef __WXGTK__
	// Reset the more button
	wxClientDC dc(this);
	DrawMoreButton(dc, -1, ControlNormal);
#endif
}

void wxFlatMenuBar::AppendToolbarItem(wxFlatToolbarItem *item)
{
	wxRect rr;
	wxFlatMenuBar::ToolBarItemPtr newItem(new wxFlatMenuBar::ToolBarItem(item, rr, ControlNormal));
	m_tbButtons.push_back(newItem);
}
/*
#if wxUSE_AUI
// Position the control inside AUI frame manager
void wxFlatMenuBar::PositionAUI(wxFrameManager *mgr)
{
	wxPaneInfo pn;
	int xx = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);

	// We add our menu bar as a toolbar, with the following settings
	pn.BestSize(wxSize(xx, this->GetSize().y));
	pn.FloatingSize(wxSize(300, this->GetSize().y));
	pn.Floatable(false);
	pn.Dockable(false);
	pn.CaptionVisible(false);
	pn.PaneBorder(false);
	pn.MinSize(wxSize(xx, this->GetSize().y));
	pn.MaxSize(wxSize(xx, this->GetSize().y));
	pn.Top();
	pn.Name(wxT("flat_menu_bar"));
	pn.Caption(wxT("Menu Bar"));
	mgr->AddPane(this, pn);
}
#endif
*/
