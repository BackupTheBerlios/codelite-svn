#include "dirpicker.h"
#include "wx/sizer.h"
#include "macros.h"
#include <wx/dirdlg.h>

DirPicker::DirPicker(wxWindow *parent, wxWindowID id, const wxString &buttonCaption, const wxString &message, const wxPoint& pos, const wxSize& size, long style)
: wxPanel(parent, id, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER)
, m_path(NULL)
, m_combo(NULL)
, m_buttonCaption(buttonCaption)
, m_dlgCaption(message)
, m_style(style)
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

	if(m_style & wxDP_USE_TEXTCTRL){
		m_path = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		mainSizer->Add(m_path, 1, wxEXPAND | wxALL, 0);
	}else{
		m_combo = new wxComboBox(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize);
		mainSizer->Add(m_combo, 1, wxEXPAND | wxALL, 0);
	}

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
		if(m_style & wxDP_USE_TEXTCTRL)
			m_path->SetValue(path);
		else
			m_combo->SetValue(path);
	}
	dlg->Destroy();
}

wxString DirPicker::GetPath() const
{
	if(m_style & wxDP_USE_TEXTCTRL)
		return m_path->GetValue();
	else{
		return m_combo->GetStringSelection();
	}
}

void DirPicker::SetPath(const wxString &path)
{
	if(m_style & wxDP_USE_TEXTCTRL)
		return m_path->SetValue(path);
	else
		return m_combo->SetValue(path);
}

void DirPicker::SetValues(const wxArrayString &values, int sel)
{
	wxASSERT_MSG(m_style & wxDP_USE_COMBOBOX, wxT("SetValues is available only for wxDP_USE_COMBOBOX style"));
	m_combo->Append(values);
	m_combo->SetSelection(sel);
}
