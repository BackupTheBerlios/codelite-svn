#include "app.h"
#include <wx/image.h>
#include <wx/xrc/xmlres.h>
#include <wx/sysopt.h>


BEGIN_EVENT_TABLE(App, wxApp)
END_EVENT_TABLE()

IMPLEMENT_APP(App)

extern void InitXmlResource();
App::App(void)
: m_pMainFrame(NULL)
{
}

App::~App(void)
{
	wxImage::CleanUpHandlers();
}

bool App::OnInit()
{
	// Init resources and add the PNG handler
	wxSystemOptions::SetOption(wxT("msw.remap"), 0); 
	wxXmlResource::Get()->InitAllHandlers(); 
	wxImage::AddHandler( new wxPNGHandler ); 
	wxImage::AddHandler( new wxCURHandler ); 
	wxImage::AddHandler( new wxICOHandler );
	wxImage::AddHandler( new wxXPMHandler );

	InitXmlResource();

	// Create the main application window (a dialog in this case)
	// NOTE: Vertical dimension comprises the caption bar.
	//       Horizontal dimension has to take into account the thin
	//       hilighting border around the dialog (2 points in
	//       Win 95).
	m_pMainFrame = Frame::Get();

	// Center the dialog when first shown
	m_pMainFrame->Centre();

	// Show it and tell the application that it's our main window
	m_pMainFrame->Show(TRUE);

	SetTopWindow(m_pMainFrame);

	return TRUE;
} 

int App::OnExit()
{
	return 0;
}
