///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  6 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tags_options_dlg__
#define __tags_options_dlg__

#include <wx/wx.h>

#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "serialized_object.h"
#include "wx/filename.h"
#include "filepicker.h"

///////////////////////////////////////////////////////////////////////////
enum CodeCompletionOpts
{
	CC_PARSE_COMMENTS	= 1,
	CC_DISP_COMMENTS	= 2,
	CC_DISP_TYPE_INFO	= 4,
	CC_DISP_FUNC_CALLTIP= 8,
	CC_LOAD_EXT_DB		= 16
};

class TagsOptionsData : public SerializedObject
{
	size_t m_ccFlags;
	wxFileName m_prepFile;
	wxString m_fileSpec;
	wxArrayString m_languages;

public:
	TagsOptionsData();
	virtual ~TagsOptionsData();

	//setters/getters
	void SetFlags(const size_t &flags) {m_ccFlags = flags;}
	void SetFileSpec(const wxString &filespec){m_fileSpec = filespec;}
	void SetPreprocessorFilename(const wxFileName& fn){m_prepFile = fn;}
	void SetLanguages(const wxArrayString &langs){m_languages = langs;}

	const wxArrayString &GetLanguages() const {return m_languages;}
	const wxFileName& GetPreprocessorFilename() const {return m_prepFile;}
	const size_t& GetFlags() const {return m_ccFlags;}
	const wxString &GetFileSpec() const{return m_fileSpec;}

	//Serialization API
	void Serialize(Archive &arch);
	void DeSerialize(Archive &arch);
};

///////////////////////////////////////////////////////////////////////////////
/// Class TagsOptionsDlg
///////////////////////////////////////////////////////////////////////////////
class TagsOptionsDlg : public wxDialog 
{
private:

protected:
	wxNotebook* m_mainBook;
	wxPanel* m_generalPage;
	wxCheckBox* m_checkParseComments;
	wxCheckBox* m_checkDisplayComments;
	wxCheckBox* m_checkDisplayTypeInfo;
	wxCheckBox* m_checkDisplayFunctionTip;
	wxStaticText* m_staticTextHelp;
	wxPanel* m_ctagsPage;
	wxStaticText* m_staticText1;
	FilePicker* m_filePicker;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_textFileSpec;
	wxStaticText* m_staticText5;
	wxComboBox* m_comboBoxLang;
	wxStaticLine* m_staticline1;
	wxButton* m_buttonOK;
	wxButton* m_buttonCancel;
	wxCheckBox* m_checkLoadLastDB;
	TagsOptionsData m_data;

protected:
	void SetFlag(CodeCompletionOpts flag, bool set);

	void InitValues();
	void CopyData();
	void OnButtonOK(wxCommandEvent &event);

public:
	TagsOptionsDlg( wxWindow* parent, 
					const TagsOptionsData& data,					
				    int id = wxID_ANY, 
				    wxString title = wxT("Tags Options"), 
				    wxPoint pos = wxDefaultPosition, 
				    wxSize size = wxSize(469, 362), 
				    int style = wxDEFAULT_DIALOG_STYLE);

	TagsOptionsData &GetData() {return m_data;}
};

#endif //__tags_options_dlg__
