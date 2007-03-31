#ifndef MENU_EVENT_HANDLERS_H
#define MENU_EVENT_HANDLERS_H

#include "wx/event.h"
#include "smart_ptr.h"

/**
 * The interface for menu event handler classes
 */ 
class MenuEventHandler
{
protected:
	int m_id;

public:
	MenuEventHandler(){};
	virtual ~MenuEventHandler(){};

	// handle an event
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event) = 0;
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event) = 0;

	// Setters/Getters
	const int &GetEventId() const { return m_id; }
	void SetEventId(const int &id) { m_id = id; }
};

typedef SmartPtr<MenuEventHandler> MenuEventHandlerPtr;

//-----------------------------------------------------------------
// Define here a class per event
//-----------------------------------------------------------------

//------------------------------------
// Copy
//------------------------------------
class CopyHandler : public MenuEventHandler 
{
public:
	CopyHandler() { m_id = wxID_COPY; };
	virtual ~CopyHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Cut
//------------------------------------
class CutHandler : public MenuEventHandler 
{
public:
	CutHandler(){m_id = wxID_CUT;};
	virtual ~CutHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Paste
//------------------------------------
class PasteHandler : public MenuEventHandler 
{
public:
	PasteHandler(){m_id = wxID_PASTE;};
	virtual ~PasteHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Undo
//------------------------------------
class UndoHandler : public MenuEventHandler 
{
public:
	UndoHandler(){m_id = wxID_UNDO;};
	virtual ~UndoHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Redo
//------------------------------------
class RedoHandler : public MenuEventHandler 
{
public:
	RedoHandler(){m_id = wxID_REDO;};
	virtual ~RedoHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};
#endif // MENU_EVENT_HANDLERS_H