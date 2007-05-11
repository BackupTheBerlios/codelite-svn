///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __project_settings_dlg__
#define __project_settings_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/filepicker.h>
#include <wx/statline.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/notebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSettingsDlg
///////////////////////////////////////////////////////////////////////////////
class ProjectSettingsDlg : public wxDialog 
{
	DECLARE_EVENT_TABLE()
private:

	// Private event handlers
	void _wxFB_OnConfigurationTypeSelected( wxCommandEvent& event ){ OnConfigurationTypeSelected( event ); }
	void _wxFB_OnCheckCompilerNeeded( wxCommandEvent& event ){ OnCheckCompilerNeeded( event ); }
	void _wxFB_OnButtonAddSearchPath( wxCommandEvent& event ){ OnButtonAddSearchPath( event ); }
	void _wxFB_OnButtonAddCompilerOption( wxCommandEvent& event ){ OnButtonAddCompilerOption( event ); }
	void _wxFB_OnCheckLinkerNeeded( wxCommandEvent& event ){ OnCheckLinkerNeeded( event ); }
	void _wxFB_OnButtonAddLibraryPath( wxCommandEvent& event ){ OnButtonAddLibraryPath( event ); }
	void _wxFB_OnButtonAddLibrary( wxCommandEvent& event ){ OnButtonAddLibrary( event ); }
	void _wxFB_OnButtonAddLinkerOption( wxCommandEvent& event ){ OnButtonAddLinkerOption( event ); }
	void _wxFB_OnPreBuildCommandSelected( wxCommandEvent& event ){ OnPreBuildCommandSelected( event ); }
	void _wxFB_OnPreBuildCommandDClick( wxCommandEvent& event ){ OnPreBuildCommandDClick( event ); }
	void _wxFB_OnButtonNewPreBuildCommand( wxCommandEvent& event ){ OnButtonNewPreBuildCommand( event ); }
	void _wxFB_OnButtonEditPreBuildCommand( wxCommandEvent& event ){ OnButtonEditPreBuildCommand( event ); }
	void _wxFB_OnButtonUpPreBuildCommand( wxCommandEvent& event ){ OnButtonUpPreBuildCommand( event ); }
	void _wxFB_OnButtonDownPreBuildCommand( wxCommandEvent& event ){ OnButtonDownPreBuildCommand( event ); }
	void _wxFB_OnPostBuildCommandSelected( wxCommandEvent& event ){ OnPostBuildCommandSelected( event ); }
	void _wxFB_OnPostBuildCommandDClick( wxCommandEvent& event ){ OnPostBuildCommandDClick( event ); }
	void _wxFB_OnButtonNewPostBuildCommand( wxCommandEvent& event ){ OnButtonNewPostBuildCommand( event ); }
	void _wxFB_OnButtonEditPostBuildCommand( wxCommandEvent& event ){ OnButtonEditPostBuildCommand( event ); }
	void _wxFB_OnButtonUpPostBuildCommand( wxCommandEvent& event ){ OnButtonUpPostBuildCommand( event ); }
	void _wxFB_OnButtonDownPostBuildCommand( wxCommandEvent& event ){ OnButtonDownPostBuildCommand( event ); }
	void _wxFB_OnDialogButtonOK( wxCommandEvent& event ){ OnDialogButtonOK( event ); }
	void _wxFB_OnDialogButtonCancel( wxCommandEvent& event ){ OnDialogButtonCancel( event ); }
	void _wxFB_OnDialogButtonApply( wxCommandEvent& event ){ OnDialogButtonApply( event ); }


protected:
	wxStaticText* m_staticText21;
	wxChoice* m_choiceConfigurationType;
	wxNotebook* m_notebook3;
	wxPanel* m_generalPage;
	wxStaticText* m_staticText15;
	wxFilePickerCtrl* m_outputFilePicker;
	wxStaticText* m_staticText16;
	wxDirPickerCtrl* m_intermediateDirPicker;
	wxStaticText* m_staticText17;
	wxStaticLine* m_staticline5;
	wxStaticText* m_staticText18;
	wxTextCtrl* m_textCommand;
	wxStaticText* m_staticText19;
	wxTextCtrl* m_textCommandArguments;
	wxStaticText* m_staticText20;
	wxDirPickerCtrl* m_workingDirPicker;
	wxPanel* m_compilerPage;
	wxCheckBox* m_checkCompilerNeeded;
	wxStaticLine* m_staticline7;
	wxStaticText* m_staticText3;
	wxChoice* m_compilerNameChoice;
	wxStaticLine* m_staticline6;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_textAdditionalSearchPath;
	wxButton* m_buttonAddSearchPath;
	wxStaticText* m_staticText6;
	wxTextCtrl* m_textCompilerOptions;
	wxButton* m_buttonCompilerOptions;
	wxPanel* m_linkerPage;
	wxCheckBox* m_checkLinkerNeeded;
	wxStaticLine* m_staticline8;
	wxStaticText* m_staticText7;
	wxTextCtrl* m_textLibraryPath;
	wxButton* m_buttonLibraryPath;
	wxStaticText* m_staticText8;
	wxTextCtrl* m_textLibraries;
	wxButton* m_buttonLibraries;
	wxStaticText* m_staticText10;
	wxTextCtrl* m_textLinkerOptions;
	wxButton* m_buttonLinkerOptions;
	wxPanel* m_preBuildPage;
	wxStaticText* m_staticText11;
	wxStaticLine* m_staticline2;
	wxCheckListBox* m_checkListPreBuildCommands;
	wxButton* m_buttonNewPreBuildCmd;
	wxButton* m_buttonEditPreBuildCmd;
	wxButton* m_buttonUpPreBuildCmd;
	wxButton* m_buttonDownPreBuildCmd;
	wxPanel* m_postBuildPage;
	wxStaticText* m_staticText111;
	wxStaticLine* m_staticline21;
	wxCheckListBox* m_checkListPostBuildCommands;
	wxButton* m_buttonNewPostBuildCmd;
	wxButton* m_buttonEditPostBuildCmd;
	wxButton* m_buttonUpPostBuildCmd;
	wxButton* m_buttonDownPostBuildCmd;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	wxButton* m_buttonApply;

	// Virtual event handlers, overide them in your derived class
	virtual void OnConfigurationTypeSelected( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnCheckCompilerNeeded( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonAddSearchPath( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonAddCompilerOption( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnCheckLinkerNeeded( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonAddLibraryPath( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonAddLibrary( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonAddLinkerOption( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPreBuildCommandSelected( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPreBuildCommandDClick( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonNewPreBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonEditPreBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonUpPreBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonDownPreBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPostBuildCommandSelected( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnPostBuildCommandDClick( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonNewPostBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonEditPostBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonUpPostBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnButtonDownPostBuildCommand( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnDialogButtonOK( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnDialogButtonCancel( wxCommandEvent& event ){ event.Skip(); }
	virtual void OnDialogButtonApply( wxCommandEvent& event ){ event.Skip(); }


public:
	ProjectSettingsDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Project Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 576,415 ), int style = wxDEFAULT_DIALOG_STYLE );

};

#endif //__project_settings_dlg__
