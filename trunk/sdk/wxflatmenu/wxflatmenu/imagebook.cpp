#include "imagebook.h"
#include <wx/tooltip.h>
#include <wx/image.h>
#include "art_manager.h"

wxFlatBookBase::wxFlatBookBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: m_pages( NULL )
, m_bInitializing( true )
{
	style |= wxTAB_TRAVERSAL;
	wxPanel::Create(parent, id, pos, size, style, name);
	m_bInitializing = false;
}

void wxFlatBookBase::SetWindowStyleFlag(long style)
{
	wxWindowBase::SetWindowStyleFlag(style);
	
	// Check that we are not in initialization process
	if(m_bInitializing)
		return;

	if(!m_pages)
		return;

	// Detach the windows attached to the sizer
	if(GetSelection() >= 0)
		m_mainSizer->Detach(m_windows[GetSelection()]);
	m_mainSizer->Detach(m_pages);

	// Create new sizer with the requested orientaion
	wxString className = GetClassInfo()->GetClassName();
	if(className == wxT("wxLabelBook"))
	{
		m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	}
	else
	{
		if(style & wxINB_LEFT || style & wxINB_RIGHT)
			m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
		else
			m_mainSizer = new wxBoxSizer(wxVERTICAL);
	}

	SetSizer(m_mainSizer);
	
	// Add the tab container and the separator
	m_mainSizer->Add(m_pages, 0, wxEXPAND);
	
	if(className == wxT("wxFlatImageBook"))
	{
		if(style & wxINB_LEFT || style & wxINB_RIGHT)
			m_pages->SetSizeHints(wxSize(m_pages->m_nImgSize * 2, -1));
		else
			m_pages->SetSizeHints(wxSize(-1, m_pages->m_nImgSize * 2));
	}

	// Attach the windows back to the sizer to the sizer
	if(GetSelection() >= 0)
		DoSetSelection(m_windows[GetSelection()]);
	
	wxSizeEvent dummy;
	wxPostEvent(this, dummy);
	m_pages->Refresh();
}

wxFlatBookBase::~wxFlatBookBase(void)
{
}

void wxFlatBookBase::AddPage(wxWindow* window, const wxString& caption, bool selected, const int imgId)
{
	if(!window)
		return;

	window->Reparent(this);

	m_windows.push_back(window);
	m_pages->AddPage(caption, selected, imgId);
	if(selected || m_windows.size() == 1)
		DoSetSelection(window);
	else
		window->Hide();
	Refresh();
}

void wxFlatBookBase::DeleteAllPages()
{
	if(m_windows.empty())
		return;

	Freeze();
	for(size_t i=0; i<m_windows.size(); i++)
	{
		m_windows[i]->Destroy();
	}
	m_windows.clear();
	Thaw();

	// remove old selection
	m_pages->ClearAll();
	m_pages->Refresh();
}

void wxFlatBookBase::SetSelection(size_t page)
{
	if(page >= m_windows.size())
		return;

	if((int)page == GetSelection())
		return;

	int oldSelection = GetSelection();

	// Generate an event that indicates that an image is about to be selected
	wxImageNotebookEvent event(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGING, GetId());
	event.SetSelection((int)page);
	event.SetOldSelection(oldSelection);
	event.SetEventObject(this);
	GetEventHandler()->ProcessEvent(event);

	// The event handler allows it?
	if (!event.IsAllowed())
		return;

	DoSetSelection(m_windows[page]);
	// Now we can update the new selection
	m_pages->m_nIndex = (int)page;

	// Refresh calls the OnPaint of this class
	m_pages->Refresh();

	// Generate an event that indicates that an image was selected
	wxImageNotebookEvent eventChanged(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGED, GetId());
	eventChanged.SetEventObject(this);
	eventChanged.SetOldSelection(oldSelection);
	GetEventHandler()->ProcessEvent(eventChanged);
}

void wxFlatBookBase::AssignImageList(wxFlatImageBookImageList* imglist)
{
	m_pages->AssignImageList(imglist);

	// Force change
	SetWindowStyleFlag(GetWindowStyleFlag());
}

int wxFlatBookBase::GetSelection()
{
	if(m_pages)
		return m_pages->m_nIndex;
	else
		return -1;
}

void wxFlatBookBase::DoSetSelection(wxWindow* window)
{
	int curSel = GetSelection();
	long style = GetWindowStyleFlag();
	// Replace the window in the sizer
	Freeze();

	// Check if a new selection was made
	bool bInsertFirst = (style & wxINB_BOTTOM || style & wxINB_RIGHT);
	if(curSel >= 0)
	{
		// Remove the window from the main sizer
		m_mainSizer->Detach(m_windows[curSel]);
		m_windows[curSel]->Hide();
	}

	if(bInsertFirst)
		m_mainSizer->Insert(0, window, 1, wxEXPAND);
	else
		m_mainSizer->Add(window, 1, wxEXPAND);

	window->Show();
	m_mainSizer->Layout();
	Thaw();
}

wxFlatImageBookImageList* wxFlatBookBase::GetImageList()
{
	return m_pages->GetImageList();
}

wxImageContainerBase::wxImageContainerBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: m_nIndex(-1)
, m_nImgSize(16)
, m_ImageList(NULL)
, m_nHoeveredImgIdx( -1 )
, m_bCollapsed( false )
, m_tabAreaSize(-1, -1)
, m_nPinButtonStatus(wxINB_PIN_NONE)
{
	wxWindow::Create(parent, id, pos, size, style | wxNO_BORDER | wxNO_FULL_REPAINT_ON_RESIZE, name);
}

bool wxImageContainerBase::HasFlag(int flag)
{
	long style = GetParent()->GetWindowStyleFlag();
	bool res = style & flag ? true : false;
	return res;
}

void wxImageContainerBase::ClearFlag(int flag)
{
	long style = GetParent()->GetWindowStyleFlag();
	style &= ~( flag );
	wxWindowBase::SetWindowStyleFlag(style);
}

void wxImageContainerBase::AssignImageList(wxFlatImageBookImageList* imglist)
{  
	if(imglist && !imglist->empty())
	{
		m_nImgSize = (*imglist)[0].GetHeight();
	}

	m_ImageList = imglist;
}

wxImageContainerBase::~wxImageContainerBase(void)
{
	if(m_ImageList)
	{
		delete m_ImageList;
		m_ImageList = NULL;
	}
}

void wxImageContainerBase::FixTextSize(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText)
{
	wxArtManagerST::Get()->TruncateText(dc, text, maxWidth, fixedText);
}

void wxImageContainerBase::AddPage(const wxString& caption, bool selected, const int imgIdx)
{
	m_pagesInfoVec.push_back(wxImageInfo(caption, imgIdx));
	if(selected || m_pagesInfoVec.size() == 1)
		m_nIndex = (int)m_pagesInfoVec.size()-1;
	Refresh();
}

void wxImageContainerBase::ClearAll()
{
	m_pagesInfoVec.clear();
	m_nIndex = wxNOT_FOUND;
}

void wxImageContainerBase::OnSize(wxSizeEvent& event)
{
	Refresh(); // Call on paint
	event.Skip();
}

int wxImageContainerBase::HitTest(const wxPoint& pt, int &imgIdx)
{
	int i ;
	long style = GetParent()->GetWindowStyleFlag();
	if(style & wxINB_USE_PIN_BUTTON)
	{
		if(m_pinBtnRect.Contains(pt))
			return wxIMG_OVER_PIN;
	}

	for(i=0; i<(int)m_pagesInfoVec.size(); i++)
	{
		if(m_pagesInfoVec[i].GetPosition() == wxPoint(-1, -1))
			break;
		
		// For Web Hover style, we test the TextRect
		wxRect buttonRect;
		if( !HasFlag( wxINB_WEB_HILITE ) )
			buttonRect = wxRect(m_pagesInfoVec[i].GetPosition(), m_pagesInfoVec[i].GetSize());
		else
			buttonRect = m_pagesInfoVec[i].GetTextRect();

		if(buttonRect.Contains(pt))
		{
			imgIdx = i;
			return wxIMG_OVER_IMG;
		}
	}

	if(PointOnSash(pt))
		return wxIMG_OVER_EW_BORDER;
	else
		return wxIMG_NONE;
}

bool wxImageContainerBase::PointOnSash(const wxPoint& pt)
{
	// Check if we are on a the sash border
	wxRect cltRect = GetClientRect();
	if( HasFlag(wxINB_LEFT) || HasFlag(wxINB_TOP))
	{
		if( pt.x > cltRect.x + cltRect.width - 4)
			return true;
	}
	else
	{
		if( pt.x < 4 )
			return true;
	}
	return false;
}

void wxImageContainerBase::OnMouseLeftDown(wxMouseEvent& event)
{
	int newSelection = -1;
	event.Skip();

	/// Support for collapse/expand
	long style = GetParent()->GetWindowStyleFlag();
	if(style & wxINB_USE_PIN_BUTTON)
	{
		if(m_pinBtnRect.Contains(event.GetPosition()))
		{
			m_nPinButtonStatus = wxINB_PIN_PRESSED;
			wxClientDC dc(this);
			DrawPin(dc, m_pinBtnRect, !m_bCollapsed);
			return;
		}
	}

	// Incase panel is collapsed, there is nothing 
	// to check 
	if(m_bCollapsed)
		return;

	int where = HitTest(event.GetPosition(), newSelection);
	switch( where ) 
	{
	case wxIMG_OVER_IMG:
		m_nHoeveredImgIdx = -1;
		break;
	default:
		break;
	}

	if(newSelection == -1)
	{
		return;
	}

	((wxFlatBookBase*)GetParent())->SetSelection(newSelection);
}

void wxImageContainerBase::OnMouseLeaveWindow(wxMouseEvent& WXUNUSED(event))
{
	bool bRepaint = m_nHoeveredImgIdx != -1;
	m_nHoeveredImgIdx = -1;

	// Make sure the pin button status is NONE
	// incase we were in pin button style
	long style = GetParent()->GetWindowStyleFlag();
	if(style & wxINB_USE_PIN_BUTTON)
	{
		m_nPinButtonStatus = wxINB_PIN_NONE;
		wxClientDC dc(this);
		DrawPin(dc, m_pinBtnRect, !m_bCollapsed);
	}

	/// Restore cursor
	::wxSetCursor(wxCursor( wxCURSOR_ARROW ));
	if(bRepaint) 
		Refresh();
}

void wxImageContainerBase::OnMouseLeftUp(wxMouseEvent& event)
{
	long style = GetParent()->GetWindowStyleFlag();
	if(style & wxINB_USE_PIN_BUTTON)
	{
		bool bIsLabelContainer = !CanDoBottomStyle();
		if(m_pinBtnRect.Contains(event.GetPosition()))
		{
			m_nPinButtonStatus = wxINB_PIN_NONE;
			m_bCollapsed = !m_bCollapsed;

			if(m_bCollapsed)
			{
				// Save the current tab area width
				m_tabAreaSize = GetSize();
				
				if(bIsLabelContainer)
				{
					SetSizeHints(wxSize(20, m_tabAreaSize.y));
				}
				else
				{
					if(style & wxINB_BOTTOM || style & wxINB_TOP)
						SetSizeHints(wxSize(m_tabAreaSize.x, 20));
					else
						SetSizeHints(wxSize(20, m_tabAreaSize.y));
				}
			}
			else
			{
				if(bIsLabelContainer)
				{
					SetSizeHints(wxSize(m_tabAreaSize.x, -1));
				}
				else
				{
					// Restore the tab area size
					if(style & wxINB_BOTTOM || style & wxINB_TOP)
						SetSizeHints(wxSize(-1, m_tabAreaSize.y));
					else
						SetSizeHints(wxSize(m_tabAreaSize.x, -1));
				}
			}
			GetParent()->GetSizer()->Layout();
			return;
		}
	}
}

void wxImageContainerBase::OnMouseMove(wxMouseEvent& event)
{
	long style = GetParent()->GetWindowStyleFlag();
	if(style & wxINB_USE_PIN_BUTTON)
	{
		// Check to see if we are in the pin button rect
		if(!m_pinBtnRect.Contains(event.GetPosition()) && m_nPinButtonStatus == wxINB_PIN_PRESSED)
		{
			m_nPinButtonStatus = wxINB_PIN_NONE;
			wxClientDC dc(this);
			DrawPin(dc, m_pinBtnRect, !m_bCollapsed);
		}
	}

	int imgIdx = -1;
	HitTest(event.GetPosition(), imgIdx);
	m_nHoeveredImgIdx = imgIdx;
	
	if(!m_bCollapsed)
	{
		if(m_nHoeveredImgIdx >= 0 && m_nHoeveredImgIdx < (int)m_pagesInfoVec.size())
		{
			/// Change the cursor to be Hand
			if( HasFlag( wxINB_WEB_HILITE) && m_nHoeveredImgIdx != m_nIndex) 
				::wxSetCursor( wxCURSOR_HAND );
		}
		else
		{
			// Restore the cursor only if we have the Web hover style set,
			// and we are not currently hovering the sash
			if( HasFlag( wxINB_WEB_HILITE) && !PointOnSash(event.GetPosition())) 
				::wxSetCursor( wxCURSOR_ARROW );
		}
	}


	// Dont display hover effect when hoevering the 
	// selected label
	if(m_nHoeveredImgIdx == m_nIndex)
	{
		m_nHoeveredImgIdx = -1;
	}

	Refresh();
}

void wxImageContainerBase::DrawPin(wxDC& dc, wxRect &rect, bool downPin)
{
	// Set the bitmap according to the button status
	wxBitmap pinBmp;

	if(downPin)
		pinBmp = wxBitmap(pin_down_xpm);
	else
		pinBmp = wxBitmap(pin_left_xpm);

	int xx = rect.x + 2;
	switch(m_nPinButtonStatus)
	{
	case wxINB_PIN_HOVER:
	case wxINB_PIN_NONE:
		{
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(*wxBLACK_PEN);
			dc.DrawRectangle(xx, rect.y, 16, 16);

			// Draw upper and left border with grey color
			dc.SetPen(*wxWHITE_PEN);
			dc.DrawLine(xx, rect.y, xx + 16, rect.y);
			dc.DrawLine(xx, rect.y, xx, rect.y + 16);
		}
		break;
	case wxINB_PIN_PRESSED:
		{
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.SetPen(wxPen(wxT("LIGHT GREY")));
			dc.DrawRectangle(xx, rect.y, 16, 16);

			// Draw upper and left border with grey color
			dc.SetPen(wxPen(wxT("BLACK")));
			dc.DrawLine(xx, rect.y, xx + 16, rect.y);
			dc.DrawLine(xx, rect.y, xx, rect.y + 16);
		}
		break;
	default:
		break;
	}

	/// Set the masking
	pinBmp.SetMask(new wxMask(pinBmp, *wxWHITE));

	// Draw the new bitmap
	dc.DrawBitmap(pinBmp, xx, rect.y, true);

	// Save the pin rect
	m_pinBtnRect = rect;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// wxFlatImageBook Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxFlatImageBook, wxFlatBookBase)

IMPLEMENT_DYNAMIC_CLASS(wxImageNotebookEvent, wxNotifyEvent)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxImageContainer, wxImageContainerBase)
EVT_PAINT(wxImageContainer::OnPaint)
EVT_SIZE(wxImageContainerBase::OnSize)
EVT_LEFT_DOWN(wxImageContainerBase::OnMouseLeftDown)
EVT_LEFT_UP(wxImageContainerBase::OnMouseLeftUp)
EVT_ERASE_BACKGROUND(wxImageContainerBase::OnEraseBackground)
EVT_MOTION(wxImageContainerBase::OnMouseMove)
EVT_LEAVE_WINDOW(wxImageContainerBase::OnMouseLeaveWindow)
END_EVENT_TABLE()

void wxImageContainer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	long style = GetParent()->GetWindowStyleFlag();

	wxBrush backBrush = wxBrush(wxColor(wxT("WHITE")));
	wxPen   borderPen = wxPen(style & wxINB_BORDER ? wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW) : *wxTRANSPARENT_PEN);

	wxSize size = GetSize();

	// Background
	dc.SetBrush(backBrush);

	borderPen.SetWidth(1);
	dc.SetPen(borderPen);
	dc.DrawRectangle(0, 0, size.x, size.y);
	bool bUsePin = style & wxINB_USE_PIN_BUTTON ? true : false;

	if(bUsePin)
	{
		// Draw the pin button
		wxRect clientRect = GetClientRect();
		wxRect pinRect = wxRect(clientRect.GetX() + clientRect.GetWidth() - 20, 2, 20, 20);
		DrawPin(dc, pinRect, !m_bCollapsed);

		if(m_bCollapsed)
			return;
	}

	borderPen = wxPen(*wxBLACK);
	borderPen.SetWidth(1);
	dc.SetPen(borderPen);
	dc.DrawLine(0, size.y, size.x, size.y);
	dc.DrawPoint(0, size.y);

	int clientSize = 0;
	bool bUseYcoord = (style & wxINB_RIGHT || style & wxINB_LEFT);

	if(bUseYcoord)
		clientSize = size.GetHeight();
	else
		clientSize = size.GetWidth();

	// We reserver 20 pixels for the 'pin' button
	int pos;
	
	// The drawing of the images start position. This is 
	// depenedent of the style, especially when Pin button
	// style is requested
	if(bUsePin)
		if(style & wxINB_TOP || style & wxINB_BOTTOM)
			pos = style & wxINB_BORDER ? 0 : 1;
		else	
			pos = style & wxINB_BORDER ? 20 : 21;
	else
		pos = style & wxINB_BORDER ? 0 : 1;

	int textHeight, textWidth;
	const int nPadding = 4;	// Pad text with 2 pixels on the left and right
	const int nTextPaddingLeft = 2;

	int i=0;
	for(; i<(int)m_pagesInfoVec.size(); i++)
	{
		// incase the 'fit button' style is applied, we set the rectangle width to the
		// text width plus padding
		// Incase the style IS applied, but the style is either LEFT or RIGHT
		// we ignore it
		wxFont normalFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		dc.SetFont(normalFont);

		dc.GetTextExtent(m_pagesInfoVec[i].GetCaption(), &textWidth, &textHeight);

		// Restore font to be normal
		normalFont.SetWeight(wxFONTWEIGHT_NORMAL);
		dc.SetFont(normalFont);

		// Default values for the surronounding rectangle 
		// around a button
		int rectWidth = m_nImgSize * 2 ; // To avoid the recangle to 'touch' the borders
		int rectHeight = m_nImgSize * 2;

		// Incase the style requires non-fixed button (fit to text)
		// recalc the rectangle width
		if((style & wxINB_FIT_BUTTON) &&								// fit button style is required
		   !((style & wxINB_LEFT) || (style & wxINB_RIGHT)) &&			// buttons orientation is top or bottom
		   !m_pagesInfoVec[i].GetCaption().IsEmpty() &&					// Button has text
		   !(style & wxINB_SHOW_ONLY_IMAGES))							// The style allows printing of text label
		{
			rectWidth = ((textWidth + nPadding * 2) > rectWidth) ? nPadding * 2 + textWidth : rectWidth;

			/// Make the width an even number
			if( rectWidth % 2 != 0)
				rectWidth += 1;
		}

		// Check that we have enough space to draw the button
		// If Pin button is used, consider its space as well (applicable for top/botton style)
		// since in the left/right, its size is already considered in 'pos'
		int pinBtnSize = bUsePin ? 20 : 0;
		if(pos + rectWidth + pinBtnSize > clientSize)
			break;

		// Calculate the button rectangle
		wxRect buttonRect;
		int modRectWidth = (style & wxINB_LEFT || style & wxINB_RIGHT) ? rectWidth - 2 : rectWidth;
		int modRectHeight = (style & wxINB_LEFT || style & wxINB_RIGHT) ? rectHeight : rectHeight - 2;

		if(bUseYcoord)
			buttonRect = wxRect(1, pos, modRectWidth, modRectHeight);
		else
			buttonRect = wxRect(pos , 1, modRectWidth, modRectHeight);

		// Check if we need to draw a rectangle around the button
		if(m_nIndex == i)
		{

			// Set the colors
			wxColor penColor(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
			wxColor brushColor = wxArtManagerST::Get()->LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 75);

			dc.SetPen(wxPen(penColor));
			dc.SetBrush(wxBrush(brushColor));

			// Fix the surrounding of the rect if border is set
			if(style & wxINB_BORDER)
			{
				if(style & wxINB_TOP || style & wxINB_BOTTOM)
					buttonRect = wxRect(buttonRect.x + 1, buttonRect.y, buttonRect.width - 1, buttonRect.height);
				else
					buttonRect = wxRect(buttonRect.x, buttonRect.y + 1, buttonRect.width, buttonRect.height - 1);
			}

			dc.DrawRectangle(buttonRect);
		}

		if(m_nHoeveredImgIdx == i)
		{
			// Set the colors
			wxColor penColor(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
			wxColor brushColor = wxArtManagerST::Get()->LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), 90);

			dc.SetPen(wxPen(penColor));
			dc.SetBrush(wxBrush(brushColor));

			// Fix the surrounding of the rect if border is set
			if(style & wxINB_BORDER)
			{
				if(style & wxINB_TOP || style & wxINB_BOTTOM)
					buttonRect = wxRect(buttonRect.x + 1, buttonRect.y, buttonRect.width - 1, buttonRect.height);
				else
					buttonRect = wxRect(buttonRect.x, buttonRect.y + 1, buttonRect.width, buttonRect.height - 1);
			}

			dc.DrawRectangle(buttonRect);
		}

		wxRect rect;
		if(bUseYcoord)
			rect = wxRect(0, pos, rectWidth, rectWidth);
		else
			rect = wxRect(pos, 0, rectWidth, rectWidth);

		// Incase user set both flags:
		// wxINB_SHOW_ONLY_TEXT and wxINB_SHOW_ONLY_IMAGES
		// We override them to display both
		wxASSERT_MSG(!(((style & wxINB_SHOW_ONLY_TEXT) && (style & wxINB_SHOW_ONLY_IMAGES))), wxT("Can't set both flags wxINB_SHOW_ONLY_TEXT, wxINB_SHOW_ONLY_IMAGES"));
		if(((style & wxINB_SHOW_ONLY_TEXT) && (style & wxINB_SHOW_ONLY_IMAGES)))
		{
			style ^= wxINB_SHOW_ONLY_TEXT;
			style ^= wxINB_SHOW_ONLY_IMAGES;
			wxWindowBase::SetWindowStyleFlag(style);
		}

		// Draw the caption and text
		int imgTopPadding = 10;
		if(!(style & wxINB_SHOW_ONLY_TEXT) && m_pagesInfoVec[i].GetImageIndex() != -1)
		{
			int imgYcoord, imgXcoord;
			if(bUseYcoord)
			{
				imgXcoord = m_nImgSize / 2;
				imgYcoord = (style & wxINB_SHOW_ONLY_IMAGES) ? pos + m_nImgSize / 2 : pos + imgTopPadding;
			}
			else
			{
				imgXcoord = pos + (rectWidth / 2) - (m_nImgSize / 2);
				imgYcoord = (style & wxINB_SHOW_ONLY_IMAGES) ? m_nImgSize / 2 : imgTopPadding;
				
			}

			//Convert to grey 
			//wxBMP_8BPP_GRAY
			//wxImage image = (*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()].ConvertToImage();
			//image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, wxPNG_TYPE_GREY_RED); 
			//image.SaveFile("test.png", wxBITMAP_TYPE_PNG);
			//wxBitmap bmp(image);

			//dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()], imgXcoord, imgYcoord, true);
			dc.DrawBitmap((*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()], imgXcoord, imgYcoord, true);
		}

		// Draw the text
		if(!(style & wxINB_SHOW_ONLY_IMAGES) && !m_pagesInfoVec[i].GetCaption().IsEmpty())
		{
			dc.SetFont(normalFont);
						
			// Check if the text can fit the size of the rectangle,
			// if not truncate it 
			wxString fixedText(m_pagesInfoVec[i].GetCaption());
			if(!(style & wxINB_FIT_BUTTON) || ((style & wxINB_LEFT) || (style & wxINB_RIGHT)))
			{
				FixTextSize(dc, m_pagesInfoVec[i].GetCaption(), m_nImgSize *2 - 4, fixedText);

				// Update the length of the text
				dc.GetTextExtent(fixedText, &textWidth, &textHeight);
			}
			
			int textOffsetX;
			int textOffsetY;
			if(bUseYcoord)
			{
				textOffsetX = ((rectWidth - textWidth) / 2 );
				textOffsetY = !(style & wxINB_SHOW_ONLY_TEXT) ? pos + m_nImgSize  + imgTopPadding + 3 : pos + ((m_nImgSize * 2 - textHeight) / 2 );
			}
			else
			{
				textOffsetX = ((rectWidth - textWidth) / 2 ) + pos + nTextPaddingLeft;
				textOffsetY = !(style & wxINB_SHOW_ONLY_TEXT) ? m_nImgSize + imgTopPadding + 3 : ((m_nImgSize * 2 - textHeight) / 2 );
			}

			dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
			dc.DrawText(fixedText, textOffsetX, textOffsetY);
		}

		// Update the page info
		m_pagesInfoVec[i].SetPosition(buttonRect.GetPosition());
		m_pagesInfoVec[i].SetSize(buttonRect.GetSize());

		pos += rectWidth;
	}

	// Update all buttons that can not fit into the screen as non-visible
	for(; i<(int)m_pagesInfoVec.size(); i++)
		m_pagesInfoVec[i].SetPosition(wxPoint(-1, -1));

	// Draw the pin button
	if(bUsePin)
	{
		wxRect clientRect = GetClientRect();
		wxRect pinRect = wxRect(clientRect.GetX() + clientRect.GetWidth() - 20, 2, 20, 20);
		DrawPin(dc, pinRect, !m_bCollapsed);
	}
}

BEGIN_EVENT_TABLE(wxFlatImageBook, wxFlatBookBase)
END_EVENT_TABLE()

wxFlatImageBook::wxFlatImageBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxFlatBookBase(parent, id, pos, size, style, name) 
{
	m_pages = CreateImageContainer();

	if(style & wxINB_LEFT || style & wxINB_RIGHT)
		m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	else
		m_mainSizer = new wxBoxSizer(wxVERTICAL);

	SetSizer(m_mainSizer);

	// Add the tab container to the sizer
	m_mainSizer->Add(m_pages, 0, wxEXPAND);

	if(style & wxINB_LEFT || style & wxINB_RIGHT)
		m_pages->SetSizeHints(wxSize(m_pages->GetImageSize() * 2, -1));
	else
		m_pages->SetSizeHints(wxSize(-1, m_pages->GetImageSize() * 2));
}
	
wxImageContainerBase* wxFlatImageBook::CreateImageContainer()
{
	return new wxImageContainer(this, wxID_ANY);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// wxFlatImageBook Implementation
//
////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxLabelBook, wxFlatBookBase)

BEGIN_EVENT_TABLE(wxLabelContainer, wxImageContainerBase)
EVT_PAINT(wxLabelContainer::OnPaint)
EVT_SIZE(wxImageContainer::OnSize)
EVT_LEFT_DOWN(wxLabelContainer::OnMouseLeftDown)
EVT_LEFT_UP(wxLabelContainer::OnMouseLeftUp)
EVT_MOTION(wxLabelContainer::OnMouseMove)
EVT_LEAVE_WINDOW(wxLabelContainer::OnMouseLeaveWindow)
EVT_ERASE_BACKGROUND(wxImageContainerBase::OnEraseBackground)
END_EVENT_TABLE()

wxLabelContainer::wxLabelContainer(wxWindow* parent, wxWindowID id , const wxPoint& pos, const wxSize& size, long style)
: wxImageContainerBase(parent, id, pos, size, style, wxT("wxLabelContainer")) 
, m_nTabAreaWidth(100) 
, m_oldCursor(wxNullCursor)
{
}

void wxLabelContainer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
	wxBufferedPaintDC dc(this);
	wxBrush backBrush = wxBrush(m_colorsMap[wxINB_TAB_AREA_BACKGROUND_COLOR]);
	wxPen   borderPen = wxPen( HasFlag(wxINB_BORDER) ? m_colorsMap[wxINB_TABS_BORDER_COLOR] : *wxTRANSPARENT_PEN);
	wxSize size = GetSize();
	
	/// Set the pen & brush
	dc.SetBrush(backBrush);
	dc.SetPen(borderPen);

	if( HasFlag(wxINB_GRADIENT_BACKGROUND) && !m_skin.Ok())
	{
		/// Draw graident in the background area
		wxColor startColor = m_colorsMap[wxINB_TAB_AREA_BACKGROUND_COLOR];
		wxColor endColor   = wxArtManagerST::Get()->LightColour(m_colorsMap[wxINB_TAB_AREA_BACKGROUND_COLOR], 50);
		wxArtManagerST::Get()->PaintStraightGradientBox(dc, wxRect(0, 0, size.x / 2, size.y), startColor, endColor, false);
		wxArtManagerST::Get()->PaintStraightGradientBox(dc, wxRect(size.x / 2, 0, size.x / 2, size.y), endColor, startColor, false);
	}
	else
	{
		/// Draw the border and background
		if( m_skin.Ok() )
		{
			dc.SetBrush( *wxTRANSPARENT_BRUSH );
			DrawBackgroundBitmap(dc);
		}

		dc.DrawRectangle(wxRect(0, 0, size.x, size.y));
	}

	/// Draw border
	if( HasFlag(wxINB_BORDER) && HasFlag(wxINB_GRADIENT_BACKGROUND))
	{
		// Just draw the border with transparent brush
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
		dc.DrawRectangle(wxRect(0, 0, size.x, size.y));
	}

	bool bUsePin = HasFlag(wxINB_USE_PIN_BUTTON) ? true : false;

	if(bUsePin)
	{
		// Draw the pin button
		wxRect clientRect = GetClientRect();
		wxRect pinRect = wxRect(clientRect.GetX() + clientRect.GetWidth() - 20, 2, 20, 20);
		DrawPin(dc, pinRect, !m_bCollapsed);

		if(m_bCollapsed)
			return;
	}
	
	dc.SetPen(*wxBLACK_PEN);
	SetSizeHints(wxSize(m_nTabAreaWidth, -1));

	// We reserve 20 pixels for the pin button
	int posy = 20 ;
	int i=0;

	for(; i<(int)m_pagesInfoVec.size(); i++)
	{
		// Default values for the surronounding rectangle 
		// around a button
		int rectWidth = m_nTabAreaWidth ; 
		int rectHeight = m_nImgSize * 2;

		// Check that we have enough space to draw the button
		if(posy + rectHeight > size.GetHeight())
			break;

		// Calculate the button rectangle
		wxRect buttonRect;
		int posx = 0;

		buttonRect = wxRect(posx, posy, rectWidth, rectHeight);
		wxBitmap bmp = -1 == m_pagesInfoVec[i].GetImageIndex() ? wxNullBitmap : (*m_ImageList)[m_pagesInfoVec[i].GetImageIndex()];

		DrawLabel(	dc, 
					buttonRect, 
					m_pagesInfoVec[i].GetCaption(), 
					bmp, 
					m_pagesInfoVec[i], 
					HasFlag(wxINB_LEFT) || HasFlag(wxINB_TOP), 
					i,
					m_nIndex == i,
					m_nHoeveredImgIdx == i);

		posy += rectHeight;
	}

	// Update all buttons that can not fit into the screen as non-visible
	for(; i<(int)m_pagesInfoVec.size(); i++)
		m_pagesInfoVec[i].SetPosition(wxPoint(-1, -1));

	if(bUsePin)
	{
		wxRect clientRect = GetClientRect();
		wxRect pinRect = wxRect(clientRect.GetX() + clientRect.GetWidth() - 20, 2, 20, 20);
		DrawPin(dc, pinRect, !m_bCollapsed);
	}
}

void wxLabelContainer::DrawBackgroundBitmap(wxDC& dc)
{
	wxRect clientRect( GetClientRect() );
	int width = clientRect.GetWidth();
	int height = clientRect.GetHeight();
	int coveredY = 0, coveredX = 0;
	int xstep = m_skin.GetWidth();
	int ystep = m_skin.GetHeight();
	wxRect bmpRect(0, 0, xstep, ystep);
	if(bmpRect != clientRect)
	{
		wxMemoryDcEx mem_dc;
		wxBitmap bmp( width, height );
		mem_dc.SelectObject( bmp );

		while( coveredY < height )
		{
			while( coveredX < width )
			{
				mem_dc.DrawBitmap( m_skin, coveredX, coveredY, true );
				coveredX += xstep;
			}
			coveredX = 0;
			coveredY += ystep;
		}
		mem_dc.SelectObject(wxNullBitmap);
		//m_skin = bmp;
		dc.DrawBitmap(bmp, 0, 0);
	}
	else
	{
		dc.DrawBitmap(m_skin, 0, 0);
	}
}

void wxLabelContainer::OnMouseLeftUp(wxMouseEvent& event)
{
	if( HasFlag(wxINB_NO_RESIZE) )
	{
		wxImageContainerBase::OnMouseLeftUp(event);
		return;
	}

	if(HasCapture())
		ReleaseMouse();

	// Sash was being dragged?
	if( !m_sashRect.IsEmpty() )
	{
		// Remove sash
		wxArtManagerST::Get()->DrawDragSash(m_sashRect);
		Resize(event);

		m_sashRect = wxRect();
		return;
	}
	m_sashRect = wxRect();

	// Restore cursor
	if( m_oldCursor.Ok() )
	{
		wxSetCursor(m_oldCursor);
		m_oldCursor = wxNullCursor;
	}

	wxImageContainerBase::OnMouseLeftUp(event);
}

void wxLabelContainer::Resize(wxMouseEvent& event)
{
	// Resize our size
	m_tabAreaSize = GetSize();
	int newWidth = m_tabAreaSize.x;
	int x = event.GetX();
	if(HasFlag(wxINB_BOTTOM) || HasFlag(wxINB_RIGHT))
	{
		newWidth -= event.GetX();
	}
	else
	{
		newWidth = x;
	}

	if( newWidth < 100 ) // Dont allow width to be lower than that 
		newWidth = 100;

	SetSizeHints(wxSize(newWidth, m_tabAreaSize.y));

	// Update the tab new area width
	m_nTabAreaWidth = newWidth;
	GetParent()->Freeze();
	GetParent()->GetSizer()->Layout();
	GetParent()->Thaw();
}

void wxLabelContainer::OnMouseMove(wxMouseEvent& event)
{
	if( HasFlag(wxINB_NO_RESIZE) )
	{
		wxImageContainerBase::OnMouseMove(event);
		return;
	}

	// Remove old sash
	if( !m_sashRect.IsEmpty() )
		wxArtManagerST::Get()->DrawDragSash(m_sashRect);

	if( event.LeftIsDown() )
	{
		if( !m_sashRect.IsEmpty() )
		{
			// Progress sash, and redraw it
			wxRect clientRect = GetClientRect();
			wxPoint pt;
			pt = ClientToScreen(wxPoint(event.GetX(), 0));
			m_sashRect = wxRect(pt, wxSize(4, clientRect.height));
			wxArtManagerST::Get()->DrawDragSash(m_sashRect);
		}
		else
		{
			// Sash is not being dragged
			if( m_oldCursor.Ok() )
			{
				wxSetCursor(m_oldCursor);
				m_oldCursor = wxNullCursor;
			}
		}
	}
	else
	{
		if(HasCapture())
			ReleaseMouse();

		if(PointOnSash(event.GetPosition()))
		{
			// Change cursor to EW cursor
			m_oldCursor = GetCursor();
			wxSetCursor(wxCURSOR_SIZEWE);
		}
		else if( m_oldCursor.Ok() )
		{
			wxSetCursor(m_oldCursor);
			m_oldCursor = wxNullCursor;
		}

		m_sashRect = wxRect();
		wxImageContainerBase::OnMouseMove(event);
	}
}

void wxLabelContainer::OnMouseLeftDown(wxMouseEvent& event)
{
	if( HasFlag(wxINB_NO_RESIZE) )
	{
		wxImageContainerBase::OnMouseLeftDown(event);
		return;
	}

	int imgIdx = -1;
	int where = HitTest(event.GetPosition(), imgIdx);
	if( wxIMG_OVER_EW_BORDER == where && !m_bCollapsed)
	{	
		// We are over the sash
		if( !m_sashRect.IsEmpty() )
			wxArtManagerST::Get()->DrawDragSash(m_sashRect);
		else
		{
			// first time, begin drawing sash
			CaptureMouse();

			// Change mouse cursor
			m_oldCursor = GetCursor();
			wxSetCursor(wxCURSOR_SIZEWE);
		}

		wxRect clientRect = GetClientRect();
		wxPoint pt;

		pt = ClientToScreen(wxPoint(event.GetX(), 0));
		m_sashRect = wxRect(pt, wxSize(4, clientRect.height));

		wxArtManagerST::Get()->DrawDragSash(m_sashRect);
	}
	else
		wxImageContainerBase::OnMouseLeftDown(event);
}

void wxLabelContainer::OnMouseLeaveWindow(wxMouseEvent& event)
{
	if( HasFlag(wxINB_NO_RESIZE) )
	{
		wxImageContainerBase::OnMouseLeaveWindow(event);
		return;
	}

	// If Sash is being dragged, ignore this event
	if( !HasCapture() )
	{
		wxImageContainerBase::OnMouseLeaveWindow(event);		
	}
}

void wxLabelContainer::DrawRegularHover(wxDC& dc, wxRect& rect)
{
	// The hovered tab with default border
	dc.SetBrush(wxBrush(*wxTRANSPARENT_BRUSH));
	dc.SetPen(wxPen(*wxWHITE));

	/// We draw CCW
	if( HasFlag(wxINB_RIGHT) || HasFlag(wxINB_TOP))
	{
		/// Right images
		/// Upper line
		dc.DrawLine(rect.x + 1, rect.y, rect.x + rect.width, rect.y);

		/// Right line (white)
		dc.DrawLine(rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height);

		/// Bottom diagnol - we change pen
		dc.SetPen(wxPen(m_colorsMap[wxINB_TABS_BORDER_COLOR]));

		/// Bottom line
		dc.DrawLine(rect.x + rect.width, rect.y + rect.height, rect.x, rect.y + rect.height);
	}
	else
	{
		/// Left images
		/// Upper line white
		dc.DrawLine(rect.x, rect.y, rect.x + rect.width - 1, rect.y);

		/// Left line
		dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);

		/// Bottom diagnol, we change the pen
		dc.SetPen(wxPen(m_colorsMap[wxINB_TABS_BORDER_COLOR]));

		/// Bottom line
		dc.DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height);
	}
}

void wxLabelContainer::DrawWebHover(wxDC& dc, wxString& caption, int xCoord, int yCoord)
{
	// Redraw the text with underlined font
	wxFont underLinedFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	underLinedFont.SetUnderlined( true );
	dc.SetFont(underLinedFont);
	dc.DrawText(caption, xCoord, yCoord);
}

void wxLabelContainer::SetColour(int which, const wxColor &color)
{
	m_colorsMap[which] = color;
}

wxColor wxLabelContainer::GetColour(int which)
{
	std::map<int, wxColor>::iterator iter = m_colorsMap.find(which);
	if(iter == m_colorsMap.end())
		return wxColor();
	return iter->second;
}

void wxLabelContainer::InitializeColors()
{
	// Initialize map colors
	m_colorsMap[wxINB_TAB_AREA_BACKGROUND_COLOR] = wxArtManagerST::Get()->LightColour(wxArtManagerST::Get()->FrameColour(), 50);
	m_colorsMap[wxINB_ACTIVE_TAB_COLOR] = wxArtManagerST::Get()->GetMenuFaceColour();
	m_colorsMap[wxINB_TABS_BORDER_COLOR] = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
	m_colorsMap[wxINB_HILITE_TAB_COLOR] = wxColor(wxT("LIGHT BLUE"));
	m_colorsMap[wxINB_TEXT_COLOR] = wxColor(wxT("WHITE"));
	m_colorsMap[wxINB_ACTIVE_TEXT_COLOR] = wxColor(wxT("BLACK"));

	// dont allow bright colour one on the other
	if( !wxArtManagerST::Get()->IsDark( m_colorsMap[wxINB_TAB_AREA_BACKGROUND_COLOR]) &&
		!wxArtManagerST::Get()->IsDark( m_colorsMap[wxINB_TEXT_COLOR] ))
	{
		m_colorsMap[wxINB_TEXT_COLOR] = wxArtManagerST::Get()->DarkColour(m_colorsMap[wxINB_TEXT_COLOR], 100);
	}
}

void wxLabelContainer::DrawLabel(wxDC& dc, 
								 wxRect rect, 
								 const wxString& text, 
								 const wxBitmap& bmp,
								 wxImageInfo& imgInfo, 
								 bool orientationLeft, 
								 int imgIdx,
								 bool selected,
								 bool hover)
{
	wxDCSaver dc_saver(&dc);
	wxString caption;
	int w, h;
	const int nPadding = 6;
	
	if( orientationLeft )
	{
		rect.x += nPadding;
		rect.width -= nPadding;
	}
	else
	{
		rect.width -= nPadding;
	}

	wxRect textRect( rect );
	wxRect imgRect( rect );
	
	dc.SetFont( wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT) );

	// First we define the rectangle for the text
	dc.GetTextExtent(text, &w, &h);
	

	//-------------------------------------------------------------------------
	// Label layout:
	// [ nPadding | Image | nPadding | Text | nPadding ]
	//-------------------------------------------------------------------------

	// Text bounding rectangle
	textRect.x += nPadding;

	textRect.y = rect.y + (rect.height - h)/2;
	textRect.width = rect.width - 2 * nPadding;

	if( bmp.Ok() )
	{
		textRect.x += (bmp.GetWidth() + nPadding);
		textRect.width -= (bmp.GetWidth() + nPadding);
	}
	textRect.height = h;

	// Trunctae text if needed
	wxArtManagerST::Get()->TruncateText(dc, text, textRect.width, caption);

	// Image bounding rectangle
	if( bmp.Ok() )
	{
		imgRect.x += nPadding;
		imgRect.width = bmp.GetWidth();
		imgRect.y = rect.y + (rect.height - bmp.GetHeight())/2;
		imgRect.height = bmp.GetHeight();
	}


	// Draw bounding rectangle
	if( selected )
	{
		// First we colour the tab
		dc.SetBrush( wxBrush(m_colorsMap[wxINB_ACTIVE_TAB_COLOR]) );

		if( HasFlag(wxINB_BORDER) ){
			dc.SetPen( wxPen(m_colorsMap[wxINB_TABS_BORDER_COLOR]) );
		} else {
			dc.SetPen( wxPen(m_colorsMap[wxINB_ACTIVE_TAB_COLOR]) );
		}

		wxRect labelRect(rect);

		if( orientationLeft ) {
			labelRect.width += 3;
		} else {
			labelRect.width += 3;
			labelRect.x -= 3;
		}
		dc.DrawRoundedRectangle( labelRect, 3 );

		if( !orientationLeft && HasFlag(wxINB_DRAW_SHADOW)) {
			dc.SetPen( *wxBLACK_PEN);
			dc.DrawPoint(labelRect.x + labelRect.width - 1, labelRect.y + labelRect.height - 1);
		}
	}

	// Draw the text & bitmap
	if( !caption.IsEmpty() )
	{
		selected ?  dc.SetTextForeground(m_colorsMap[wxINB_ACTIVE_TEXT_COLOR]) : dc.SetTextForeground(m_colorsMap[wxINB_TEXT_COLOR]);
		dc.DrawText(caption, textRect.x, textRect.y);
		imgInfo.SetTextRect( textRect );
	}
	else
	{
		imgInfo.SetTextRect( wxRect() );
	}

	if(bmp.Ok())
		dc.DrawBitmap(bmp, imgRect.x, imgRect.y, true);

	// Drop shadow
	if( HasFlag(wxINB_DRAW_SHADOW) && selected )
	{
		long sstyle =0;
		if( orientationLeft )
			sstyle = BottomShadow;
		else
		{
			sstyle = BottomShadowFull | RightShadow;
		}

		if( HasFlag( wxINB_WEB_HILITE ) )
		{
			// Always drop shadow for this style
			wxArtManagerST::Get()->DrawBitmapShadow(dc, rect, sstyle);
		}
		else
		{
			if(imgIdx+1 != m_nHoeveredImgIdx)
			{
				wxArtManagerST::Get()->DrawBitmapShadow(dc, rect, sstyle);
			}
		}
	}

	// Draw hover effect 
	if(hover)
	{
		if( HasFlag( wxINB_WEB_HILITE ) && !caption.IsEmpty())
		{
			DrawWebHover(dc, caption, textRect.x, textRect.y);
		}
		else
			DrawRegularHover(dc, rect);
	}
	
	// Update the page information bout position and size
	imgInfo.SetPosition(rect.GetPosition());
	imgInfo.SetSize(rect.GetSize());
}

BEGIN_EVENT_TABLE(wxLabelBook, wxFlatBookBase)
EVT_SIZE(wxLabelBook::OnSize)
END_EVENT_TABLE()

wxLabelBook::wxLabelBook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxFlatBookBase(parent, id, pos, size, style, name) 
{
	m_pages = CreateImageContainer();

	// Label book specific initialization
	m_mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(m_mainSizer);

	// Add the tab container to the sizer
	m_mainSizer->Add(m_pages, 0, wxEXPAND);
	m_pages->SetSizeHints(wxSize(((wxLabelContainer*)m_pages)->GetTabAreaWidth(), -1));

	// Initialize the colors maps
	((wxLabelContainer*)m_pages)->InitializeColors();
}
	
wxImageContainerBase* wxLabelBook::CreateImageContainer()
{
	return new wxLabelContainer(this, wxID_ANY);
}

void wxLabelBook::SetColour(int which, const wxColor& color)
{
	((wxLabelContainer*)m_pages)->SetColour(which, color);
}

wxColor wxLabelBook::GetColour(int which)
{
	return ((wxLabelContainer*)m_pages)->GetColour(which);
}

void wxLabelBook::OnSize(wxSizeEvent& event)
{
	m_pages->Refresh();
	event.Skip();
}
