#ifndef __FLAT_BUTTON_H__
#define __FLAT_BUTTON_H__

#ifndef WXMAKINGDLL_FLATMENU_EXPIMP
#ifdef WXMAKINGDLL_FLATMENU
#    define WXMAKINGDLL_FLATMENU_EXPIMP WXEXPORT
#elif defined(WXUSINGDLL)
#    define WXMAKINGDLL_FLATMENU_EXPIMP WXIMPORT
#else /* not making nor using DLL */
#    define WXMAKINGDLL_FLATMENU_EXPIMP
#endif // WXMAKINGDLL_FLATMENU
#endif 
#include <wx/panel.h>
#include <wx/bitmap.h>
#include "art_manager.h"
#include <wx/timer.h>

class wxTimer;
class wxFlatMenuBase;

/**
 * \ingroup wxflatmenu
 * \version 1.0
 * first version
 *
 * \date 11-04-2006
 *
 * \author Eran
 * A nice small class that functions like wxBitmapButton, the reason
 * I did not used BitmapButton is that on Linux, it has some extra margins
 * that I cant seem to remove
 */
class WXMAKINGDLL_FLATMENU_EXPIMP wxFlatMenuButton
{
	wxBitmap m_normalBmp, m_disabledBmp;
	ControlState m_state;
	wxTimer *m_timer;
	bool m_up;
	wxPoint m_pos;
	wxSize m_size;
	wxFlatMenuBase* m_parent;
	int m_timerID;

public:
	/**
	 * \brief constructor
	 */
	wxFlatMenuButton();

	/**
	 * Constructor
	 * \param normalBmp normal bitmap
	 * \param disabledBmp disabled bitmap
	 */
	wxFlatMenuButton(wxFlatMenuBase *menu, bool up, const wxBitmap &normalBmp, const wxBitmap &disabledBmp = wxNullBitmap);

	/**
	 * Create button (two phase contruction)
	 * \param normalBmp normal bitmap
	 * \param disabledBmp disabled bitmap
	 */
	bool Create(wxFlatMenuBase *menu, bool up, const wxBitmap &normalBmp, const wxBitmap &disabledBmp = wxNullBitmap);

	void Move(const wxPoint &pos){ m_pos = pos;}
	void Move(int x, int y) { m_pos = wxPoint(x, y); }
	void SetSize(int x, int y) { m_size = wxSize(x, y); }
	void SetSize(const wxSize& size) { m_size = size; }
	wxRect GetClientRect() const {return wxRect(m_pos, m_size);}

	/**
	 * \brief destructor
	 */
	virtual ~wxFlatMenuButton();

	/**
	 * Set the button bitmaps
	 * \param normalBmp 
	 * \param disabledBmp 
	 */
	void SetBitmap(const wxBitmap &normalBmp, const wxBitmap &disabledBmp = wxNullBitmap);
	
	/**
	 * Draw self at rect using dc
	 * \param dc device context
	 * \param rect rectangle
	 */
	void Draw(wxDC& dc);

	/**
	 * Return the timer object ID
	 */
	int GetTimerId() const { return m_timerID;}

	/**
	 * Return the timer 
	 */
	wxTimer *GetTimer() { return m_timer; }

	// attach event to this class
	bool Contains(wxPoint &pt);

	virtual bool ProcessLeftDown(wxPoint &pt);
	virtual bool ProcessLeftUp(wxPoint &pt);
	virtual bool ProcessMouseMove(wxPoint &pt);
};

#endif 
