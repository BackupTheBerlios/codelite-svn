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

#include "edit_workspace_conf_dlg.h"
#include "macros.h"
#include "manager.h"

///////////////////////////////////////////////////////////////////////////

EditWorkspaceConfDlg::EditWorkspaceConfDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_wspConfList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	mainSizer->Add( m_wspConfList, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* btnSizer;
	btnSizer = new wxBoxSizer( wxVERTICAL );
	
	m_buttonRename = new wxButton( this, wxID_ANY, wxT("&Rename"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonRename, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, wxT("&Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonDelete, 0, wxALL, 5 );
	
	m_buttonClose = new wxButton( this, wxID_CANCEL, wxT("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
	btnSizer->Add( m_buttonClose, 0, wxALL, 5 );
	
	mainSizer->Add( btnSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();

	CustomInit();
}

void EditWorkspaceConfDlg::OnListBoxDClick(wxCommandEvent &event)
{
	DoRename(event.GetString());
}

void EditWorkspaceConfDlg::CustomInit()
{
	//fill the list box
	FillList();

	ConnectButton(m_buttonDelete, EditWorkspaceConfDlg::OnDelete);
	ConnectButton(m_buttonRename, EditWorkspaceConfDlg::OnRename);
	ConnectListBoxDClick(m_wspConfList, EditWorkspaceConfDlg::OnRename);
}

void EditWorkspaceConfDlg::FillList()
{
	m_wspConfList->Clear();
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	std::list<WorkspaceConfigurationPtr> confs;

	confs = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = confs.begin();

	for(; iter != confs.end(); iter++){
		m_wspConfList->Append((*iter)->GetName());	
		m_wspConfList->SetSelection(0);
	}
}

void EditWorkspaceConfDlg::OnDelete(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if(m_wspConfList->GetCount() == 0){
		return;
	}
	wxString delMe = m_wspConfList->GetStringSelection();
	if(delMe.IsEmpty()){
		return;
	}

	//remove the requested workspace build configuration
	BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
	wxString msg;
	msg << wxT("Remove workspace configuration '") << delMe << wxT("' ?");
	if(wxMessageBox(msg, wxT("Lite Editor"), wxICON_QUESTION | wxYES_NO | wxCANCEL) == wxYES){
		matrix->RemoveConfiguration(delMe);
		//apply changes
		ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);

		//refresh list
		FillList();
	}
}

void EditWorkspaceConfDlg::DoRename(const wxString &selItem)
{
	wxTextEntryDialog *dlg = new wxTextEntryDialog(this, wxT("New Configuration Name:"), wxT("Rename"), selItem);
	if(dlg->ShowModal() == wxID_OK){
		wxString newName = dlg->GetValue();
		TrimString(newName);
		if(!newName.IsEmpty()){
			BuildMatrixPtr matrix = ManagerST::Get()->GetWorkspaceBuildMatrix();
			WorkspaceConfigurationPtr conf = matrix->GetConfigurationByName(selItem);
			//rename the configuration
			conf->SetName(newName);
			matrix->SetConfiguration(conf);
			matrix->RemoveConfiguration(selItem);

			//apply changes
			ManagerST::Get()->SetWorkspaceBuildMatrix(matrix);
			//refresh list
			FillList();
		}
	}
	dlg->Destroy();
}

void EditWorkspaceConfDlg::OnRename(wxCommandEvent &event)
{
	wxString changeMe;
	wxUnusedVar(event);
	if(m_wspConfList->GetCount() == 0){
		return;
	}
	
	changeMe = m_wspConfList->GetStringSelection();
	if(changeMe.IsEmpty()){
		return;
	}
	
	DoRename(changeMe);
}
