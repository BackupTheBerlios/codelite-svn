#ifndef MENU_EVENT_HANDLERS_H
#define MENU_EVENT_HANDLERS_H

#include "wx/event.h"
#include "smart_ptr.h"
#include <wx/xrc/xmlres.h>

/**
 * The interface for menu event handler classes
 */ 
class MenuEventHandler
{
protected:
	int m_id;

public:
	MenuEventHandler(int id) : m_id(id) {};
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
	CopyHandler(int id) : MenuEventHandler(id){};
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
	CutHandler(int id) : MenuEventHandler(id){};
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
	PasteHandler(int id) : MenuEventHandler(id){};
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
	UndoHandler(int id) : MenuEventHandler(id){};
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
	RedoHandler(int id) : MenuEventHandler(id){};
	virtual ~RedoHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Redo
//------------------------------------
class SelectAllHandler : public MenuEventHandler 
{
public:
	SelectAllHandler(int id) : MenuEventHandler(id){};
	virtual ~SelectAllHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Redo
//------------------------------------
class DuplicateLineHandler : public MenuEventHandler 
{
public:
	DuplicateLineHandler(int id) : MenuEventHandler(id){};
	virtual ~DuplicateLineHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Brace matching
//------------------------------------
class BraceMatchHandler : public MenuEventHandler 
{
public:
	BraceMatchHandler(int id) : MenuEventHandler(id){};
	virtual ~BraceMatchHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};
#endif // MENU_EVENT_HANDLERS_H