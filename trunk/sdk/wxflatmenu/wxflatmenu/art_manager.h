///////////////////////////////////////////////////////////////////////////////
// Name:		art_manager.h
// Purpose:     Art provider auxilary class
// Author:      <eranif@bezeqint.net>
// Created:     15/10/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/config.h>
#include "fm_resources.h"
#include "fm_renderer.h"
#include "fm_singleton.h"
#include "fm_smart_ptr.h"
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <map>

enum wxControlTheme
{
	StyleXP,
	Style2007
};

class wxFMRendererBase;

WX_DECLARE_HASH_MAP(int, wxFMRendererBase*, wxIntegerHash, wxIntegerEqual, wxRenderersMap);

enum ShadowStyle
{
	RightShadow				= 1,	// Right side shadow
	BottomShadow			= 2,	// Not full bottom shadow
	BottomShadowFull		= 4		// Full bottom shadow
};

/**
 * \ingroup wxflatmenu
 * a wrapper around the wxMemoryDC. On some platforms (e.g. MAC), memory dc
 * cant do nothing before being initialised with a bitmap, this object construct wxMemoryDC 
 * and provide it with a dummy 1x1 bitmap
 *
 * \version 1.0
 * first version
 *
 * \date 10-27-2006
 * \author Eran
 *
 */
class wxMemoryDcEx : public wxMemoryDC
{
public:
	/**
	 * Construct memory dc ex and provide it with a dummy bitmap
	 */
	wxMemoryDcEx() : wxMemoryDC() {
		wxBitmap bmp(1, 1);
		SelectObject( bmp );
	}

	/**
	 * Destructor
	 */
	virtual ~wxMemoryDcEx(){}
};

typedef fmSmartPtr<wxBitmap> wxBitmapPtr;

/**
 * \ingroup wxflatcontrols
 * \brief This class provides various art utilities, such as creating shadow, providing lighter / darker 
 * colors for a given color
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 *
 * \version 1.0
 * first version
 *
 * \date 02-28-2006
 *
 * \author wxWindows
 *
 *
 */
class wxArtManager : public wxEvtHandler
{
	// members
	wxRenderersMap m_renderers;
	wxConfigBase* m_config;
	int m_alignmentBuffer;
	wxControlTheme m_menuTheme;
	bool m_ms2007sunken;
	bool m_verticalGradient;
	wxBitmap m_rightTop;
	wxBitmap m_right;
	wxBitmap m_rightBottomCorner;
	wxBitmap m_bottom;
	wxBitmap m_bottomLeft;
	bool m_drowMBBorder;
	int m_menuBgFactor;
	wxString m_menuBarColourScheme;
	wxColour m_menuBarBgColour;
	wxColour m_menuBarSelColour;
	std::map<wxString, wxColour> m_colorSchemeMap;
	bool m_raiseTB;
	

	friend class fmSingleton<wxArtManager>;
	std::map<wxString, wxBitmapPtr> m_bitmaps;

private: 
	/**
	 * Initialise the colour map
	 */
	void InitColours();

	// methods
	/**
	 * \brief Default constructor
	 */
	wxArtManager(void);

	/**
	 * \brief Destrucotor
	 */
	virtual ~wxArtManager(void);

	/**
	 * \brief Return the top left x & y cordinates of the bitmap drawing 
	 *
	 * \param dc device context to use
	 * \param rect the rectangle that the caclulation are being performed on
	 * \param startLocationX output x location
	 * \param startLocationY output y location
	 * \param bitmap the required bitmap
	 * \param text the required text 
	 * \param style the control style
	 */
	void GetBitmapStartLocation(wxDC &dc, 
		const wxRect &rect, 
		int &startLocationX, 
		int &startLocationY,
		const wxBitmap &bitmap,
		const wxString &text = wxEmptyString,
		const long &style = 0);

	/**
	 * \brief Return the top left x & y cordinates of the text drawing.
	 * \ incase the text is too long, the text is being fixed (the text is cut and 
	 * \ a '...' mark is added in the end)
	 *
	 * \param dc device context to use
	 * \param rect the rectangle that the caclulation are being performed on
	 * \param fixedText fixed text buffer incase the text is too long for the input 
	 * \	rectangle & the current font
	 * \param startLocationX output x location
	 * \param startLocationY output y location
	 * \param bitmap the required bitmap
	 * \param text the required text 
	 * \param style the control style
	 */
	void GetTextStartLocation(wxDC &dc, 
		const wxRect &rect, 
		wxString &fixedText, 
		int &startLocationX, 
		int &startLocationY,
		const wxBitmap &bitmap,
		const wxString &text,
		const long &style = 0);

	void DarkPixel(unsigned char **newimage, const wxImage& img, int x, int y, int amount);

	void FillStokBitmaps();
public:
	/**
	 * \brief Return light contrast of color. The color returned is from the scale of 
	 * color -> white. the precent determines the how light the color will be. percent = 100 return white
	 * percent = 0 returns color.
	 *
	 * \param color starting color
	 * \param percent lightning factor
	 * \return lighter color
	 */
	wxColor LightColour(const wxColour& color, int percent) const ;

	/**
	 * \brief like the LightColour() function, but create the color more dark by percent
	 * \param color start color
	 * \param percent darking factor
	 * \return darker color
	 */
	wxColor DarkColour(const wxColour& color, int percent) const;

	/**
	 * \brief Return the surrounding color for a control 
	 * \return the frame color
	 */
	wxColor FrameColour() const;

	/**
	 * \brief The background color of a control when not in focus
	 * \return background color
	 */
	wxColor BackgroundColor() const;

	/**
 	 * \brief The background color of a control when in focus
	 * \return background color when control is highlighted
	 */
	wxColor HighlightBackgroundColor() const;

	/**
 	 * \brief Mix The background color of a control when in focus
	 * \return result of mix color
	 */
	/**
	 * \brief Return mix of input colors. 
	 *
	 * \param firstColor an input color
	 * \param secondColor an input color
	 * \param percent a number in a scale of 0-100 that represent the weight of each color. 
	 * \	0 = 100% of first color and 0% of second color
	 * \	50 = 50% of first color and 50% of second color
	 * \	100 = 0% of first color and 100% of second color
	 * \return mix color
	 */
	wxColor MixColors(const wxColour& firstColor, const wxColour& secondColor, int percent) const ;

	/**
	 * \brief Paint rectagnle with gradient coloring
	 * \the gradient lines are either horizontal or vertial
	 * \param dc device context to use
	 * \param rect bounding rectangle
	 * \param startColor starting color of the gradient
	 * \param endColor end color of the gradient
	 * \param vertical vertical drawings or horizontal
	 */
	void PaintStraightGradientBox(wxDC& dc,
		const wxRect& rect,
		const wxColour& startColor,
		const wxColour& endColor,
		bool  vertical = true);

	/**
	 * \brief Paint rectagnle with gradient coloring
	 * \the gradient lines are diagonal and may start from the 
	 * \upper left corner or from the upper right corner
	 * \
	 * \param dc device context to use
	 * \param rect bounding rectangle
	 * \param startColor starting color of the gradient
	 * \param endColor end color of the gradient
	 * \param startAtUpperLeft start from the upper left corner 
	 * \		or from the upper right corner
	 * \param trimToSquare trim the input rect into a square. the square side is 
	 * \		calculated according to the smaller dimention
	 */
	void PaintDiagonalGradientBox(wxDC &dc, 
		const wxRect &rect, 
		const wxColor &startColor,
		const wxColor &endColor,
		bool startAtUpperLeft = true,
		bool trimToSquare = true);

	/**
	 * \brief Paint a region with gradient coloring
	 * \param dc device context to use
	 * \param region bounding region
	 * \param startColor starting color of the gradient
	 * \param endColor end color of the gradient
	 * \param vertical vertical drawings or horizontal
	 */
	void PaintGradientRegion(wxDC& dc,
		const wxRegion& region,
		const wxColour& startColor,
		const wxColour& endColor,
		bool  vertical = true);

	/**
	 * \brief Paint a region with gradient coloring
	 * \	the gradient is in crescent shape which fits the 2007 style
	 * \
	 * \param dc device context to use
	 * \param rect bounding rectangle
	 * \param startColor starting color of the gradient (top middle point in the rectangle)
	 * \param endColor end color of the gradient (bottom middle point in the rectangle)
	 * \param concave indicate if the crescent shape is concave or convex
	 */
	void PaintCrescentGradientBox(wxDC& dc,
		const wxRect& rect,
		const wxColour& startColor,
		const wxColour& endColor,
		const bool& concave = true);

	/**
	 * \brief Generate random color
	 * \return Random color 
	 */
	wxColor RandomColor() const;


	/**
	 * \brief is the color is dark or light
	 */
	bool IsDark(const wxColor &color) const;

	/**
	 * \brief truncate given string to fit given width size. if the text does not
	 *  fit into the given width it is truncated to fit. the format of the fixed text is
	 *  <truncate text ..>
	 * \param dc device context to use for measuring the text length
	 * \param text input text to adjust
	 * \param maxWidth maximum possible text length
	 * \param fixedText the truncated text
	 */
	void TruncateText(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText);

	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param theme requested theme - StyleXP, Style2007, wx_FLAT,	wx_SEMI_FLAT
	 * \param useLightColours when set to false, the art manager will replace light colours with
	 * the default theme colour (Dark Orange)
	 */
	void DrawButton(wxDC& dc, 
		const wxRect &rect, 
		const wxControlTheme &theme, 
		const ControlState &state,
		const bool useLightColours = true);


	/**
	 * \brief Color rectangle according to the theme
	 * \param dc device context to use
	 * \param rect area to color
	 * \param theme requested theme - StyleXP, Style2007, wx_FLAT,	wx_SEMI_FLAT
	 * \param color base colour for button
	 */
	void DrawButton(wxDC& dc, 
		const wxRect &rect, 
		const wxControlTheme &theme, 
		const ControlState &state,
		const wxColor &color);

	/**
	 * \brief make a window transparent (Windows Only)
	 * \param wnd window to make transparent
	 * \param amount value, where 0 - full transpareny, 255 fully visible
	 */
	void MakeWindowTransparent(wxWindow* wnd, int amount);

	/**
	 * \brief adds a shadow under the window (Windows Only)
	 * \param wnd window to add shadow
	 * \paran drop drop or hide shadow
	 */
	void DropShadow(wxWindow* wnd, bool drop = true) ;

	/**
	 * \brief draw the text & bitmap on the input dc
	 *
	 * \param dc device context to use
	 * \param rect the rectangle that the caclulation are being performed on
	 * \param text the required text 
	 * \param enable true is the control is enabled, false otherwise
	 * \param font the required font - if wxNullFont is set the default font will be used
	 * \param fontColor the required fort color
	 * \param bitmap the required bitmap
	 * \param grayBitmap the required gray bitmap (for disable state)
	 * \	Note: all location calculation are performed on the standard bitmap
	 * \	it is expected that both bitmaps are in the same size
	 * \param style the control style
	 */
	void DrawTextAndBitmap(wxDC &dc, 
		const wxRect &rect,
		const wxString &text,
		const bool &enable = true,
		const wxFont &font = wxNullFont,
		const wxColor &fontColor = wxT("BLACK"),
		const wxBitmap &bitmap = wxNullBitmap,
		const wxBitmap &grayBitmap = wxNullBitmap,
		const long &style = 0);


	/**
	 * \brief return the padding buffer for a text or bitmap
	 */
	int GetAlignBuffer() const { return m_alignmentBuffer; }

	/**
	 * \brief return the best fit size for the supplied label & bitmap
	 * \param label the label that is contained in the return area
	 * \param bmp the bitmap that is contained in the return area
	 */
	wxSize CalcButtonBestSize(const wxString& label, const wxBitmap& bmp) const;

	/**
	 * \brief return the colour used for menu face
	 * \return menu face colour
	 */
	wxColour GetMenuFaceColour() const;

	/**
	 * \brief return the menomonic index of the label (e.g. 'lab&el' --> will result in 3 and labelOnly = label)
	 * \param label label
	 * \param labelOnly string for the stripped label
	 */
	int GetAccelIndex(const wxString& label, wxString &labelOnly);

	/**
	 * \brief set the menu theme, possible values (Style2007, StyleXP)	
	 * \param theme theme to use 
	 */
	void SetMenuTheme(wxControlTheme theme) { m_menuTheme = theme; }

	/**
	 * \brief return the menu theme
	 * \return menu theme
	 */
	wxControlTheme GetMenuTheme() const { return m_menuTheme; }

	/**
	 * \brief return the theme (Blue, Silver, Green etc.) base color, if no theme is active
	 * it return the active caption colour, lighter in 30 %
	 * \param useLightColours set to true so the ArtManager will convert light colours to default Orange Theme
	 * \return theme based colour
	 */
	wxColour GetThemeBaseColour(const bool useLightColours = true) const;

	/**
	 * \brief set ms 2007 button style sunken
	 * \param sunken
	 */
	void SetMS2007ButtonSunken(bool sunken) { m_ms2007sunken = sunken; }

	/**
	 * \brief return the sunken flag for ms 2007 buttons
	 * \return ms style 2007 sunken style
	 */
	bool GetMS2007ButtonSunken() const { return m_ms2007sunken; }

	/**
	 * \brief draw resize sash
	 * \param rect sash rectangle
	 */
	void DrawDragSash(const wxRect& rect);

	/**
	 * Return true if the meun bar should be painted with vertical gradient 
	 * false otherwise
	 */
	bool GetMBVerticalGardient() const { return m_verticalGradient; }

	/**
	 * Set the menu bar gradient style
	 */
	void SetMBVerticalGradient(bool v) { m_verticalGradient = v; }

	/**
	 * \brief draw shadow using background bitmap
	 * \param dc device context to use
	 * \param labelRect label rectangle. The shadow will be around this rectangle
	 * \param type sahdow position
	 */
	void DrawBitmapShadow(wxDC& dc, const wxRect& labelRect, int where = BottomShadow | RightShadow );

	/**
	 * Take a screenshot of the screen at give pos & size (rect)
	 * \param rect area and position of the screen to capture
	 * \param bmp [output] the screenshot
	 */
	void TakeScreenShot(const wxRect& rect, wxBitmap &bmp);

	/**
	 * Draw the menu bar background according to the active theme
	 * \param dc device context
	 * \param rect menu bar rectangle
	 */
	void DrawMenuBarBg(wxDC& dc, const wxRect &rect);

	/**
	 * Draw the tool bar background according to the active theme
	 * \param dc device context
	 * \param rect menu bar rectangle
	 */
	void DrawToolBarBg(wxDC& dc, const wxRect &rect);

	/**
	 * Enable menu border drawing (XP style only)
	 * \param border 
	 */
	void DrawMenuBarBorder(bool border) { m_drowMBBorder = border; }

	/**
	 * Return menu bar morder drawing flag
	 * \return true if it is enabled
	 */
	bool GetMenuBarBorder() const { return m_drowMBBorder; }

	/** 
	 * Get the visibility depth of the menu in Metalic style
	 * the higher the value, the menu bar will look more raised
	 */
	int GetMenuBgFactor() const { return m_menuBgFactor; }

	/** 
	 * Set the visibility depth of the menu in Metalic style
	 * the higher the value, the menu bar will look more raised
	 * \return factor 
	 */
	void SetMenuBgFactor(int f) { m_menuBgFactor = f; }

	/**
	 * Set the menu bar color scheme to use
	 * \param scheme to use
	 */
	void SetMenuBarColour(const wxString &scheme);

	/**
	 * Return the menu bar color scheme
	 * \return menu bar colour scheme
	 */
	wxString GetMenuBarColourScheme() const;

	/**
	 * Return the menu bar face colour
	 * \return face color
	 */
	wxColour GetMenuBarFaceColour() const;

	/**
	 * Return menu bar selection color
	 * \return selection color
	 */
	wxColour GetMenuBarSelectionColour() const;

	/**
	 * Create grey bitmap image from bmp
	 * \param bmp source bitmap
	 * \return greyed bitmap
	 */
	wxBitmap CreateGreyBitmap(wxBitmap bmp) const;

	/**
	 * Load an image from resource file
	 */
	void ConvertToBitmap(unsigned char *data, const size_t size, wxBitmap &bmp);

	/**
	 * Do Drop shadow under toolbar?
	 */
	bool GetRaiseToolbar() const { return m_raiseTB; }

	/**
	 * Enable/Disable toobar shadow drop
	 */
	void SetRaiseToolbar(bool raise) { m_raiseTB = raise; }

	/**
	 * Get a bitmap from a stock, if bitmap does not exist, return wxNullBitmap
	 * \param name bitmap name
	 * \return bitmap or wxNullBitmap
	 */
	wxBitmapPtr GetStokBitmap(const wxString &name) const;

protected:
	void OnSysColourChange(wxSysColourChangedEvent &event);
};

typedef fmSingleton<wxArtManager> wxArtManagerST;

/**
 * \ingroup wxflatcontrols
 * \brief a utility class that helps restore a DC when entring scope
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 *
 * \version 1.0
 * first version
 *
 * \date 03-27-2006
 *
 *
 */
class wxDCSaver
{
	wxDC* m_pdc;
	wxPen m_pen;
	wxBrush m_brush;
	
public:
	/**
	 * \brief construct a DC saver. The pointer to the dc is copied as-is.
	 * \param pdc pointer to dc 
	 * \return 
	 */
	wxDCSaver(wxDC* pdc) 
	: m_pdc( pdc )
	, m_pen( pdc->GetPen() )
	, m_brush( pdc->GetBrush() )
	{};


	/**
	 * \brief destructor. While destructing, restores the dc pen and brush
	 */
	~wxDCSaver()
	{
		if( m_pdc )
		{
			m_pdc->SetPen( m_pen );
			m_pdc->SetBrush( m_brush );
		}
		m_pdc = NULL;
	}
};
