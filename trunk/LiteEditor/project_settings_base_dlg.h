///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __project_settings_base_dlg__
#define __project_settings_base_dlg__

#include <wx/wx.h>

#include <wx/choice.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/filepicker.h>
#include <wx/panel.h>
#include <wx/gbsizer.h>
#include <wx/checklst.h>
#include <wx/treebook.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProjectSettingsBaseDlg
///////////////////////////////////////////////////////////////////////////////
class ProjectSettingsBaseDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText21;
		wxChoice* m_choiceConfigurationType;
		wxButton* m_buttonConfigManager;
		wxStaticLine* m_staticline81;
		wxTreebook* m_notebook3;
		wxPanel* m_generalPage;
		wxStaticText* m_staticText22;
		wxChoice* m_choiceProjectTypes;
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
		wxTextCtrl* m_textCompilerName;
		wxStaticLine* m_staticline6;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCompilerOptions;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textAdditionalSearchPath;
		wxButton* m_buttonAddSearchPath;
		wxPanel* m_linkerPage;
		wxCheckBox* m_checkLinkerNeeded;
		wxStaticLine* m_staticline8;
		wxStaticText* m_staticText23;
		wxTextCtrl* m_textLinkerName;
		wxStaticLine* m_staticline9;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textLinkerOptions;
		wxStaticText* m_staticText7;
		wxTextCtrl* m_textLibraryPath;
		wxButton* m_buttonLibraries;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_textLibraries;
		wxButton* m_buttonLibraryPath;
		wxPanel* m_preBuildPage;
		wxStaticText* m_staticText11;
		wxStaticLine* m_staticline2;
		wxCheckListBox* m_checkListPreBuildCommands;
		wxButton* m_buttonNewPreBuildCmd;
		wxButton* m_buttonDeletePreBuildCmd;
		wxButton* m_buttonEditPreBuildCmd;
		wxButton* m_buttonUpPreBuildCmd;
		wxButton* m_buttonDownPreBuildCmd;
		wxPanel* m_postBuildPage;
		wxStaticText* m_staticText111;
		wxStaticLine* m_staticline21;
		wxCheckListBox* m_checkListPostBuildCommands;
		wxButton* m_buttonNewPostBuildCmd;
		wxButton* m_buttonDeletePostBuildCmd;
		wxButton* m_buttonEditPostBuildCmd;
		wxButton* m_buttonUpPostBuildCmd;
		wxButton* m_buttonDownPostBuildCmd;
		wxPanel* m_toolsPage;
		wxStaticText* m_staticText191;
		wxTextCtrl* m_textArchiveTool;
		wxStaticText* m_staticText211;
		wxTextCtrl* m_textCleanTool;
		wxStaticLine* m_staticline1;
		wxButton* m_buttonOK;
		wxButton* m_buttonCancel;
		wxButton* m_buttonApply;
	
	public:
		ProjectSettingsBaseDlg( wxWindow* parent, int id = wxID_ANY, wxString title = wxT("Project Settings"), wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 576,415 ), int style = wxDEFAULT_DIALOG_STYLE );
	
};

#endif //__project_settings_base_dlg__
