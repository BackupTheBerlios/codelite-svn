#include "dirpicker.h"
#include "wx/sizer.h"
#include "macros.h"
#include <wx/dirdlg.h>

DirPicker::DirPicker(wxWindow *parent, wxWindowID id, const wxString &message, const wxString &buttonCaption, const wxPoint& pos, const wxSize& size)
: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
, m_buttonCaption(buttonCaption)
, m_dlgCaption(message)
{
	CreateControls();
	ConnectEvents();
}

DirPicker::~DirPicker()
{
}

void DirPicker::CreateControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	SetSizer(mainSizer);

	m_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
	mainSizer->Add(m_path, 1, wxEXPAND | wxALL, 0);

	m_button = new wxButton(this, wxID_ANY, m_buttonCaption);
	mainSizer->Add(m_button, 0, wxLEFT, 5);

	Layout();
}

void DirPicker::ConnectEvents()
{
	ConnectButton(m_button, DirPicker::OnButtonClicked);
}

void DirPicker::OnButtonClicked(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxDirDialog *dlg = new wxDirDialog(this, m_dlgCaption, wxGetCwd());
	if(dlg->ShowModal() == wxID_OK)
	{
		// Get the dirname
		wxString path = dlg->GetPath();
		m_path->SetValue(path);
	}
	dlg->Destroy();
}
