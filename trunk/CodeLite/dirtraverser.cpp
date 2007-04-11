#include "dirtraverser.h"
#include "wx/tokenzr.h"
#include "wx/filename.h"

DirTraverser::DirTraverser(const wxString &filespec)
: wxDirTraverser()
, m_filespec(filespec)
{
	if(m_filespec.Trim() == wxT("*.*") || m_filespec.Trim() == wxT("*")){
		m_specMap.clear();
	} else {
		wxStringTokenizer tok(m_filespec, wxT(";"));
		while( tok.HasMoreTokens() ){
			std::pair<wxString, bool> val;
			val.first = tok.GetNextToken().AfterLast(wxT('*'));
			val.first = val.first.AfterLast(wxT('.')).MakeLower();
			val.second = true;
			m_specMap.insert( val );
		}
	}
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& filename)
{
	// add the file to our array
	wxFileName fn(filename);

	if( m_specMap.empty() ){
		m_files.Add(filename);
	} else if(m_specMap.find(fn.GetExt().MakeLower()) != m_specMap.end()){
		m_files.Add(filename);
	}
	return wxDIR_CONTINUE;
}
