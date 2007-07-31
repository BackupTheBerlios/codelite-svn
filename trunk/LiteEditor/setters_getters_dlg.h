#ifndef __setters_getters_dlg__
#define __setters_getters_dlg__

/**
@file
Subclass of SettersGettersBaseDlg, which is generated by wxFormBuilder.
@todo Add your event handlers directly to this file.
*/

#include "setters_getters_base_dlg.h"
#include "manager.h"
#include "ctags_manager.h"

/** Implementing SettersGettersBaseDlg */
class SettersGettersDlg : public SettersGettersBaseDlg
{
	std::vector<TagEntryPtr> m_members;
	wxFileName m_file;
	int m_lineno;
	std::map<wxString, TagEntryPtr> m_tagsMap;
	wxString m_code;

protected:
	void OnCheckStartWithUpperCase(wxCommandEvent &event);
	wxString GenerateFunctions();
	wxString GenerateSetter(TagEntryPtr tag);
	wxString GenerateGetter(TagEntryPtr tag);
	void FormatName(wxString &name);
	void UpdatePreview();
	void GenerateGetters(wxString &code);
	void GenerateSetters(wxString &code);

public:
	/** Constructor */
	SettersGettersDlg( wxWindow* parent, const std::vector<TagEntryPtr> &tags, const wxFileName &file, int lineno);
	const wxString &GetGenCode() const {return m_code;}
};

#endif // __setters_getters_dlg__
