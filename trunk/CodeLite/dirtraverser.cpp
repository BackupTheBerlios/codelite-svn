#include "dirtraverser.h"

wxDirTraverseResult DirTraverser::OnFile(const wxString& filename)
{
	// add the file to our array
	wxFileName file(filename);

	if(m_validExt.empty()){
		m_files.push_back(filename);
	} else {
		// we a have a list of extesions
		if(m_validExt.find(file.GetExt().MakeLower()) != m_validExt.end()){
			m_files.push_back(filename);
		}
	}
	return wxDIR_CONTINUE;
}


void DirTraverser::SetExtensions(const std::map<wxString, bool> extMap)
{
	m_validExt.clear();

	// loop over the input map and convert all entries to upper case before inserting
	// them to our map
	std::map<wxString, bool>::const_iterator iter = extMap.begin();
	for(; iter != extMap.end(); iter++){
		wxString key = iter->first; // the key
		m_validExt[key.MakeLower()]  = true;
	}
}
