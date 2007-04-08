#ifndef DIR_SAVER_H
#define DIR_SAVER_H

#include "wx/string.h"
#include "wx/filefn.h"

// Utility class that helps keeping the current directory
class DirSaver 
{
	wxString m_curDir;
public:
	DirSaver(){
		m_curDir = wxGetCwd();
	}

	virtual ~DirSaver(){
		wxSetWorkingDirectory(m_curDir);
	}
};
#endif // DIR_SAVER_H
