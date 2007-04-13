// Name:		fm_renderer.h
// Purpose:     define the flat menu renderer classes
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#include "art_manager.h"
#include <wx/gdicmn.h>
#include <wx/region.h>
#include <wx/settings.h>

void wxFMRendererBase::DrawButtonBorders(wxDC &dc, 
										 const wxRect &rect, 
										 const wxColor &penColor, 
										 const wxColor &brushColor)
{
	// Keep old pen and brush
	wxDCSaver dcsaver( &dc );
	dc.SetPen(wxPen(penColor));
	dc.SetBrush(wxBrush(brushColor));
	dc.DrawRectangle(rect);
}

void wxFMRendererBase::DrawBitmapArea(wxDC &dc, 
									  char *xpm_name[], 
									  const wxRect &rect,
									  const wxColor &baseColor, 
									  const bool &flipSide)
{
	// draw the grandient area
	if(!flipSide)
		wxArtManagerST::Get()->PaintDiagonalGradientBox(dc, 
			rect, wxColor(wxT("WHITE")), wxArtManagerST::Get()->LightColour(baseColor, 20), true, false);
	else
		wxArtManagerST::Get()->PaintDiagonalGradientBox(dc, 
			rect, wxArtManagerST::Get()->LightColour(baseColor, 20), wxColor(wxT("WHITE")), true, false);

	// draw arrow
	wxBitmap arrowDown(xpm_name);
	arrowDown.SetMask(new wxMask(arrowDown, wxColor(*wxWHITE)));
	dc.DrawBitmap(arrowDown, rect.x + 1 , rect.y + 1, true);
}
void wxFMRendererBase::DrawBitmapBorders(wxDC &dc, 
									  const wxRect &rect,
									  const wxColor &penColor, 
									  const wxColor &bitmapBorderUpperLeftPen)
{
	// Keep old pen and brush
	wxDCSaver dcsaver( &dc );

	// lower right size
	dc.SetPen(wxPen(penColor));
	dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width, rect.y + rect.height - 1);
	dc.DrawLine(rect.x + rect.width - 1, rect.y, rect.x + rect.width - 1, rect.y + rect.height);
	
	// upper left side
	dc.SetPen(wxPen(bitmapBorderUpperLeftPen));
	dc.DrawLine(rect.x, rect.y, rect.x + rect.width, rect.y);
	dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);
}

//--------------------------------------------------------------------------------------------
// XP renderer
//--------------------------------------------------------------------------------------------

void wxFMRendererXP::DrawButton(wxDC& dc, 
								const wxRect& rect, 
								const ControlState &state, 
								const bool WXUNUSED(useLightColours))
{
	// Set colors according to state
	wxColor penColor, brushColor;

	// switch according to the status
	switch(state)
	{
	case ControlFocus:
		penColor = wxArtManagerST::Get()->FrameColour();
		brushColor = wxArtManagerST::Get()->BackgroundColor();
		break;
	case ControlPressed:
		penColor = wxArtManagerST::Get()->FrameColour();
		brushColor = wxArtManagerST::Get()->HighlightBackgroundColor();
		break;
	case ControlNormal:
	case ControlDisabled:
	default:
		penColor = wxArtManagerST::Get()->FrameColour();;
		brushColor = wxArtManagerST::Get()->BackgroundColor();
		break;
	}

	// Draw the button borders
	DrawButtonBorders(dc, rect, penColor, brushColor);
}

void wxFMRendererXP::DrawButton(wxDC& dc, 
										  const wxRect& rect, 
										  const ControlState &state, 
										  const wxColor &color)
{
		// Set colors according to state
	wxColor penColor, brushColor;

	// switch according to the status
	switch(state)
	{
	case ControlFocus:
		penColor = color;
		brushColor = wxArtManagerST::Get()->LightColour(color, 75);
		break;
	case ControlPressed:
		penColor = color;
		brushColor = wxArtManagerST::Get()->LightColour(color, 60);
		break;
	case ControlNormal:
	case ControlDisabled:
	default:
		penColor = color;
		brushColor = wxArtManagerST::Get()->LightColour(color, 75);
		break;
	}

	// Draw the button borders
	DrawButtonBorders(dc, rect, penColor, brushColor);
}

void wxFMRendererXP::DrawMenuBarBg(wxDC& dc, const wxRect& rect)
{
	// For office style, we simple draw a rectangle with a gradient colouring
	bool vertical = wxArtManagerST::Get()->GetMBVerticalGardient();

	wxDCSaver dcsaver( &dc );

	// fill with gradient
	wxColour startColor = wxArtManagerST::Get()->GetMenuBarFaceColour();
	if( wxArtManagerST::Get()->IsDark(startColor) ) {	
		startColor = wxArtManagerST::Get()->LightColour(startColor, 50);
	}
	
	wxColour endColor   = wxArtManagerST::Get()->LightColour(startColor, 90);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, rect, startColor, endColor, vertical);

	// Draw the border
	if( wxArtManagerST::Get()->GetMenuBarBorder() )
	{
		dc.SetPen( startColor );
		dc.SetBrush( *wxTRANSPARENT_BRUSH );
		dc.DrawRectangle( rect );
	}
}


void wxFMRendererXP::DrawToolBarBg(wxDC& dc, const wxRect& rect)
{
	if(!wxArtManagerST::Get()->GetRaiseToolbar())
		return;

	// For office style, we simple draw a rectangle with a gradient colouring
	bool vertical = wxArtManagerST::Get()->GetMBVerticalGardient();

	wxDCSaver dcsaver( &dc );

	// fill with gradient
	wxColour startColor = wxArtManagerST::Get()->GetMenuBarFaceColour();
	if( wxArtManagerST::Get()->IsDark(startColor) ) {	
		startColor = wxArtManagerST::Get()->LightColour(startColor, 50);
	}
	startColor = wxArtManagerST::Get()->LightColour(startColor, 20);

	wxColour endColor   = wxArtManagerST::Get()->LightColour(startColor, 90);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, rect, startColor, endColor, vertical);
	wxArtManagerST::Get()->DrawBitmapShadow(dc, rect);
}

//--------------------------------------------------------------------------------------------
// vista buttons style
//--------------------------------------------------------------------------------------------

void wxFMRendererMSOffice2007::GetColorsAccordingToState(const ControlState &state,
														 int &upperBoxTopPercent,
														 int &upperBoxBottomPercent,
														 bool& concaveUpperBox,
														 int &lowerBoxTopPercent,
														 int &lowerBoxBottomPercent,
														 bool& concaveLowerBox)
{
	// switch according to the status
	switch(state)
	{
	case ControlFocus:
		upperBoxTopPercent = 95;
		upperBoxBottomPercent = 50;
		lowerBoxTopPercent = 40;
		lowerBoxBottomPercent = 90;
		concaveUpperBox = true;
		concaveLowerBox = true;
		break;
	case ControlPressed:
		upperBoxTopPercent = 75;
		upperBoxBottomPercent = 90;
		lowerBoxTopPercent = 90;
		lowerBoxBottomPercent = 40;
		concaveUpperBox = true;
		concaveLowerBox = true;
		break;
	case ControlDisabled:
		upperBoxTopPercent = 100;
		upperBoxBottomPercent = 100;
		lowerBoxTopPercent = 70;
		lowerBoxBottomPercent = 70;
		concaveUpperBox = true;
		concaveLowerBox = true;
		break;
	case ControlNormal:
	default:
		upperBoxTopPercent = 90;
		upperBoxBottomPercent = 50;
		lowerBoxTopPercent = 30;
		lowerBoxBottomPercent = 75;
		concaveUpperBox = true;
		concaveLowerBox = true;
		break;
	}
}
void wxFMRendererMSOffice2007::DrawButton(wxDC& dc, 
										  const wxRect& rect, 
										  const ControlState &state, 
										  const bool useLightColours)
{
	DrawButton(dc, rect, state, wxArtManagerST::Get()->GetThemeBaseColour(useLightColours));
}

void wxFMRendererMSOffice2007::DrawButton(wxDC& dc, 
										  const wxRect& rect, 
										  const ControlState &state, 
										  const wxColor &color)
{
	// Keep old pen and brush
	wxDCSaver dcsaver( &dc );
	
	// Define the rounded rectangle base on the given rect
	// we need an array of 9 points for it
	wxPoint regPts[9];
	wxColour baseColour = color;

	// Define the middle points
	wxPoint leftPt = wxPoint(rect.x, rect.y + (rect.height / 2));
	wxPoint rightPt = wxPoint(rect.x + rect.width - 1, rect.y + (rect.height / 2));

	// Define the top region
	wxRect top(rect.GetLeftTop(), rightPt);
	wxRect bottom(leftPt, rect.GetBottomRight());

	int upperBoxTopPercent, upperBoxBottomPercent, lowerBoxTopPercent, lowerBoxBottomPercent;
	bool concaveUpperBox, concaveLowerBox;
	GetColorsAccordingToState(state, upperBoxTopPercent, upperBoxBottomPercent, concaveUpperBox, lowerBoxTopPercent, lowerBoxBottomPercent, concaveLowerBox);

	wxColor topStartColor   = wxArtManagerST::Get()->LightColour(baseColour, upperBoxTopPercent);
	wxColor topEndColor = wxArtManagerST::Get()->LightColour(baseColour, upperBoxBottomPercent);
	wxColor bottomStartColor = wxArtManagerST::Get()->LightColour(baseColour, lowerBoxTopPercent);
	wxColor bottomEndColor   = wxArtManagerST::Get()->LightColour(baseColour, lowerBoxBottomPercent);
	
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, top, topStartColor, topEndColor);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);

	wxRect rr( rect );
	dc.SetBrush( *wxTRANSPARENT_BRUSH );

	wxColor frameColor = wxArtManagerST::Get()->LightColour(baseColour, 60);
	dc.SetPen( wxPen( frameColor ));
	dc.DrawRectangle( rr );

	wxColor wc = wxArtManagerST::Get()->LightColour(baseColour, 80);
	dc.SetPen( wxPen(wc) );
	rr.Deflate(1);
	dc.DrawRectangle( rr );
}

void wxFMRendererMSOffice2007::DrawMenuBarBg(wxDC& dc, const wxRect& rect)
{
	// Keep old pen and brush
	wxDCSaver dcsaver( &dc );
	wxColour baseColour = wxArtManagerST::Get()->GetMenuBarFaceColour();

	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) );
	dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) );
	dc.DrawRectangle(rect);

	// Define the rounded rectangle base on the given rect
	// we need an array of 9 points for it
	wxPoint regPts[9];
	int radius = 2;
	
	regPts[0] = wxPoint(rect.x, rect.y + radius);
	regPts[1] = wxPoint(rect.x+radius, rect.y);
	regPts[2] = wxPoint(rect.x+rect.width-radius-1, rect.y);
	regPts[3] = wxPoint(rect.x+rect.width-1, rect.y + radius);
	regPts[4] = wxPoint(rect.x+rect.width-1, rect.y + rect.height - radius - 1);
	regPts[5] = wxPoint(rect.x+rect.width-radius-1, rect.y + rect.height-1);
	regPts[6] = wxPoint(rect.x+radius, rect.y + rect.height-1);
	regPts[7] = wxPoint(rect.x, rect.y + rect.height - radius - 1);
	regPts[8] = regPts[0];

	// Define the middle points
	int factor = wxArtManagerST::Get()->GetMenuBgFactor();

	wxPoint leftPt1 = wxPoint(rect.x, rect.y + (rect.height / factor));
	wxPoint leftPt2 = wxPoint(rect.x, rect.y + (rect.height / factor)*(factor-1));

	wxPoint rightPt1 = wxPoint(rect.x + rect.width, rect.y + (rect.height / factor));
	wxPoint rightPt2 = wxPoint(rect.x + rect.width, rect.y + (rect.height / factor)*(factor-1));

	// Define the top region
	wxPoint topReg[7];
	topReg[0] = regPts[0];
	topReg[1] = regPts[1];
	topReg[2] = wxPoint(regPts[2].x+1, regPts[2].y);
	topReg[3] = wxPoint(regPts[3].x + 1, regPts[3].y);
	topReg[4] = wxPoint(rightPt1.x, rightPt1.y+1);
	topReg[5] = wxPoint(leftPt1.x, leftPt1.y+1);
	topReg[6] = topReg[0];

	// Define the middle region
	wxRect middle(leftPt1, wxPoint(rightPt2.x - 2, rightPt2.y));
		
	// Define the bottom region
	wxRect bottom(leftPt2, wxPoint(rect.GetBottomRight().x - 1, rect.GetBottomRight().y));

	wxColor topStartColor   = wxArtManagerST::Get()->LightColour(baseColour, 90);
	wxColor topEndColor = wxArtManagerST::Get()->LightColour(baseColour, 60);
	wxColor bottomStartColor = wxArtManagerST::Get()->LightColour(baseColour, 40);
	wxColor bottomEndColor   = wxArtManagerST::Get()->LightColour(baseColour, 20);
	
	wxRegion topRegion(7, topReg);

	wxArtManagerST::Get()->PaintGradientRegion(dc, topRegion, topStartColor, topEndColor);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, middle, topEndColor, bottomStartColor);
}

void wxFMRendererMSOffice2007::DrawToolBarBg(wxDC& dc, const wxRect& rect)
{
	if(!wxArtManagerST::Get()->GetRaiseToolbar())
		return;

	// Keep old pen and brush
	wxDCSaver dcsaver( &dc );
	wxColour baseColour = wxArtManagerST::Get()->GetMenuBarFaceColour();
	baseColour = wxArtManagerST::Get()->LightColour(baseColour, 20);

	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) );
	dc.SetPen( wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) );
	dc.DrawRectangle(rect);

	// Define the rounded rectangle base on the given rect
	// we need an array of 9 points for it
	wxPoint regPts[9];
	int radius = 2;
	
	regPts[0] = wxPoint(rect.x, rect.y + radius);
	regPts[1] = wxPoint(rect.x+radius, rect.y);
	regPts[2] = wxPoint(rect.x+rect.width-radius-1, rect.y);
	regPts[3] = wxPoint(rect.x+rect.width-1, rect.y + radius);
	regPts[4] = wxPoint(rect.x+rect.width-1, rect.y + rect.height - radius - 1);
	regPts[5] = wxPoint(rect.x+rect.width-radius-1, rect.y + rect.height-1);
	regPts[6] = wxPoint(rect.x+radius, rect.y + rect.height-1);
	regPts[7] = wxPoint(rect.x, rect.y + rect.height - radius - 1);
	regPts[8] = regPts[0];

	// Define the middle points
	int factor = wxArtManagerST::Get()->GetMenuBgFactor();

	wxPoint leftPt1 = wxPoint(rect.x, rect.y + (rect.height / factor));
	wxPoint rightPt1 = wxPoint(rect.x + rect.width, rect.y + (rect.height / factor));
	
	wxPoint leftPt2 = wxPoint(rect.x, rect.y + (rect.height / factor)*(factor-1));
	wxPoint rightPt2 = wxPoint(rect.x + rect.width, rect.y + (rect.height / factor)*(factor-1));

	// Define the top region
	wxPoint topReg[7];
	topReg[0] = regPts[0];
	topReg[1] = regPts[1];
	topReg[2] = wxPoint(regPts[2].x+1, regPts[2].y);
	topReg[3] = wxPoint(regPts[3].x + 1, regPts[3].y);
	topReg[4] = wxPoint(rightPt1.x, rightPt1.y+1);
	topReg[5] = wxPoint(leftPt1.x, leftPt1.y+1);
	topReg[6] = topReg[0];

	// Define the middle region
	wxRect middle(leftPt1, wxPoint(rightPt2.x - 2, rightPt2.y));
		
	// Define the bottom region
	wxRect bottom(leftPt2, wxPoint(rect.GetBottomRight().x - 1, rect.GetBottomRight().y));

	wxColor topStartColor   = wxArtManagerST::Get()->LightColour(baseColour, 90);
	wxColor topEndColor = wxArtManagerST::Get()->LightColour(baseColour, 60);
	wxColor bottomStartColor = wxArtManagerST::Get()->LightColour(baseColour, 40);
	wxColor bottomEndColor   = wxArtManagerST::Get()->LightColour(baseColour, 20);
	
	wxRegion topRegion(7, topReg);

	wxArtManagerST::Get()->PaintGradientRegion(dc, topRegion, topStartColor, topEndColor);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, bottom, bottomStartColor, bottomEndColor);
	wxArtManagerST::Get()->PaintStraightGradientBox(dc, middle, topEndColor, bottomStartColor);
	wxArtManagerST::Get()->DrawBitmapShadow(dc, rect);
}
