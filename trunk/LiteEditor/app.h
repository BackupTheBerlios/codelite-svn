#ifndef LITEEDITOR_APP_H
#define LITEEDITOR_APP_H

#include "frame.h"

class App : public wxApp
{
private:
	Frame *m_pMainFrame;

public:
	App(void);
	virtual ~App(void);

protected:
	virtual bool OnInit();
	virtual int OnExit();

private:
	DECLARE_EVENT_TABLE()
	void OnKeyDown(wxKeyEvent &event);
};

#endif // LITEEDITOR_APP_H
