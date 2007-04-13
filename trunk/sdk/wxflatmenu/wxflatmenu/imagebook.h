///////////////////////////////////////////////////////////////////////////////
// Name:		imagebook.h
// Purpose:     Generic declaration of flat image notebook class.
// Author:      eranif@bezeqint.net
// Created:     30/12/2005
// Modified:    01/01/2006
// Copyright:   Eran Ifrah (c)
// Licence:     wxWidgets license
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <wx/statline.h>
#include <map>

typedef std::vector<wxBitmap> wxFlatImageBookImageList;
class wxImageContainerBase;

/// Place images to the bottom of the 
/// book
#define wxINB_BOTTOM				0x00000001
#define wxINB_LEFT					0x00000002
#define wxINB_RIGHT					0x00000004
#define wxINB_TOP					0x00000008
#define wxINB_BORDER				0x00000010
#define wxINB_SHOW_ONLY_TEXT		0x00000020
#define wxINB_SHOW_ONLY_IMAGES		0x00000040
#define wxINB_FIT_BUTTON			0x00000080
#define wxINB_DRAW_SHADOW			0x00000100
#define wxINB_USE_PIN_BUTTON		0x00000200
#define wxINB_GRADIENT_BACKGROUND	0x00000400
#define wxINB_WEB_HILITE			0x00000800
#define wxINB_NO_RESIZE				0x00001000

#define wxINB_DEFAULT_STYLE wxINB_BORDER | wxINB_TOP | wxINB_USE_PIN_BUTTON

/**
* \ingroup wxcontrols
*
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* Base class for the wxXXXNotebook classes
*
* \date 02-18-2006
*
* \author Eran Ifrah
*
*/
class wxFlatBookBase :
	public wxPanel
{
public:
	/**
	* \brief Parametrized constructor
	* \param parent - parent window
	* \param id - window ID 
	* \param pos - window position
	* \param size - window size
	* \param style - window style
	* \param name - window class name
	* \return 
	*/
	wxFlatBookBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxINB_DEFAULT_STYLE, const wxString& name = wxT("FlatBookBase"));

	///Default constructor
	/**
	 * \brief default constructor
	 */
	wxFlatBookBase(){}

	/**
	 * \brief Destructor	
	 * \param void 
	 */
	virtual ~wxFlatBookBase(void) ;

	/// Appends new book page
	/**	
	* \param window - window to be appended
	* \param caption - tab caption
	* \param selected - determines if new page should be selected automatically
	* \param imgId - page image index
	*
	*/
	virtual void AddPage(wxWindow* window, const wxString& caption, bool selected, const int imgId = -1);


	/// Sets an image list associated with notebook pages
	/**	
	* \param imglist - image list object. 
	* Image list assigned with this method will be deleted by wxFlatNotebookBase's destructor.
	*/
	virtual void AssignImageList(wxFlatImageBookImageList * imglist);

	/**
	 * \brief Returns an image list object associated with wxFlatNotebookBase	
	 */
	virtual wxFlatImageBookImageList * GetImageList();

	/**
	 * \brief Return the selected page index (starting from zero)
	 * or -1 if there are no pages in the book
	 * \return 
	 */
	virtual int GetSelection();

	/**
	 * \brief Changes the selection from currently visible/selected page to the page given by index
	 * \param page - index of page to be selected
	 */
	virtual void SetSelection(size_t page);

	/**
	 * \brief Alters the notebook style 
	 * \param style - new value of notebook style
	 */
	virtual void SetWindowStyleFlag(long style);

	/** 
	 * Delete all page from the book
	 */
	virtual void DeleteAllPages();

protected:
	/**
	 * \brief Select the window by the provided pointer
	 * \param window pointer of the window to select
	 */
	void DoSetSelection(wxWindow* window);

protected:

	/// Members
	wxImageContainerBase* m_pages;
	wxBoxSizer *m_mainSizer;
	wxStaticLine *m_separator;

	/// vector of all the windows associated with the notebook pages.
	std::vector<wxWindow*> m_windows;

	// Loading flag
	bool m_bInitializing;
};

// HitTest results
enum
{
	wxIMG_OVER_IMG = 0,
	wxIMG_OVER_PIN, 
	wxIMG_OVER_EW_BORDER,
	wxIMG_NONE
};

// Pin button states
enum
{
	wxINB_PIN_NONE = 0,
	wxINB_PIN_HOVER = 200,
	wxINB_PIN_PRESSED = 201
};

/**
* \ingroup wxcontrols
* \brief this class holds information (such as caption, image index ..) about a window in 
* in the container
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* Contains parameters of notebook page
*
* \date 02-18-2006
*
*/
class wxImageInfo
{
public:
	// Members
	/// Page caption
	wxString m_strCaption;

	/// Page position
	wxPoint m_pos;

	/// Page size
	wxSize  m_size;

	/// Page image index
	int m_ImageIndex;		

	/// Rectangle surrounding the text
	wxRect m_captionRect;

public:

	/**
	 * \brief 	Default constructor
	 */
	wxImageInfo(): m_strCaption(wxEmptyString), m_ImageIndex(-1){};
	
	/**
	 * \brief Parametrized constructor
	 * \param caption page caption 
	 * \param imgindex image index
	 * \return 
	 */
	wxImageInfo(const wxString& caption, int imgindex) : 
	m_strCaption(caption), m_pos(-1, -1), m_size(-1, -1), m_ImageIndex(imgindex){}

	/**
	 * \brief Destructor
	 * \return 
	 */
	~wxImageInfo(){};

	/**
	 * \brief sets the page caption
	 * \param value new page caption
	 */
	void SetCaption(wxString value) {m_strCaption = value;}

	/**
	 * \brief return the page caption 
	 * \return 
	 */
	wxString GetCaption() {return m_strCaption;}

	/**
	 * \brief Sets page position
	 * \param value 
	 */
	void SetPosition(wxPoint value) {m_pos = value;}

	/**
	 * \brief Returns page position
	 * \return 
	 */
	const wxPoint & GetPosition() {return m_pos;}

	/**
	 * \brief Sets page size
	 * \param value - new page size
	 */
	void SetSize(wxSize value) {m_size = value;}

	/**
	 * \brief Returns page size
	 * \return Page Size
	 */
	const wxSize & GetSize() {return m_size;}

	/**
	 * \brief Sets page image index
	 * \param value - new image index
	 */
	void SetImageIndex(int value) {m_ImageIndex = value;}

	/**
	 * \brief Return this page image index
	 * \return the page image index
	 */
	int GetImageIndex() {return m_ImageIndex;}

	/**
	 * \brief Set the rectangle surrounding the text
	 * \param rect Rectangle surrounding the text
	 */
	void SetTextRect(const wxRect& rect) { m_captionRect = rect; }

	/**
	 * \brief return an approximate rectagnle that surrounding the 
	 *  text
	 * \return Rectangle surrounding the text
	 */
	const wxRect& GetTextRect() { return m_captionRect; }
};


/**
 * \ingroup wxcontrols
 * \brief base class for the image book container. Caller should usually create an instance
 * of one of the derived classes (wxFlatImageBook, wxLabelBook) 
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
 */
class wxImageContainerBase : public wxPanel
{
protected:
	friend class wxFlatBookBase;
	wxFlatImageBookImageList * m_ImageList;
	wxRect m_sashRect;

public:
	/**
	* \brief Parameterized constructor
	* \param parent - Pointer to parent
	* \param id - Window id of the praent
	* \param pos - Window position
	* \param size - Window size
	* \param style - possible style wxINB_XXX
	* \return 
	*/
	wxImageContainerBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("wxImageContainerBase"));

	/**
	 * \brief Destructor
	 */
	virtual ~wxImageContainerBase(void);

	/**
	 * \brief Sets an image list associated with notebook pages. The image list must be allocated
	 * on the heap. Once set, the container takes ownership over the image list.
	 * \param imglist - image list object.
	 */
	virtual void AssignImageList(wxFlatImageBookImageList * imglist);

	/**
	 * \brief Returns an image list object associated with wxFlatBookBase	
	 * \return 
	 */
	wxFlatImageBookImageList * GetImageList() { return m_ImageList; }

	/**
	 * \brief adds a page to the container
	 * \param caption page caption
	 * \param selected after adding this page, should it be selected
	 * \param imgIdx image index for this page (default is -1, meaning no image)
	 */
	virtual void AddPage(const wxString& caption, bool selected = false, const int imgIdx = -1);

	/**
	 *
	 * \brief the image size
	 */
	const int& GetImageSize() const { return m_nImgSize; }

	/**
	 * \brief Virtual functions to allow the parent to examine the children type. Some implementation (such as wxLabelBook), does 
	 * not support top/bottom images, only left/right
	 * \return 
	 */
	virtual bool CanDoBottomStyle() = 0;

	/**
	 * \brief Test for existance of flag in the style 
	 * \param flag style to test
	 * \return 
	 */
	virtual bool HasFlag(int flag);

	/**
	 * \brief Remove flag from the style
	 * \param flag flag to remove
	 */
	virtual void ClearFlag(int flag);

	/// Event handler functions
	virtual void OnPaint(wxPaintEvent& event) = 0; 
	virtual void OnSize(wxSizeEvent& WXUNUSED(event));
	virtual void OnMouseLeftDown(wxMouseEvent& event);
	virtual void OnEraseBackground(wxEraseEvent& WXUNUSED(event)) { }
	virtual void OnMouseLeaveWindow(wxMouseEvent& event);
	virtual void OnMouseLeftUp(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);

protected:
	/**
	 * \brief fix the text, to fit maxWidth value. If the text length exceeds maxWidth value
	 * this function truncates it and appends two dots at the end. ("Long Long Long Text" might 
	 * become "Long Long .."
	 * \param dc device context used for calculating the text width
	 * \param text text to fix (if needed)
	 * \param maxWidth maxmium width allowed for this text
	 * \param fixedText fixed text
	 */
	void FixTextSize(wxDC& dc, const wxString& text, const int &maxWidth, wxString& fixedText);
	
	/**
	 * \brief Returns the index of the tab at the specified position or 
	 *  wxNOT_FOUND if none. If flags parameter is non NULL, 
	 *  the position of the point inside the tab is returned as well.
	 * \param pt point to test
	 * \param imgIdx if pt is on an image, returns its index
	 * \return of the values wxIMG_OVER_IMG, wxIMG_OVER_PIN, wxIMG_NONE
	 */
	int  HitTest(const wxPoint& pt, int &imgIdx);

	/**
	 * \brief draw a pin button, that allows collapsing of the image panel
	 * \param dc device context
	 * \param rect pin rectangle
	 * \param downPin pin mode, downPin = true means draw pressed pin button
	 */
	void DrawPin(wxDC& dc, wxRect &rect, bool downPin);

	/**
	 * \brief test whethe pt is located on the sash
	 * \param pt point to test, in client coords
	 */
	bool PointOnSash(const wxPoint& pt);

	/**
	 * Clear all images
	 */
	void ClearAll();

	int m_nIndex;
	int m_nImgSize;
	int m_nHoeveredImgIdx;
	bool m_bCollapsed;
	wxSize  m_tabAreaSize;
	wxRect  m_pinBtnRect;
	int     m_nPinButtonStatus;
	std::vector<wxImageInfo> m_pagesInfoVec;
};

/**
* \ingroup wxcontrols
*
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* This class holds information about events associated with wxFlatNotebookBase objects
*
* \date 02-18-2006
*
*/
class wxImageNotebookEvent : public wxNotifyEvent 
{
	DECLARE_DYNAMIC_CLASS(wxImageNotebookEvent)
	size_t sel, oldsel;

public:
	/**
	 * \brief Constructor
	 * \param commandType event type
	 * \param winid window ID
	 * \param nSel current selection
	 * \param nOldSel old selection
	 */
	wxImageNotebookEvent(wxEventType commandType = wxEVT_NULL, int winid = 0, int nSel = -1, int nOldSel = -1)
		: wxNotifyEvent(commandType, winid), sel(nSel), oldsel(nOldSel)
	{}

	/**
	 * \brief Sets the value of current selection
	 * \param s index of currently selected page
	 */
	void SetSelection(int s) { sel = s; }

	/**
	 * \brief Sets the value of previous selection
	 * \param s index of previously selected page
	 */
	void SetOldSelection(int s) { oldsel = s; }

	/**
	 * \brief Returns the index of currently selected page
	 */
	int  GetSelection() { return (int)sel; }

	/**
	 * \brief Returns the index of previously selected page
	 */
	int  GetOldSelection() { return (int)oldsel; }
}; 

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Specific implementations for wxFlatImageBook
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* \ingroup wxcontrols
*
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* A helper class that used by wxFlatImageBook. This class is responsible for 
* drawing the labels of the wxFlatImageBook class and to act on events
*
* \date 02-18-2006
*
* \author Eran Ifrah
*
*/
class wxImageContainer : public wxImageContainerBase
{
public:

	/**
	 * \brief Parameterized constructor
	 * \param parent - Pointer to parent
	 * \param id - Window id of the praent
	 * \param pos - Window position
	 * \param size - Window size
	 * \param style - possible style wxINB_XXX
	 */
	wxImageContainer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0)
		: wxImageContainerBase(parent, id, pos, size, style, wxT("wxImageContainer")) {};

	/**
	 * \brief For this implementation, we can do bottom style
	 * \return true
	 */
	bool CanDoBottomStyle() { return true; }

	/**
	 * \brief Destructor
	 */
	virtual ~wxImageContainer(void) {};

	/// Implementation of the OnPaint() event handler
	DECLARE_EVENT_TABLE()
	void OnPaint(wxPaintEvent& event);
};

/**
 * \ingroup wxcontrols
 * \brief Default implelemntation of the image book, it is like 
 * a note book, except that images are used to control the different 
 * pages. This container is usually used for configuration dialogs etc.
 * Currently, this control works properly for image of 32x32 and bigger
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
 * \todo Support images of lower size such as 16x16
 *
 *
 */
class wxFlatImageBook : public wxFlatBookBase
{
public:
	/**
	 * \param parent - parent window
	 * \param id - window ID 
	 * \param pos - window position
	 * \param size - window size
	 * \param style - window style
	 * \param name - window class name
	 */
	wxFlatImageBook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxINB_DEFAULT_STYLE, const wxString& name = wxT("wxFlatImageBook"));

	/**
	 * \brief Destructor
	 */
	wxFlatImageBook() {}


	/**
	 * \brief Create the image container for the wxFlatImageBook class
	 * \return wxImageContainer
	 */
	virtual wxImageContainerBase* CreateImageContainer();

	DECLARE_DYNAMIC_CLASS(wxFlatImageBook)
	DECLARE_EVENT_TABLE()
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Specific implementations for wxLabelBook
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

enum 
{
	wxINB_TAB_AREA_BACKGROUND_COLOR = 100,
	wxINB_ACTIVE_TAB_COLOR,
	wxINB_TABS_BORDER_COLOR,
	wxINB_TEXT_COLOR,
	wxINB_ACTIVE_TEXT_COLOR,
	wxINB_HILITE_TAB_COLOR
};

#define wxINB_LABEL_BOOK_DEFAULT wxINB_DRAW_SHADOW | wxINB_BORDER | wxINB_USE_PIN_BUTTON | wxINB_LEFT

/**
* \ingroup wxcontrols
*
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* A helper class that used by wxLabelBook. This class is responsible for 
* drawing the labels of the wxLabelBook class and to act on events
*
* \date 02-18-2006
*
*/
class wxLabelContainer : public wxImageContainerBase
{
	int m_nTabAreaWidth;
	std::map<int, wxColor> m_colorsMap;
	wxBitmap m_skin;
	wxCursor m_oldCursor;

protected:
	/**
	 * \brief draw label using dc
	 * \param dc device context to use
	 * \param rect bounding rectangle for the label
	 * \param text label caption
	 * \param bmp bitmap to use 
	 * \param imgInfo (output) wxImageInfo to fill label information
	 * \param orientationLeft label is placed right or left
	 * \param selected this tab is selected
	 */
	void DrawLabel(wxDC& dc, 
					wxRect rect, 
					const wxString& text, 
					const wxBitmap& bmp,
					wxImageInfo& imgInfo,
					bool orientationLeft,
					int imgIdx,
					bool selected,
					bool hover);

	/**
	 * \brief draw bitmap as the background of the control (provided by SetBackgroundBitmap() function)
	 * if the bitmap is smaller than the client area, the image will be multiply 
	 * if the bitmap is larger than the client area, drawing starts from top left
	 * \param dc device context
	 */
	void DrawBackgroundBitmap(wxDC& dc);

	void Resize(wxMouseEvent& event);

public:

	/**
	 * \brief Parameterized constructor
	 * \param parent - Pointer to parent
	 * \param id - Window id of the praent
	 * \param pos - Window position
	 * \param size - Window size
	 * \param style - possible style wxINB_XXX
	 */
	wxLabelContainer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);

	/**
	 * \brief Destructor
	 */
	virtual ~wxLabelContainer(void) {};

	/**
	 * \brief Set at color parameter
	 * \param which - Which parameter to update 
	 * \param color - Color to use
	 */
	void SetColour(int which, const wxColor& color);

	/**
	 * \brief return the 'which' color
	 * \param which - Which color to return
	 * \return return the 'which' color
	 */
	wxColor GetColour(int which);

	/**
	 * \brief Return the tab area width
	 * \return 
	 */
	const int& GetTabAreaWidth() const { return m_nTabAreaWidth; }

	/**
	 * \brief Set the tab area width. This function does not cause of refresh the 
	 * the control
	 * \param width 
	 */
	void SetTabAreaWidth(const int& width) { m_nTabAreaWidth = width; }

	/**
	 * \brief Initialize the colors map to be used for this control
	 */
	void InitializeColors();

	/**
	 * \brief returns false, this implementation does not
	 * support bottom/top styles
	 * \return false
	 */
	bool CanDoBottomStyle() { return false; }

	/**
	 * \brief Draw a rounded rectangle around the current tab
	 * \param dc Device context
	 * \param rect Surrounding rectangle of the button
	 */
	void DrawRegularHover(wxDC& dc, wxRect& rect);

	/**
	 * \brief Draw a web style hover effect ( cursor set to hand & text is underlined)
	 * \param dc Device context
	 * \param caption Label caption
	 * \param textX Caption X coord
	 * \param textY Caption Y coord
	 */
	void DrawWebHover(wxDC& dc, wxString& caption, int textX, int textY);


	/**
	 * \brief draws a bitmap as the background of the control
	 * \param bmp bitmap to use as background
	 */
	void SetBackgroundBitmap( wxBitmap& bmp ) { m_skin = bmp; }

	/// Implementation of the OnPaint() event handler
	DECLARE_EVENT_TABLE()
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnMouseLeftDown(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseLeftUp(wxMouseEvent& event);
	virtual void OnMouseLeaveWindow(wxMouseEvent& event);
};

/**
* \ingroup wxcontrols
*
*
* \par requirements
* win98 or later\n
* win2k or later\n
*
* \version 1.0
* An implementation of a notebook control - except that instead of 
* tabs to show labels, it labels to the right or left (arranged horozintaly)
*
* \date 02-18-2006
*
* \author Eran Ifrah
*/

class wxLabelBook : public wxFlatBookBase
{
public:
	/**
	 * \param parent - parent window
	 * \param id - window ID 
	 * \param pos - window position
	 * \param size - window size
	 * \param style - window style
	 * \param name - window class name
	 */
	wxLabelBook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxINB_LABEL_BOOK_DEFAULT, const wxString& name = wxT("wxLabelBook"));

	/**
	 * \brief default constructor
	 */
	wxLabelBook() {}

	/**
	 * \brief Destructor
	 */
	virtual ~wxLabelBook() {}


	/**
	 * \brief Create the image container for the wxFlatImageBook class
	 * \return wxImageContainer
	 */
	virtual wxImageContainerBase* CreateImageContainer();

	/**
	 * \brief Set at color parameter
	 * \param which - Which parameter to update 
	 * \param color - Color to use
	 */
	void SetColour(int which, const wxColor& color);

	/**
	 * \brief return the 'which' color
	 * \param which - Which color to return
	 * \return return the 'which' color
	 */
	wxColor GetColour(int which);

	/**
 	 * \brief draws a bitmap as the background of the control
	 * \param bmp bitmap to use as background
	 */
	void SetBackgroundBitmap( wxBitmap& bmp ) { static_cast<wxLabelContainer*>(m_pages)->SetBackgroundBitmap(bmp); }

	DECLARE_DYNAMIC_CLASS(wxLabelBook)
	DECLARE_EVENT_TABLE()
	void OnSize(wxSizeEvent& event);
};


#define wxIN_IMPEXP

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EXPORTED_EVENT_TYPE(wxIN_IMPEXP, wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGED, 51000)
DECLARE_EXPORTED_EVENT_TYPE(wxIN_IMPEXP, wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGING, 51001)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxImageNotebookEventFunction)(wxImageNotebookEvent&);

#define wxImageNotebookEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxImageNotebookEventFunction, &func)

#define EVT_IMAGENOTEBOOK_PAGE_CHANGED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGED, winid, wxImageNotebookEventHandler(fn))

#define EVT_IMAGENOTEBOOK_PAGE_CHANGING(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_COMMAND_IMAGENOTEBOOK_PAGE_CHANGING, winid, wxImageNotebookEventHandler(fn))
