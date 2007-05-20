#ifndef NEW_DLG_H
#define NEW_DLG_H

#include "wx/dialog.h"
#include "wx/listctrl.h"
#include "ctags_manager.h"

DECLARE_EVENT_TYPE(wxEVT_NEW_DLG_CREATE, -1)

#define NEW_DLG_PROJECT   0
#define NEW_DLG_WORKSPACE 1

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;
class wxDirPickerCtrl;
class wxComboBox;
class wxFilePickerCtrl;
class wxFlatNotebook;
class wxListBox;

// Workspace information
class WorkspaceData {
public:
	wxString m_name;			//< Workspace name
	wxString m_path;			//< Workspace directoy
	wxString m_externalTagsDB;	//< External tags database to use 
};

class ProjectData {
public:
	wxString m_name;	//< project name
	wxString m_path;	//< project directoy
	wxString m_type;	//< project type (static library, dynamic or executable
};

// Workspace dialog
class NewDlg : public wxDialog
{
	WorkspaceData m_workspaceData;
	ProjectData m_projectData;

	// Options
	wxTextCtrl *m_name;
	wxDirPickerCtrl *m_pathPicker;
	wxFilePickerCtrl *m_tagsPicker;

	wxTextCtrl *m_projName;
	wxDirPickerCtrl *m_projPathPicker;
	wxListBox *m_projTypes;

	// Buttons
	wxButton *m_create;
	wxButton *m_cancel;
	wxButton *m_ctagsOptions;

	int m_selection;
	wxFlatNotebook *m_book;
	CtagsOptions m_ctagsOptionsData;

public:
	virtual ~NewDlg( );
	NewDlg();
	NewDlg(	wxWindow* parent, 
					int selection = NEW_DLG_WORKSPACE,  
					wxWindowID id = wxID_ANY, 
					const wxString& caption = wxT("New"), 
					const wxPoint& pos = wxDefaultPosition, 
					const wxSize& size = wxSize(400, 200), 
					long style = wxDEFAULT_DIALOG_STYLE);

	// Creation
	bool Create(wxWindow* parent, 
				int selection = NEW_DLG_WORKSPACE,  
				wxWindowID id = wxID_ANY, 
				const wxString& caption = wxT("New"), 
				const wxPoint& pos = wxDefaultPosition, 
				const wxSize& size = wxSize(400, 200), 
				long style = wxDEFAULT_DIALOG_STYLE
				);
 
	const WorkspaceData& GetWorksapceData() const { return m_workspaceData; }
	const ProjectData& GetProjectData() const { return m_projectData; }
	const CtagsOptions& GetCtagsOptions() const { return m_ctagsOptionsData; }

	// return the selected notebook tab
	int GetSelection() const;

protected:
	void CreateGUIControls();
	void ConnectEvents();
	void OnClick(wxCommandEvent &event);
	void OnListItemSelected(wxListEvent &event);

	wxWindow *CreateProjectPage();
	wxWindow *CreateWorkspacePage();
};
#endif // NEW_DLG_H

