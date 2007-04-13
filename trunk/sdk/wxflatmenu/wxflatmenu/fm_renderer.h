///////////////////////////////////////////////////////////////////////////////
// Name:		fm_renderer.h
// Purpose:     declare the flat menu renderer classes
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef __WXES_RENDERER_H__
#define __WXES_RENDERER_H__

#include <wx/dc.h>

enum wxButtonStyle
{
	wxBU_EXT_XP_STYLE =				0x0000001,
	wxBU_EXT_2007_STYLE =			0x0000002,
	wxBU_EXT_LEFT_ALIGN_STYLE =		0x0000004,
	wxBU_EXT_CENTER_ALIGN_STYLE =	0x0000008,
	wxBU_EXT_RIGHT_ALIGN_STYLE =	0x0000010,		
	wxBU_EXT_RIGHT_TO_LEFT_STYLE =	0x0000020
};

enum ControlState
{
	ControlPressed,
	ControlFocus,
	ControlDisabled,
	ControlNormal
};

enum ControlStatus
{
	ControlStatusNoFocus,
	ControlStatusFocus,
	ControlStatusPressed
};
/**
 * \ingroup wxflatcontrols
 *
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 *
 * \version 1.0
 * first version
 *
 * \date 04-02-2006
 * \brief Base class for all theme renderers 
 *
 */
class wxFMRendererBase
{
public:
	wxFMRendererBase() {};
	virtual  ~wxFMRendererBase() {};

	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 * \param theme requested theme - StyleXP, Style2007
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const bool useLightColours = true) = 0;

	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 * \param theme requested theme - StyleXP, Style2007
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const wxColor &color) = 0;

	/**
	 * Draw the menu bar background according to the active theme
	 * \param dc device context
	 * \param rect menu bar rectangle
	 */
	virtual void DrawMenuBarBg(wxDC& dc, const wxRect& rect) = 0;

	/**
	 * Draw the tool bar background according to the active theme
	 * \param dc device context
	 * \param rect tool bar rectangle
	 */
	virtual void DrawToolBarBg(wxDC& dc, const wxRect& rect) = 0;

protected:
	virtual void DrawButtonBorders(wxDC &dc, 
		const wxRect &rect, 
		const wxColor &penColor, 
		const wxColor &brushColor);

	virtual void DrawBitmapArea(wxDC &dc, 
		char *xpm_name[], 
		const wxRect &rect,
		const wxColor &baseColor, 
		const bool &flipSide = false);

	virtual void DrawBitmapBorders(wxDC &dc, 
		const wxRect &rect,
		const wxColor &penColor, 
		const wxColor &bitmapBorderUpperLeftPen);
};

//------------------------------------------------------------------------
// XP renderer
//------------------------------------------------------------------------

class wxFMRendererXP : public wxFMRendererBase
{
public:
	wxFMRendererXP() {};
	virtual  ~wxFMRendererXP() {};

	/**
	 * \brief Color rectangle according to the XP theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 * \param backgroundColor the background color
	 * \param style the control style
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const bool useLightColours = true);

	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 * \param theme requested theme - StyleXP, Style2007
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const wxColor &color);

	/**
	 * Draw the menu bar background according to the active theme
	 * \param dc device context
	 * \param rect menu bar rectangle
	 */
	virtual void DrawMenuBarBg(wxDC& dc, const wxRect& rect);

	/**
	 * Draw the tool bar background according to the active theme
	 * \param dc device context
	 * \param rect tool bar rectangle
	 */
	virtual void DrawToolBarBg(wxDC& dc, const wxRect& rect);
};

//------------------------------------------------------------------------
// Office 2007 renderer
//------------------------------------------------------------------------

class wxFMRendererMSOffice2007 : public wxFMRendererBase
{
public:
	wxFMRendererMSOffice2007() {};
	virtual  ~wxFMRendererMSOffice2007() {};

	/**
	 * \brief Color rectangle according to the Office 2007 theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const bool useLightColours = true);

	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param state the control state. One of: pressed, focus, disable, normal
	 * \param theme requested theme - StyleXP, Style2007
	 */
	virtual void DrawButton(wxDC& dc, 
		const wxRect& rect, 
		const ControlState &state, 
		const wxColor &color);

	/**
	 * Draw the menu bar background according to the active theme
	 * \param dc device context
	 * \param rect menu bar rectangle
	 */
	virtual void DrawMenuBarBg(wxDC& dc, const wxRect& rect);

	/**
	 * Draw the tool bar background according to the active theme
	 * \param dc device context
	 * \param rect tool bar rectangle
	 */
	virtual void DrawToolBarBg(wxDC& dc, const wxRect& rect);

protected:
	virtual void GetColorsAccordingToState(const ControlState &state,
		int &upperBoxTopPercent,
		int &upperBoxBottomPercent,
		bool& concaveUpperBox,
		int &lowerBoxTopPercent,
		int &lowerBoxBottomPercent,
		bool& concaveLowerBox);
};
#endif
