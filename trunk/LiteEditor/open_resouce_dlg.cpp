///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "open_resouce_dlg.h"
#include "macros.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////

OpenResourceDlg::OpenResourceDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) 
: wxDialog( parent, id, title, pos, size, style )
{
	m_timer = new wxTimer(this);
	m_timer->Start(100);

	//load all files from the workspace
	ManagerST::Get()->GetWorkspaceFiles(m_files, true);

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticTitle = new wxStaticText( mainPanel, wxID_ANY, wxT("Find Resource:"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_staticTitle, 0, wxALL, 5 );
	
	m_textResourceName = new wxTextCtrl(mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	panelSizer->Add( m_textResourceName, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticText *label = new wxStaticText( mainPanel, wxID_ANY, wxT("Matched resources:"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( label, 0, wxALL, 5 );

	m_listShortNames = new wxListBox( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	panelSizer->Add( m_listShortNames, 1, wxALL|wxEXPAND, 5 );
	
	mainPanel->SetSizer( panelSizer );
	mainPanel->Layout();
	panelSizer->Fit( mainPanel );
	mainSizer->Add( mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_btnOk, 0, wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_button2, 0, wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	ConnectEvents();
	m_textResourceName->SetFocus();
}

OpenResourceDlg::~OpenResourceDlg()
{
	m_timer->Stop();
	delete m_timer;
}

void OpenResourceDlg::ConnectEvents()
{
	ConnectButton(m_button2, OpenResourceDlg::OnButtonCancel);
	ConnectButton(m_btnOk, OpenResourceDlg::OnButtonOK);
	m_listShortNames->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(OpenResourceDlg::OnItemActivated), NULL, this);
	m_textResourceName->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(OpenResourceDlg::OnEnterHit), NULL, this);
	Connect(m_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(OpenResourceDlg::OnTimer), NULL, this);
}

void OpenResourceDlg::OnTimer(wxTimerEvent &event)
{
	wxUnusedVar(event);
	wxArrayString tmpArr;
	wxString curSel = m_textResourceName->GetValue();
	if(!curSel.IsEmpty())
	{
		for(size_t i=0; i<m_files.size(); i++)
		{
			if(m_files[i].GetFullName() == curSel)
			{
				tmpArr.Add(m_files[i].GetFullPath());
			}
		}
	}	

	wxArrayString actValues = m_listShortNames->GetStrings();
	actValues.Sort();
	tmpArr.Sort();

	if(tmpArr == actValues)
		return;

	//change was done, update the file list
	Freeze();
	m_listShortNames->Clear();
	for(size_t i=0; i<tmpArr.GetCount(); i++)
		m_listShortNames->Append(tmpArr.Item(i));
	Thaw();
}

void OpenResourceDlg::OnButtonCancel(wxCommandEvent &event)
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void OpenResourceDlg::OnButtonOK(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if(UpdateFileName())
	{
		EndModal(wxID_OK);	
	}
}

void OpenResourceDlg::OnItemActivated(wxCommandEvent &event)
{
	wxUnusedVar(event);

	if(UpdateFileName())
	{
		EndModal(wxID_OK);	
	}
}

bool OpenResourceDlg::UpdateFileName()
{
	wxString openWhat = m_listShortNames->GetStringSelection();
	if(openWhat.IsEmpty())
		return false;

	m_fileName = openWhat;
	return true;
}


void OpenResourceDlg::OnEnterHit(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if(m_listShortNames->GetCount() == 1){
		m_listShortNames->SetSelection(0);
		if(UpdateFileName()){
			EndModal(wxID_OK);
			return;
		}
	}
}
