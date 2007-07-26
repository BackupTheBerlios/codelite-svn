#include "tags_options_data.h"

//------------------------------------------------------------------------------
// CtagsOptions
//------------------------------------------------------------------------------
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

