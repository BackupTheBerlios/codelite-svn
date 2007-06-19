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

#include "project_settings_base_dlg.h"

///////////////////////////////////////////////////////////////////////////

ProjectSettingsBaseDlg::ProjectSettingsBaseDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, wxT("Configuration Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceConfigurationTypeChoices;
	m_choiceConfigurationType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceConfigurationTypeChoices, 0 );
	bSizer22->Add( m_choiceConfigurationType, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonConfigManager = new wxButton( this, wxID_ANY, wxT("Configuration Manager..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_buttonConfigManager, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer22, 0, wxEXPAND, 5 );
	
	m_staticline81 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline81, 0, wxEXPAND | wxALL, 5 );
	
	m_notebook3 = new wxTreebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_generalPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText22 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Project Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText22, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choiceProjectTypesChoices[] = { wxT("Static Library"), wxT("Dynamic Library"), wxT("Executable") };
	int m_choiceProjectTypesNChoices = sizeof( m_choiceProjectTypesChoices ) / sizeof( wxString );
	m_choiceProjectTypes = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceProjectTypesNChoices, m_choiceProjectTypesChoices, 0 );
	fgSizer3->Add( m_choiceProjectTypes, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText191 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText191, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceCompilerTypeChoices;
	m_choiceCompilerType = new wxChoice( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceCompilerTypeChoices, 0 );
	fgSizer3->Add( m_choiceCompilerType, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText15 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Output File:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText15, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_outputFilePicker = new wxFilePickerCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_USE_TEXTCTRL );
	fgSizer3->Add( m_outputFilePicker, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Intermediate Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_intermediateDirPicker = new wxDirPickerCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	fgSizer3->Add( m_intermediateDirPicker, 0, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText17 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Action:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_staticText17, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticline5 = new wxStaticLine( m_generalPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer21->Add( m_staticline5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer19->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText18 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Command:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_staticText18, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommand = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommand, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText19 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Command Arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_staticText19, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCommandArguments = new wxTextCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCommandArguments, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( m_generalPage, wxID_ANY, wxT("Working Directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_workingDirPicker = new wxDirPickerCtrl( m_generalPage, wxID_ANY, wxEmptyString, wxT("Select a folder"), wxDefaultPosition, wxDefaultSize, wxDIRP_USE_TEXTCTRL );
	fgSizer6->Add( m_workingDirPicker, 1, wxALL|wxEXPAND, 5 );
	
	bSizer19->Add( fgSizer6, 1, wxEXPAND, 5 );
	
	m_generalPage->SetSizer( bSizer19 );
	m_generalPage->Layout();
	bSizer19->Fit( m_generalPage );
	m_notebook3->AddPage( m_generalPage, wxT("General"), false );
	m_compilerPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* compilerPageSizer;
	compilerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkCompilerNeeded = new wxCheckBox( m_compilerPage, wxID_ANY, wxT("Compiler is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	
	compilerPageSizer->Add( m_checkCompilerNeeded, 0, wxALL, 5 );
	
	m_staticline7 = new wxStaticLine( m_compilerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	compilerPageSizer->Add( m_staticline7, 0, wxEXPAND | wxALL, 5 );
	
	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer( 0, 0 );
	gbSizer1->AddGrowableCol( 1 );
	gbSizer1->SetFlexibleDirection( wxBOTH );
	gbSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Compiler Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_staticText6, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerOptions = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textCompilerOptions, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText4 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Additional Search Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_staticText4, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textAdditionalSearchPath = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textAdditionalSearchPath, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddSearchPath = new wxButton( m_compilerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_buttonAddSearchPath, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText171 = new wxStaticText( m_compilerPage, wxID_ANY, wxT("Preprocessor:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_staticText171, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textPreprocessor = new wxTextCtrl( m_compilerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_textPreprocessor, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonAddPreprocessor = new wxButton( m_compilerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_buttonAddPreprocessor, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_buttonCompilerOptions = new wxButton( m_compilerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer1->Add( m_buttonCompilerOptions, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	compilerPageSizer->Add( gbSizer1, 1, wxEXPAND, 5 );
	
	m_compilerPage->SetSizer( compilerPageSizer );
	m_compilerPage->Layout();
	compilerPageSizer->Fit( m_compilerPage );
	m_notebook3->AddPage( m_compilerPage, wxT("Compiler"), true );
	m_linkerPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* linkerPageSizer;
	linkerPageSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkLinkerNeeded = new wxCheckBox( m_linkerPage, wxID_ANY, wxT("Linker is not required for this project"), wxDefaultPosition, wxDefaultSize, 0 );
	
	linkerPageSizer->Add( m_checkLinkerNeeded, 0, wxALL, 5 );
	
	m_staticline8 = new wxStaticLine( m_linkerPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	linkerPageSizer->Add( m_staticline8, 0, wxEXPAND | wxALL, 5 );
	
	wxGridBagSizer* gbSizer2;
	gbSizer2 = new wxGridBagSizer( 0, 0 );
	gbSizer2->AddGrowableCol( 1 );
	gbSizer2->SetFlexibleDirection( wxBOTH );
	gbSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_staticText10, wxGBPosition( 0, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerOptions = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLinkerOptions, wxGBPosition( 0, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_staticText7 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Library Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_staticText7, wxGBPosition( 1, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraryPath = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLibraryPath, wxGBPosition( 1, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonLibraries = new wxButton( m_linkerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_buttonLibraries, wxGBPosition( 2, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_staticText8 = new wxStaticText( m_linkerPage, wxID_ANY, wxT("Libraries:"), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_staticText8, wxGBPosition( 2, 0 ), wxGBSpan( 1, 1 ), wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLibraries = new wxTextCtrl( m_linkerPage, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_textLibraries, wxGBPosition( 2, 1 ), wxGBSpan( 1, 1 ), wxEXPAND|wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_buttonLibraryPath = new wxButton( m_linkerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_buttonLibraryPath, wxGBPosition( 1, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	m_buttonLinkerOptions = new wxButton( m_linkerPage, wxID_ANY, wxT("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	gbSizer2->Add( m_buttonLinkerOptions, wxGBPosition( 0, 2 ), wxGBSpan( 1, 1 ), wxTOP|wxBOTTOM|wxRIGHT, 5 );
	
	linkerPageSizer->Add( gbSizer2, 1, wxEXPAND, 5 );
	
	m_linkerPage->SetSizer( linkerPageSizer );
	m_linkerPage->Layout();
	linkerPageSizer->Fit( m_linkerPage );
	m_notebook3->AddPage( m_linkerPage, wxT("Linker"), false );
	m_preBuildPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText11 = new wxStaticText( m_preBuildPage, wxID_ANY, wxT("Specifies here a command lines to run in the pre-build event:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( m_preBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListPreBuildCommandsChoices;
	m_checkListPreBuildCommands = new wxCheckListBox( m_preBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListPreBuildCommandsChoices, 0 );
	bSizer9->Add( m_checkListPreBuildCommands, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonNewPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonDeletePreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonDeletePreBuildCmd, 0, wxALL, 5 );
	
	m_buttonEditPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonEditPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonUpPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonUpPreBuildCmd, 0, wxALL, 5 );
	
	m_buttonDownPreBuildCmd = new wxButton( m_preBuildPage, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_buttonDownPreBuildCmd, 0, wxALL, 5 );
	
	bSizer9->Add( bSizer10, 0, wxEXPAND, 5 );
	
	bSizer8->Add( bSizer9, 1, wxEXPAND, 5 );
	
	m_preBuildPage->SetSizer( bSizer8 );
	m_preBuildPage->Layout();
	bSizer8->Fit( m_preBuildPage );
	m_notebook3->AddPage( m_preBuildPage, wxT("PreBuild"), false );
	m_postBuildPage = new wxPanel( m_notebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText111 = new wxStaticText( m_postBuildPage, wxID_ANY, wxT("Specifies here a command lines to run in the post-build event:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer81->Add( m_staticText111, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 5 );
	
	m_staticline21 = new wxStaticLine( m_postBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer81->Add( m_staticline21, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxHORIZONTAL );
	
	wxArrayString m_checkListPostBuildCommandsChoices;
	m_checkListPostBuildCommands = new wxCheckListBox( m_postBuildPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListPostBuildCommandsChoices, 0 );
	bSizer91->Add( m_checkListPostBuildCommands, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonNewPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonDeletePostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonDeletePostBuildCmd, 0, wxALL, 5 );
	
	m_buttonEditPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonEditPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonUpPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonUpPostBuildCmd, 0, wxALL, 5 );
	
	m_buttonDownPostBuildCmd = new wxButton( m_postBuildPage, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer101->Add( m_buttonDownPostBuildCmd, 0, wxALL, 5 );
	
	bSizer91->Add( bSizer101, 0, wxEXPAND, 5 );
	
	bSizer81->Add( bSizer91, 1, wxEXPAND, 5 );
	
	m_postBuildPage->SetSizer( bSizer81 );
	m_postBuildPage->Layout();
	bSizer81->Fit( m_postBuildPage );
	m_notebook3->AddPage( m_postBuildPage, wxT("PostBuild"), false );
	
	mainSizer->Add( m_notebook3, 1, wxEXPAND | wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	m_buttonApply = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonApply, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}
