///////////////////////////////////////////////////////////////////////////////
// Name:		art_manager.h
// Purpose:     Art provider auxilary class
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#include "art_manager.h"
#include "fm_renderer.h"
#include <wx/image.h>
#include <wx/splash.h>
#include <wx/mstream.h>
#include <wx/dcmemory.h>
#include <wx/settings.h>
#include <wx/dcscreen.h>

#ifdef __WXMSW__

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif 

#endif


wxArtManager::wxArtManager(void)
: m_alignmentBuffer( 7 )
, m_menuTheme(StyleXP)
, m_ms2007sunken(false)
, m_verticalGradient(false)
, m_config(NULL)
, m_drowMBBorder(true)
, m_menuBgFactor(5)
, m_menuBarColourScheme(wxT("Default"))
, m_raiseTB(true)
{
	/// Initialise random generator
	srand( time(NULL) );

	// Initialize the renderers map
	m_renderers[StyleXP] = new wxFMRendererXP();
	m_renderers[Style2007] = new wxFMRendererMSOffice2007();

	// create wxBitmaps from the xpm's
	if( !m_rightTop.Ok() )
		ConvertToBitmap(shadow_right_top, shadow_right_top_size, m_rightTop);

	if( !m_right.Ok() )
		ConvertToBitmap(shadow_right, shadow_right_size, m_right);

	if( !m_rightBottomCorner.Ok() )
		ConvertToBitmap(shadow_center, shadow_center_size, m_rightBottomCorner);

	if( !m_bottom.Ok() )
		ConvertToBitmap(shadow_buttom, shadow_buttom_size, m_bottom);

	if( !m_bottomLeft.Ok() )
		ConvertToBitmap(shadow_buttom_left, shadow_buttom_left_size, m_bottomLeft);

	// connect an event handler to the system colour change event
	Connect(wxEVT_SYS_COLOUR_CHANGED, wxSysColourChangedEventHandler(wxArtManager::OnSysColourChange));

	// initialise the colour map
	InitColours();
	SetMenuBarColour( m_menuBarColourScheme );

	// we need PNG & BMP handlers
	wxImage::AddHandler( new wxPNGHandler );
	wxImage::AddHandler( new wxBMPHandler );

	// Create common bitmaps
	FillStokBitmaps();
}

wxArtManager::~wxArtManager(void)
{
	wxRenderersMap::iterator iter = m_renderers.begin();
	for(; iter != m_renderers.end(); iter++)
	{
		wxFMRendererBase* renderer = iter->second;
		delete renderer;
	}

	if(m_config)
	{
		delete m_config;
		m_config = NULL;
	}
}

void wxArtManager::FillStokBitmaps()
{
	wxBitmapPtr bmp(new wxBitmap());
	ConvertToBitmap(arrow_down, arrow_down_size, *bmp);
	bmp->SetMask(new wxMask(*bmp, wxColor(0, 128, 128)));
	m_bitmaps[wxT("arrow_down")] = bmp;

	wxBitmapPtr bmp1(new wxBitmap());
	ConvertToBitmap(arrow_up, arrow_up_size, *bmp1);
	bmp1->SetMask(new wxMask(*bmp1, wxColor(0, 128, 128)));
	m_bitmaps[wxT("arrow_up")] = bmp1;
}

void wxArtManager::OnSysColourChange(wxSysColourChangedEvent &event)
{
	wxUnusedVar(event);

	// reinitialise the colour map
	InitColours();
}

wxColor wxArtManager::LightColour(const wxColour& color, int percent) const
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT("WHITE");
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> white
	int i = percent;
	int r = color.Red() +  ((i*rd*100)/high)/100;
	int g = color.Green() + ((i*gd*100)/high)/100;
	int b = color.Blue() + ((i*bd*100)/high)/100;
	return wxColor(r, g, b);
}

wxColor wxArtManager::DarkColour(const wxColour& color, int percent) const
{
	int rd, gd, bd, high = 0;
	wxColor end_color = wxT("BLACK");
	rd = end_color.Red() - color.Red();
	gd = end_color.Green() - color.Green();
	bd = end_color.Blue() - color.Blue();
	high = 100;

	// We take the percent way of the color from color --> black
	int i = percent;
	int r = color.Red() +  ((i*rd*100)/high)/100;
	int g = color.Green() + ((i*gd*100)/high)/100;
	int b = color.Blue() + ((i*bd*100)/high)/100;
	return wxColor(r, g, b);
}

void wxArtManager::PaintStraightGradientBox(wxDC& dc,
											const wxRect& rect,
											const wxColour& startColor,
											const wxColour& endColor,
											bool  vertical /* true */)
{
	int rd, gd, bd, high = 0;
	rd = endColor.Red() - startColor.Red();
	gd = endColor.Green() - startColor.Green();
	bd = endColor.Blue() - startColor.Blue();

	/// Save the current pen and brush
	wxPen savedPen = dc.GetPen();
	wxBrush savedBrush = dc.GetBrush();

	if ( vertical )
		high = rect.GetHeight()-1;
	else
		high = rect.GetWidth()-1;

	if( high < 1 )
		return;

	for (int i = 0; i <= high; ++i)
	{
		int r = startColor.Red() +  ((i*rd*100)/high)/100;
		int g = startColor.Green() + ((i*gd*100)/high)/100;
		int b = startColor.Blue() + ((i*bd*100)/high)/100;

		wxPen p(wxColor(r, g, b));
		dc.SetPen(p);

		if ( vertical )
			dc.DrawLine(rect.x, rect.y+i, rect.x+rect.width, rect.y+i);
		else
			dc.DrawLine(rect.x+i, rect.y, rect.x+i, rect.y+rect.height);
	}

	/// Restore the pen and brush
	dc.SetPen( savedPen );
	dc.SetBrush( savedBrush );
}

void wxArtManager::PaintGradientRegion(wxDC& dc, const wxRegion& region, const wxColour& startColor, const wxColour& endColor, bool vertical)
{
	// The way to achieve non-rectangle 
	wxMemoryDcEx memDC;
	wxRect rect = region.GetBox();
	wxBitmap bitmap(rect.width , rect.height );
	memDC.SelectObject(bitmap);

	// Color the whole rectangle with gradient
	wxRect rr( 0, 0, rect.width, rect.height );
	PaintStraightGradientBox(memDC, rr, startColor, endColor, vertical);

	// Convert the region to a black and white bitmap with the white pixels being inside the region
	// we draw the bitmap over the gradient colored rectangle, with mask set to white, 
	// this will cause our region to be colored with the gradient, while area outside the 
	// region will be painted with black. then we simply draw the bitmap to the dc with mask set to 
	// black
	wxRegion tmpRegion ( region );
	tmpRegion.Offset( -rect.x, -rect.y );
	wxBitmap regionBmp = tmpRegion.ConvertToBitmap();
	regionBmp.SetMask( new wxMask(regionBmp, *wxWHITE) );

	// The function ConvertToBitmap() return a rectangle bitmap
	// which is shorter by 1 pixl on the height and width (this is correct behavior, since 
	// DrawLine does not include the second point as part of the line)
	// we fix this issue by drawing our own line at the bottom and left side of the rectangle
	memDC.SetPen(*wxBLACK_PEN);
	memDC.DrawBitmap(regionBmp, 0, 0, true);
	memDC.DrawLine(0, rr.height - 1, rr.width, rr.height - 1);
	memDC.DrawLine(rr.width - 1, 0, rr.width - 1, rr.height);

	memDC.SelectObject(wxNullBitmap);
	bitmap.SetMask(new wxMask(bitmap, *wxBLACK));
	dc.DrawBitmap(bitmap, rect.x, rect.y, true);
}
void wxArtManager::PaintDiagonalGradientBox(wxDC &dc, 
											const wxRect &rect, 
											const wxColor &startColor,
											const wxColor &endColor,
											bool startAtUpperLeft,
											bool trimToSquare)
{   
	/// Save the current pen and brush
	wxPen savedPen = dc.GetPen();
	wxBrush savedBrush = dc.GetBrush();

	// gradient fill from colour 1 to colour 2 with top to bottom
	if(rect.height < 1 || rect.width < 1)
		return;

	// calculate some basic numbers
	int size = rect.width, sizeX = 0, sizeY = 0;
	double proportion = 1;
	if(rect.width > rect.height)
	{
		if(trimToSquare)
		{
			size = rect.height;
			sizeX = sizeY = rect.height - 1;
		}
		else
		{
			proportion = (double)rect.height / (double)rect.width;
			size = rect.width;
			sizeX = rect.width - 1;
			sizeY = rect.height -1;
		}
	}
	else
	{
		if(trimToSquare)
		{
			size = rect.width;
			sizeX = sizeY = rect.width - 1;
		}
		else
		{
			sizeX = rect.width - 1;
			size = rect.height;
			sizeY = rect.height - 1;
			proportion = (double)rect.width / (double)rect.height;
		}
	}

	// calculate gradient coefficients
	wxColour col2 = endColor;
	wxColour col1 = startColor;

	double rstep = double((col2.Red() - col1.Red())) / double(size), rf = 0,
		gstep = double((col2.Green() - col1.Green())) / double(size), gf = 0,
		bstep = double((col2.Blue() -  col1.Blue())) / double(size), bf = 0;

	wxColour currCol;
	// draw the upper triangle
	for(int i = 0; i < size; i++)
	{
		currCol.Set(
			(unsigned char)(col1.Red() + rf),
			(unsigned char)(col1.Green() + gf),
			(unsigned char)(col1.Blue() + bf)
			);
		dc.SetBrush( wxBrush( currCol, wxSOLID ) );
		dc.SetPen(wxPen(currCol));
		if(startAtUpperLeft)
		{
			if(rect.width > rect.height)
			{
				dc.DrawLine(rect.x + i, rect.y, rect.x, (int)(rect.y + proportion * i));
				dc.DrawPoint(rect.x, (int)(rect.y + proportion * i));
			}
			else
			{
				dc.DrawLine((int)(rect.x + proportion * i), rect.y, rect.x, rect.y + i);
				dc.DrawPoint(rect.x, rect.y + i);
			}
		}
		else
		{
			if(rect.width > rect.height)
			{
				dc.DrawLine(rect.x + sizeX - i, rect.y, rect.x + sizeX, (int)(rect.y + proportion * i));
				dc.DrawPoint(rect.x + sizeX, (int)(rect.y + proportion * i));
			}
			else
			{
				int xTo = (int)(rect.x + sizeX - proportion * i) > rect.x ? (int)(rect.x + sizeX - proportion * i) : rect.x;
				dc.DrawLine(xTo, rect.y, rect.x + sizeX, rect.y + i);
				dc.DrawPoint(rect.x + sizeX, rect.y + i);
			}
		}
		rf += rstep/2; gf += gstep/2; bf += bstep/2;
	}
	// draw the lower triangle
	for(int i = 0; i < size; i++)
	{
		currCol.Set(
			(unsigned char)(col1.Red() + rf),
			(unsigned char)(col1.Green() + gf),
			(unsigned char)(col1.Blue() + bf)
			);
		dc.SetBrush( wxBrush( currCol, wxSOLID ) );
		dc.SetPen(wxPen(currCol));
		if(startAtUpperLeft)
		{
			if(rect.width > rect.height)
			{
				dc.DrawLine(rect.x + i, rect.y + sizeY, rect.x + sizeX, (int)(rect.y + proportion * i));
				dc.DrawPoint(rect.x + sizeX, (int)(rect.y + proportion * i));
			}
			else
			{
				dc.DrawLine((int)(rect.x + proportion * i), rect.y + sizeY, rect.x + sizeX, rect.y + i);
				dc.DrawPoint(rect.x + sizeX, rect.y + i);
			}
		}
		else
		{
			if(rect.width > rect.height)
			{
				dc.DrawLine(rect.x, (int)(rect.y + proportion * i), rect.x + sizeX - i, rect.y + sizeY);
				dc.DrawPoint(rect.x + sizeX - i, rect.y + sizeY);
			}
			else
			{
				int xTo = (int)(rect.x + sizeX - proportion * i) > rect.x ? (int)(rect.x + sizeX - proportion * i) : rect.x;
				dc.DrawLine(rect.x, rect.y + i, xTo, rect.y + sizeY);
				dc.DrawPoint(xTo, rect.y + sizeY);
			}
		}
		rf += rstep/2; gf += gstep/2; bf += bstep/2;
	}

	/// Restore the pen and brush
	dc.SetPen( savedPen );
	dc.SetBrush( savedBrush );
}
void wxArtManager::PaintCrescentGradientBox(wxDC& dc,
											const wxRect& rect,
											const wxColour& startColor,
											const wxColour& endColor,
											const bool& concave)
{
	int diagonalRectWidth = rect.GetWidth() / 4;
	int spare = rect.width - 4 * diagonalRectWidth;
	wxRect leftRect(rect.x, rect.y, diagonalRectWidth, rect.GetHeight());
	wxRect rightRect(rect.x + 3 * diagonalRectWidth + spare, rect.y, diagonalRectWidth, rect.GetHeight());
	
	if(concave)
	{
		PaintStraightGradientBox(dc, rect, MixColors(startColor, endColor, 50), endColor);
		PaintDiagonalGradientBox(dc, leftRect, startColor, endColor, true, false); 
		PaintDiagonalGradientBox(dc, rightRect, startColor, endColor, false, false); 
	}
	else
	{
		PaintStraightGradientBox(dc, rect, endColor, MixColors(endColor, startColor, 50));
		PaintDiagonalGradientBox(dc, leftRect, endColor, startColor, false, false); 
		PaintDiagonalGradientBox(dc, rightRect, endColor, startColor, true, false); 
	}
}
wxColor wxArtManager::FrameColour() const
{
	return wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
}

wxColor wxArtManager::BackgroundColor() const
{
	return LightColour(FrameColour(), 75);
}

wxColor wxArtManager::HighlightBackgroundColor() const
{
	return LightColour(FrameColour(), 60);
}

wxColor wxArtManager::MixColors(const wxColour& firstColor, 
								const wxColour& secondColor, 
								int percent) const
{
	// calculate gradient coefficients
	double redOffset = double((secondColor.Red() * (100 - percent) / 100) - (firstColor.Red() * percent / 100));
	double greenOffset = double((secondColor.Green() * (100 - percent) / 100) - (firstColor.Green() * percent / 100));
	double blueOffset = double((secondColor.Blue() * (100 - percent) / 100) -  (firstColor.Blue() * percent / 100));

	return wxColor((unsigned char)(firstColor.Red() + redOffset),
			(unsigned char)(firstColor.Green() + greenOffset),
			(unsigned char)(firstColor.Blue() + blueOffset));
}

wxColor wxArtManager::RandomColor() const
{
	int r, g, b;
	r = rand() % 256; // Random value betweem 0-255
	g = rand() % 256; // Random value betweem 0-255
	b = rand() % 256; // Random value betweem 0-255
	return wxColor(r, g, b);
}

bool wxArtManager::IsDark(const wxColor &color) const
{
	int evg = (color.Red() + color.Green() + color.Blue())/3;
	if(evg < 127)
		return true;

	return false;
}

void wxArtManager::TruncateText(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText)
{
	int textH, textW;
	int rectSize = maxWidth;
	int textLen = (int)text.Length();
	wxString tempText = text;

	fixedText = wxT("");
	dc.GetTextExtent(text, &textW, &textH);
	if(rectSize >= textW)
	{
		fixedText = text;
		return;
	}

	// The text does not fit in the designated area, 
	// so we need to truncate it a bit
	wxString suffix = wxT("..");
	int w, h;
	dc.GetTextExtent(suffix, &w, &h);
	rectSize -= w;

	for(int i=textLen; i>=0; i--)
	{
		dc.GetTextExtent(tempText, &textW, &textH);
		if(rectSize >= textW)
		{
			fixedText = tempText;
			fixedText += wxT("..");
			return;
		}
		tempText = tempText.RemoveLast();
	}
}

void wxArtManager::DrawButton(wxDC& dc, 
							  const wxRect &rect, 
							  const wxControlTheme &theme, 
							  const ControlState &state,
							  const bool useLightColours)
{
	wxRenderersMap::iterator iter = m_renderers.find(theme);
	wxASSERT_MSG(iter != m_renderers.end(), wxT("Invalid theme!"));

	wxFMRendererBase* renderer = iter->second;

	// Set background color if non given by caller
	renderer->DrawButton(dc, rect, state, useLightColours);
}

void wxArtManager::DrawButton(wxDC& dc, 
							  const wxRect &rect, 
							  const wxControlTheme &theme, 
							  const ControlState &state,
							  const wxColour &color)
{
	wxRenderersMap::iterator iter = m_renderers.find(theme);
	wxASSERT_MSG(iter != m_renderers.end(), wxT("Invalid theme!"));

	wxFMRendererBase* renderer = iter->second;

	// Set background color if non given by caller
	renderer->DrawButton(dc, rect, state, color);
}

// on supported windows systems (Win2000 and greater), this function
// will make a frame window transparent by a certain amount
void wxArtManager::MakeWindowTransparent(wxWindow* wnd, int amount)
{
	wxUnusedVar(wnd);
	wxUnusedVar(amount);
#if 0
	// this API call is not in all SDKs, only the newer ones, so
	// we will runtime bind this
	typedef DWORD (WINAPI *PSETLAYEREDWINDOWATTR)(HWND, DWORD, BYTE, DWORD);
	static PSETLAYEREDWINDOWATTR pSetLayeredWindowAttributes = NULL;
	static HMODULE h = NULL;
	HWND hwnd = (HWND)wnd->GetHWND();

	if (!h)
		h = LoadLibrary(_T("user32"));

	if (!pSetLayeredWindowAttributes)
	{
		pSetLayeredWindowAttributes =
			(PSETLAYEREDWINDOWATTR)GetProcAddress(h, "SetLayeredWindowAttributes");
	}
	if (pSetLayeredWindowAttributes == NULL)
		return;

	LONG exstyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	if (0 == (exstyle & 0x80000) /*WS_EX_LAYERED*/)
		SetWindowLong(hwnd, GWL_EXSTYLE, exstyle | 0x80000 /*WS_EX_LAYERED*/);  
	pSetLayeredWindowAttributes(hwnd, 0, amount, 2 /*LWA_ALPHA*/);
#endif
}

// assumption: the background was already drawn on the dc
void wxArtManager::DrawBitmapShadow(wxDC& dc, const wxRect& rect, int where)
{
	static const int shadowSize = 5;

	// the rect must be at least 5x5 pixles
	if( rect.height < 2*shadowSize || rect.width < 2*shadowSize )
		return;

	// Start by drawing the right bottom corner
	if( (where & BottomShadow) || (where & BottomShadowFull)) {
		dc.DrawBitmap(m_rightBottomCorner, rect.x+rect.width, rect.y+rect.height, true);
	}

	int xx, yy;

	// Draw right side shadow
	xx = rect.x + rect.width;
	yy = rect.y + rect.height - shadowSize;

	if( where & RightShadow ) {
		while( yy - rect.y > 2 * shadowSize ) {
			dc.DrawBitmap(m_right, xx, yy, true);
			yy -= shadowSize;
		}
		dc.DrawBitmap(m_rightTop, xx, yy - shadowSize, true);
	}

	if( where & BottomShadow ){
		xx = rect.x + rect.width - shadowSize;
		yy = rect.height + rect.y;
		while( xx - rect.x > 2 * shadowSize ) {
			dc.DrawBitmap(m_bottom, xx, yy, true);
			xx -= shadowSize;
		}

		dc.DrawBitmap(m_bottomLeft, xx - shadowSize, yy, true);
	}

	if( where & BottomShadowFull ){
		xx = rect.x + rect.width - shadowSize;
		yy = rect.height + rect.y;
		while( xx - rect.x >= 0 ) {
			dc.DrawBitmap(m_bottom, xx, yy, true);
			xx -= shadowSize;
		}
		dc.DrawBitmap(m_bottom, xx, yy, true);
	}
}

void wxArtManager::DropShadow(wxWindow* wnd, bool drop)
{
	wxUnusedVar(wnd);
	wxUnusedVar(drop);
#if 0
	HWND hwnd = (HWND)wnd->GetHWND();
	ULONG_PTR csstyle = GetClassLongPtr(hwnd, GCL_STYLE);
	if(drop)
	{
		if( csstyle & CS_DROPSHADOW )
			return;
		else
			csstyle |= CS_DROPSHADOW;//Nothing to be done
	}
	else
	{
		if( csstyle & CS_DROPSHADOW )
			csstyle &= ~(CS_DROPSHADOW);
		else
			return;	//Nothing to be done
	}

	SetClassLongPtr(hwnd, GCL_STYLE, csstyle);
#endif
}

void wxArtManager::GetBitmapStartLocation(wxDC &dc, 
										  const wxRect &rect, 
										  int &startLocationX, 
										  int &startLocationY,
										  const wxBitmap &bitmap,
										  const wxString &text,
										  const long &style)
{
	int alignmentBuffer = GetAlignBuffer();

	// get the startLocationY
	int fixedTextWidth = 0, fixedTextHeight = 0;
	if(text.IsEmpty())
		fixedTextHeight = bitmap.GetHeight();
	else
		dc.GetTextExtent(text, &fixedTextWidth, &fixedTextHeight);
	startLocationY = rect.y + (rect.height - fixedTextHeight) / 2;

	// get the startLocationX
	if(style & wxBU_EXT_RIGHT_TO_LEFT_STYLE)
	{
		startLocationX = rect.x + rect.width - alignmentBuffer - bitmap.GetWidth();
	}
	else
	{
		if(style & wxBU_EXT_RIGHT_ALIGN_STYLE)
		{
			int maxWidth = rect.x + rect.width - (2 * alignmentBuffer) - bitmap.GetWidth(); // the alignment is for both sides
			
			// get the truncaed text. The text may stay as is, it is not a must that is will be trancated
			wxString fixedText;
			TruncateText(dc, text, maxWidth, fixedText);

			// get the fixed text dimentions
			dc.GetTextExtent(fixedText, &fixedTextWidth, &fixedTextHeight);

			// calculate the start location
			startLocationX = maxWidth - fixedTextWidth;
		}
		else if(style & wxBU_EXT_LEFT_ALIGN_STYLE)
		{
			// calculate the start location
			startLocationX = alignmentBuffer;
		}
		else // meaning wxBU_EXT_CENTER_ALIGN_STYLE
		{
			int maxWidth = rect.x + rect.width - (2 * alignmentBuffer) - bitmap.GetWidth(); // the alignment is for both sides

			// get the truncaed text. The text may stay as is, it is not a must that is will be trancated
			wxString fixedText;
			TruncateText(dc, text, maxWidth, fixedText);

			// get the fixed text dimentions
			dc.GetTextExtent(fixedText, &fixedTextWidth, &fixedTextHeight);

			if(maxWidth > fixedTextWidth)
			{
				// calculate the start location
				startLocationX = (maxWidth - fixedTextWidth) / 2;
			}
			else
			{
				// calculate the start location
				startLocationX = maxWidth - fixedTextWidth;
			}
		}		
	}
	// it is very important to validate that the start location is not less than the alignment buffer
	if(startLocationX < alignmentBuffer)
		startLocationX = alignmentBuffer;
}
void wxArtManager::GetTextStartLocation(wxDC &dc, 
										const wxRect &rect, 
										wxString &fixedText, 
										int &startLocationX, 
										int &startLocationY,
										const wxBitmap &bitmap,
										const wxString &text,
										const long &style)
{
	int alignmentBuffer = GetAlignBuffer();

	// get the bitmap offset
	int bitmapOffset = 0;
	if(bitmap.Ok())
		bitmapOffset = bitmap.GetWidth();

	// get the truncated text. The text may stay as is, it is not a must that is will be trancated
	int maxWidth = rect.x + rect.width - (2 * alignmentBuffer) - bitmapOffset; // the alignment is for both sides
	TruncateText(dc, text, maxWidth, fixedText);

	// get the startLocationY
	int fixedTextWidth = 0, fixedTextHeight = 0;
	dc.GetTextExtent(fixedText, &fixedTextWidth, &fixedTextHeight);
	startLocationY = (rect.height - fixedTextHeight) / 2 + rect.y;

	// get the startLocationX
	if(style & wxBU_EXT_RIGHT_TO_LEFT_STYLE)
	{
		startLocationX = maxWidth - fixedTextWidth + alignmentBuffer;
	}
	else
	{
		if(style & wxBU_EXT_LEFT_ALIGN_STYLE)
		{
			// calculate the start location
			startLocationX = bitmapOffset + alignmentBuffer;
		}
		else if(style & wxBU_EXT_RIGHT_ALIGN_STYLE)
		{
			// calculate the start location
			startLocationX = maxWidth - fixedTextWidth + bitmapOffset + alignmentBuffer;
		}
		else // meaning wxBU_EXT_CENTER_ALIGN_STYLE
		{
			// calculate the start location
			startLocationX = (maxWidth - fixedTextWidth) / 2 + bitmapOffset + alignmentBuffer;
		}
	}
	// it is very important to validate that the start location is not less than the alignment buffer
	if(startLocationX < alignmentBuffer)
		startLocationX = alignmentBuffer;
}
void wxArtManager::DrawTextAndBitmap(wxDC &dc, 
									 const wxRect &rect,
									 const wxString &text,
									 const bool &enable,
									 const wxFont &font,
									 const wxColor &fontColor,
									 const wxBitmap &bitmap,
									 const wxBitmap &grayBitmap,
									 const long &style)
{
	// enable colors
	if(enable)
		dc.SetTextForeground(fontColor);
	else
		dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	
	// incase this button is selected, mark it
//	MarkButton(dc, rect);

	// set the font
	wxFont myFont = font;
	if(myFont == wxNullFont)
	{
//		myFont = wxFont (10, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("MIRIAM"), wxFONTENCODING_ISO8859_8);
		myFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	}
	dc.SetFont(myFont);
	
	int startLocationX = 0, startLocationY = 0;
	if(bitmap.Ok())
	{
		// calculate the bitmap start location
		GetBitmapStartLocation(dc, rect, startLocationX, startLocationY, bitmap, text, style);

		// draw the bitmap
		if(enable)
			dc.DrawBitmap(bitmap, startLocationX, startLocationY, true);
		else
			dc.DrawBitmap(grayBitmap, startLocationX, startLocationY, true);
	}

	// calculate the text start location
	wxString fixedText;
	wxString labelOnly;
	int location = GetAccelIndex(text, labelOnly);
	GetTextStartLocation(dc, rect, fixedText, startLocationX, startLocationY, bitmap, labelOnly, style);
	
	// after all the caculations are finished, it is time to draw the text
	// underline the first letter that is marked with a '&'
	if((location == -1) || (myFont.GetUnderlined()) || (location >= (int)fixedText.Length()))
	{
		// draw the text
		dc.DrawText(fixedText, startLocationX, startLocationY);
	}
	else
	{	
		// underline the first '&'
		wxString before = fixedText.Mid(0, (size_t)location);
		wxString underlineLetter = fixedText[location]; 
		wxString after = fixedText.Mid((size_t)location+1); 

		// before
		dc.DrawText(before, startLocationX, startLocationY);

		// underlineLetter
#ifndef __WXGTK__
		int w1, h;
		dc.GetTextExtent(before, &w1, &h);
		myFont.SetUnderlined(true);
		dc.SetFont( myFont );
		dc.DrawText(underlineLetter, startLocationX + w1, startLocationY);
#else
		int w1, h;
		dc.GetTextExtent(before, &w1, &h);
		dc.DrawText(underlineLetter, startLocationX + w1, startLocationY);

		// Draw the underline ourselves since using the Underline in GTK, 
		// causes the line to be too close to the letter
		int uderlineLetterW, uderlineLetterH;
		dc.GetTextExtent(underlineLetter, &uderlineLetterW, &uderlineLetterH);
		wxPen curPen = dc.GetPen();
		dc.SetPen( *wxBLACK_PEN );
		dc.DrawLine(startLocationX + w1, startLocationY + uderlineLetterH - 2, startLocationX + w1 + uderlineLetterW, startLocationY + uderlineLetterH - 2);
		dc.SetPen( curPen );
#endif 
		// after
		int w2;
		dc.GetTextExtent(underlineLetter, &w2, &h);
		myFont.SetUnderlined(false);
		dc.SetFont(myFont);
		dc.DrawText(after, startLocationX + w1 + w2, startLocationY);
	}	
}

wxSize wxArtManager::CalcButtonBestSize(const wxString& label, const wxBitmap& bmp) const
{
	wxMemoryDcEx dc;
	wxBitmap dummy(1, 1);

#ifdef __WXMSW__
	static const int HEIGHT = 22;
#else
	static const int HEIGHT = 26;
#endif
	wxCoord width, height;

	wxFont font(10, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("MIRIAM"), wxFONTENCODING_ISO8859_8);
	dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
//	dc.SetFont(font);
	dc.GetMultiLineTextExtent(label, &width, &height);

	width += 2* GetAlignBuffer(); 

	if ( bmp.Ok() )
	{
		// allocate extra space for the bitmap
		wxCoord heightBmp = bmp.GetHeight() + 2;
		if ( height < heightBmp )
			height = heightBmp;

		width += bmp.GetWidth() + 2;
	}
	if(height < HEIGHT)
		height = HEIGHT;
	return wxSize(width, height);
}

wxColour wxArtManager::GetMenuFaceColour() const 
{
	return LightColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 80);
}

int wxArtManager::GetAccelIndex(const wxString& label, wxString& labelOnly)
{
    static const wxChar MNEMONIC_PREFIX = _T('&');
    int indexAccel = -1;
	labelOnly.Empty();
    for ( const wxChar *pc = label; *pc != wxT('\0'); pc++ )
    {
        if ( *pc == MNEMONIC_PREFIX )
        {
            pc++; // skip it
            if ( *pc != MNEMONIC_PREFIX )
            {
                if ( indexAccel == -1 )
                {
                    // remember it (-1 is for MNEMONIC_PREFIX itself
                    indexAccel = pc - label.c_str() - 1;
                }
                else
                {
                    wxFAIL_MSG(_T("duplicate accel char in control label"));
                }
            }
        }
        labelOnly += *pc;
    }
    return indexAccel;
}

wxColour wxArtManager::GetThemeBaseColour(const bool useLightColours) const 
{
	wxColour color; 
	if( !useLightColours && !IsDark(FrameColour()) )
	{
		color = wxColour(wxT("GOLD"));
	}
	else
	{
		color = LightColour(FrameColour(), 30);
	}
	
	return color;
}
void wxArtManager::DrawDragSash(const wxRect& rect)
{
	static unsigned char data[] = { 0,0,0,192,192,192, 192,192,192,0,0,0 };
	static wxImage img(2, 2, data, true);
	static wxBitmap stipple(img);

	wxScreenDC dc;
	wxMemoryDcEx mem_dc;
	wxBitmap bmp(rect.width, rect.height);
    wxBrush brush(stipple);

	mem_dc.SelectObject(bmp);
    mem_dc.SetBrush(brush);    
    mem_dc.SetPen(*wxTRANSPARENT_PEN);
	mem_dc.DrawRectangle(0, 0, rect.width, rect.height);

	dc.Blit(rect.x, rect.y, rect.width, rect.height, &mem_dc, 0, 0, wxXOR);
}

void wxArtManager::DarkPixel(unsigned char **newimage, const wxImage& img, int x, int y, int amount)
{
	int columns  = img.GetWidth();
	
	// Calculate the offset in the array for (x, y)
	int offset = (columns * 3 * y) + (x * 3);
	wxColour currColor((*newimage)[offset], (*newimage)[offset+1], (*newimage)[offset+2]);
	
	wxColour newColor = DarkColour(currColor, amount);
	
	(*newimage)[offset]   = newColor.Red();
	(*newimage)[offset+1] = newColor.Green();
	(*newimage)[offset+2] = newColor.Blue();
}

void wxArtManager::TakeScreenShot(const wxRect& rect, wxBitmap &bmp)
{
	//Create a DC for the whole screen area
	wxScreenDC dcScreen;

	//Create a Bitmap that will later on hold the screenshot image
	//Note that the Bitmap must have a size big enough to hold the screenshot
	//-1 means using the current default colour depth
	bmp = wxBitmap(rect.width, rect.height);

	//Create a memory DC that will be used for actually taking the screenshot
	wxMemoryDcEx memDC;

	//Tell the memory DC to use our Bitmap
	//all drawing action on the memory DC will go to the Bitmap now
	memDC.SelectObject(bmp);

	//Blit (in this case copy) the actual screen on the memory DC
	//and thus the Bitmap
	memDC.Blit( 0, //Copy to this X coordinate
		0, //Copy to this Y coordinate
		rect.width, //Copy this width
		rect.height, //Copy this height
		&dcScreen, //From where do we copy?
		rect.x, //What's the X offset in the original DC?
		rect.y  //What's the Y offset in the original DC?
		);

	//Select the Bitmap out of the memory DC by selecting a new
	//uninitialized Bitmap
	memDC.SelectObject(wxNullBitmap);
}

void wxArtManager::DrawMenuBarBg(wxDC& dc, const wxRect &rect)
{
	
	wxRenderersMap::iterator iter = m_renderers.find(GetMenuTheme());
	wxASSERT_MSG(iter != m_renderers.end(), wxT("Invalid theme!"));

	wxFMRendererBase* renderer = iter->second;

	// Set background color if non given by caller
	renderer->DrawMenuBarBg(dc, rect);
}

void wxArtManager::DrawToolBarBg(wxDC& dc, const wxRect &rect)
{
	
	wxRenderersMap::iterator iter = m_renderers.find(GetMenuTheme());
	wxASSERT_MSG(iter != m_renderers.end(), wxT("Invalid theme!"));

	wxFMRendererBase* renderer = iter->second;

	// Set background color if non given by caller
	renderer->DrawToolBarBg(dc, rect);
}

void wxArtManager::SetMenuBarColour(const wxString &scheme)
{
	m_menuBarColourScheme = scheme;
	// set default colour
	m_menuBarBgColour = m_colorSchemeMap[wxT("Default")];
	std::map<wxString, wxColour>::iterator iter = m_colorSchemeMap.find(scheme);
	if( iter != m_colorSchemeMap.end() )
		m_menuBarBgColour = iter->second;
}

wxString wxArtManager::GetMenuBarColourScheme() const
{
	return m_menuBarColourScheme;
}

wxColour wxArtManager::GetMenuBarFaceColour() const
{
	return m_menuBarBgColour;
}

wxColour wxArtManager::GetMenuBarSelectionColour() const
{
	return m_menuBarSelColour;
}

void wxArtManager::InitColours()
{
	m_colorSchemeMap[wxT("Default")] = DarkColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 50);
	m_colorSchemeMap[wxT("Dark")] = wxColour(*wxBLACK);
	m_colorSchemeMap[wxT("Dark Olive Green")] = wxColour(wxT("DARK OLIVE GREEN"));
	m_colorSchemeMap[wxT("Generic")] = wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION);
}

wxBitmap wxArtManager::CreateGreyBitmap(wxBitmap bmp) const
{
	// save the file to PNG format
	if( !bmp.SaveFile(wxT("__art_manager_tmp_png_file.png"), wxBITMAP_TYPE_PNG) )
		return bmp;

	wxBitmap greyBitmap(wxT("__art_manager_tmp_png_file.png"), wxBITMAP_TYPE_PNG);
	wxRemoveFile(wxT("__art_manager_tmp_png_file.png"));

	wxImage image = greyBitmap.ConvertToImage();
	image.SetOption(wxIMAGE_OPTION_PNG_FORMAT, wxPNG_TYPE_GREY_RED); 
	image.SaveFile(wxT("__art_manager_tmp_png_file_GREY.png"), wxBITMAP_TYPE_PNG);
	wxBitmap gb(wxT("__art_manager_tmp_png_file_GREY.png"), wxBITMAP_TYPE_PNG);
	wxRemoveFile(wxT("__art_manager_tmp_png_file_GREY.png"));

	return gb;
}

void wxArtManager::ConvertToBitmap(unsigned char *data, const size_t size, wxBitmap &bmp)
{
	wxInputStream *str = new wxMemoryInputStream(data, size);
	wxImage img(*str);
	bmp = wxBitmap(img);
	delete str;
}

wxBitmapPtr wxArtManager::GetStokBitmap(const wxString &name) const
{
	std::map<wxString, wxBitmapPtr>::const_iterator iter = m_bitmaps.find(name);
	if( iter != m_bitmaps.end())
		return iter->second;
	return wxBitmapPtr(NULL);
}
