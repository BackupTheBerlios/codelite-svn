#include "tags_options_data.h"

//---------------------------------------------------------

TagsOptionsData::TagsOptionsData() 
: SerializedObject()
, m_ccFlags(0)
, m_fileSpec(wxT("*.cpp;*.cc;*.cxx;*.h;*.hpp;*.c;*.c++"))
{
	m_languages.Add(wxT("C++"));
	m_languages.Add(wxT("Java"));
}

TagsOptionsData::~TagsOptionsData()
{
}

void TagsOptionsData::Serialize(Archive &arch)
{
	arch.Write(wxT("m_ccFlags"), m_ccFlags);
	arch.Write(wxT("m_prepFile"), m_prepFile);
	arch.Write(wxT("m_fileSpec"), m_fileSpec);
	arch.Write(wxT("m_languages"), m_languages);
}

void TagsOptionsData::DeSerialize(Archive &arch)
{
	arch.Read(wxT("m_ccFlags"), m_ccFlags);
	arch.Read(wxT("m_prepFile"), m_prepFile);
	arch.Read(wxT("m_fileSpec"), m_fileSpec);
	arch.Read(wxT("m_languages"), m_languages);
}

wxString TagsOptionsData::ToString() const
{
	wxString options(wxEmptyString);
	if(GetLanguages().IsEmpty() == false){
		options += wxT(" --language-force=");
		options += GetLanguages().Item(0);
		options += wxT(" ");
	}

	if(GetPreprocessorFilename().IsOk()){
		options += wxT("-I@\"");
		options += GetPreprocessorFilename().GetFullPath();
		options += wxT("\"");
	}
	return options;
}

void TagsOptionsData::SetLanguageSelection(const wxString &lang)
{
	int where = m_languages.Index(lang);
	if(where != wxNOT_FOUND)
	{
		m_languages.RemoveAt(where);
	}
	m_languages.Insert(lang, 0);
}
