#ifndef DIRTRAVERSER_H
#define DIRTRAVERSER_H

#include <wx/dir.h>
#include <vector>
#include <map>
#include <wx/filename.h>

/**
 * A helper class that help us recurse into sub-directories.
 * For each found directory, sink.OnDir() is called and sink.OnFile() is called for every file. 
 * Depending on the return value, the enumeration may continue or stop of one of the files, the 
 * recurse may stop.
 * \ingroup CodeLite
 * \version 1.0
 * first version
 * \date 11-08-2006
 * \author Eran
 */
class DirTraverser : public wxDirTraverser
{
	std::vector<wxFileName> m_files;
	std::map<wxString, bool> m_validExt;

public:
	/**
	 * By default we search for all files, unless a map of extensions is provided
	 * \param extMap a map with all valid extensions
	 */
	void SetExtenstions(const std::map<wxString, bool> extMap);

	/**
	 * This function is called once a file is found. The traverse of the directories 
	 * can be stopped based on the return value from this function:
	 * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
	 * - wxDIR_STOP,             // stop traversing
	 * - wxDIR_CONTINUE          // continue into this directory
	 * \param filename name of the file that was found
	 * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
	 */
	virtual wxDirTraverseResult  OnFile(const wxString& filename);

	/**
	 * This function is called once a directory is found. The traverse of the directories 
	 * can be stopped based on the return value from this function:
	 * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
	 * - wxDIR_STOP,             // stop traversing
	 * - wxDIR_CONTINUE          // continue into this directory
	 * \param dirname name of the directory that was found
	 * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
	 */
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
	{
		// do nothing
		return wxDIR_CONTINUE;
	}

	/**
	 * Return list of files found 
	 * \return Return list of files found 
	 */
	std::vector<wxFileName>& GetFiles()
	{
		return m_files;
	}
};

#endif //DIRTRAVERSER_H
