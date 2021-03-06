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
// Define here a class per event/group
//-----------------------------------------------------------------

//------------------------------------
// Common edit tasks
//------------------------------------
class EditHandler : public MenuEventHandler 
{
public:
	EditHandler(int id) : MenuEventHandler(id){};
	virtual ~EditHandler(){};

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

//------------------------------------
// Find / Repalce 
//------------------------------------
class FindReplaceHandler : public MenuEventHandler 
{
public:
	FindReplaceHandler(int id) : MenuEventHandler(id){};
	virtual ~FindReplaceHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Goto
//------------------------------------
class GotoHandler : public MenuEventHandler 
{
public:
	GotoHandler(int id) : MenuEventHandler(id){};
	virtual ~GotoHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Bookmarks
//------------------------------------
class BookmarkHandler : public MenuEventHandler 
{
public:
	BookmarkHandler(int id) : MenuEventHandler(id){};
	virtual ~BookmarkHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//------------------------------------
// Go to definition
//------------------------------------
class GotoDefinitionHandler : public MenuEventHandler 
{
public:
	GotoDefinitionHandler(int id) : MenuEventHandler(id){};
	virtual ~GotoDefinitionHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};

//-------------------------------------------------
// View As
//-------------------------------------------------
class ViewAsHandler : public MenuEventHandler 
{
public:
	ViewAsHandler(int id) : MenuEventHandler(id){};
	virtual ~ViewAsHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};
//-------------------------------------------------
// Word wrap
//-------------------------------------------------
class WordWrapHandler : public MenuEventHandler 
{
public:
	WordWrapHandler(int id) : MenuEventHandler(id){};
	virtual ~WordWrapHandler(){};

public:
	virtual void ProcessCommandEvent(wxWindow *owner, wxCommandEvent &event);
	virtual void ProcessUpdateUIEvent(wxWindow *owner, wxUpdateUIEvent &event);
};
#endif // MENU_EVENT_HANDLERS_H

