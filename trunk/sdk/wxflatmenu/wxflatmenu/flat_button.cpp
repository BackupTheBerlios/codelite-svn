#include "flat_button.h"
#include <wx/dcbuffer.h>
#include <wx/event.h>
#include <wx/settings.h>
#include "flat_menu.h"

#ifdef DEVELOPMENT
#include <wx/log.h>
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; wxLogMessage( logmsg ); }
#else
# define FNB_LOG_MSG( msg ) { wxString logmsg; logmsg << msg; }
#endif

wxFlatMenuButton::wxFlatMenuButton(wxFlatMenuBase *menu, bool up, const wxBitmap &normalBmp, const wxBitmap &disabledBmp)
{
	Create(menu, up, normalBmp, disabledBmp);
}

wxFlatMenuButton::wxFlatMenuButton()
{
	m_timerID = wxNewId();
}

wxFlatMenuButton::~wxFlatMenuButton()
{
	if(m_timer)
		delete m_timer;
}

bool wxFlatMenuButton::Create(wxFlatMenuBase *menu, bool up, const wxBitmap &normalBmp, const wxBitmap &disabledBmp)
{
	m_parent = menu;
	m_timerID = wxNewId();

	m_timer = new wxTimer(m_parent, m_timerID);
	m_timer->Stop();

	wxASSERT_MSG(normalBmp.Ok(), wxT("Invalid bitmap!"));
	m_normalBmp = normalBmp;
	m_up = up;

	if( !disabledBmp.Ok() ) {
		m_disabledBmp = wxArtManagerST::Get()->CreateGreyBitmap(m_normalBmp);
	} else {
		m_disabledBmp = disabledBmp;
	}

	m_state = ControlNormal;
	return true;
}

bool wxFlatMenuButton::Contains(wxPoint &pt)
{
	wxRect rect(m_pos, m_size);
	if(!rect.Contains(pt))
	{
		return false;
	}
	return true;
}

void wxFlatMenuButton::Draw(wxDC &dc)
{
	wxRect rect(m_pos, m_size);
	int xx = rect.x + (rect.width - m_normalBmp.GetWidth())/2;
	int yy = rect.y + (rect.height - m_normalBmp.GetHeight())/2;

	wxArtManagerST::Get()->DrawButton(dc, rect, Style2007, m_state, *wxBLACK);
	dc.DrawBitmap(m_normalBmp, xx, yy, true);
}

bool wxFlatMenuButton::ProcessLeftDown(wxPoint &pt)
{
	if(!Contains(pt))
		return false;

	m_state = ControlPressed;
	m_parent->Refresh();
	if( m_up )
		m_parent->ScrollUp();
	else
		m_parent->ScrollDown();
	m_timer->Start(100);
	return true;
}

bool wxFlatMenuButton::ProcessLeftUp(wxPoint &pt)
{
	// always stop the timer
	m_timer->Stop();

	if(!Contains(pt))
		return false;

	m_state = ControlFocus;
	m_parent->Refresh();
	return true;
}

bool wxFlatMenuButton::ProcessMouseMove(wxPoint &pt)
{
	// pt is in parent coordiantes, convert it to our
	if(!Contains(pt)) 
	{
		m_timer->Stop();
		if(m_state != ControlNormal)
		{
			m_state = ControlNormal;
			m_parent->Refresh();
		}
		return false;
	}

	// Process mouse move event
	if(m_state != ControlFocus){
		if(m_state != ControlPressed)
		{
			m_state = ControlFocus;
			m_parent->Refresh();
		}
	}
	return true;
}
