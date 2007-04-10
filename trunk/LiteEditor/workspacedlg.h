#ifndef WORKSPACE_DLG_H
#define WORKSPACE_DLG_H

#include "wx/dialog.h"

DECLARE_EVENT_TYPE(wxEVT_CREATE_WORKSPACE, -1)

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;
class wxDirPickerCtrl;
class wxComboBox;
class wxFilePickerCtrl;

// Workspace information
class WorkspaceData {
public:
	wxString m_name;			//< Workspace name
	wxString m_path;			//< Workspace directoy
	wxString m_externalTagsDB;	//< External tags database to use 
};

// Workspace dialog
class WorkspaceDlg : public wxDialog
{
	wxEvtHandler *m_owner;
	WorkspaceData m_data;

	// Options
	wxTextCtrl *m_name;
	wxDirPickerCtrl *m_pathPicker;
	wxFilePickerCtrl *m_tagsPicker;

	// Buttons
	wxButton *m_create;
	wxButton *m_cancel;
	
public:
	virtual ~WorkspaceDlg( );
	WorkspaceDlg();
	WorkspaceDlg(	wxWindow* parent, 
					const WorkspaceData& data, 
					wxWindowID id = wxID_ANY, 
					const wxString& caption = wxT("Create Workspace"), 
					const wxPoint& pos = wxDefaultPosition, 
					const wxSize& size = wxSize(400, 200), 
					long style = wxDEFAULT_DIALOG_STYLE);

	// Creation
	bool Create(wxWindow* parent, 
				const WorkspaceData& data, 
				wxWindowID id = wxID_ANY, 
				const wxString& caption = wxT("Create Workspace"), 
				const wxPoint& pos = wxDefaultPosition, 
				const wxSize& size = wxSize(400, 200), 
				long style = wxDEFAULT_DIALOG_STYLE
				);
 
	const WorkspaceData& GetData() const { return m_data; }

protected:
	void CreateGUIControls();
	void ConnectEvents();
	void OnClick(wxCommandEvent &event);
};
#endif // WORKSPACE_DLG_H

