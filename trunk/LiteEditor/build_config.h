#ifndef BUILD_CONFIGURATION_H
#define BUILD_CONFIGURATION_H

#include "configuration_object.h"
#include <wx/arrstr.h>
#include "wx/string.h"

class BuildConfig : public ConfObject {
	wxString m_name;
	wxArrayString m_includePath;
	wxArrayString m_compileOptions;
	wxArrayString m_linkOptions;
	wxArrayString m_libs;
	wxArrayString m_libPath;
	wxArrayString m_preBuildCommands;
	wxArrayString m_postBuildCommands;

public:
	BuildConfig(wxXmlNode *node);
	virtual ~BuildConfig();
	wxXmlNode *ToXml() const;

	//--------------------------------
	// Setters / Getters
	//--------------------------------
	void GetIncludePath(wxArrayString &paths) { paths = m_includePath; }
	void GetCompileOptions(wxArrayString &options) { options = m_compileOptions; }
	void GetLinkOptions(wxArrayString &options) { options = m_linkOptions; }
	void GetPreBuildCommands(wxArrayString &cmds) { cmds = m_preBuildCommands; }
	void GetPostBuildCommands(wxArrayString &cmds) { cmds = m_postBuildCommands; }
	void GetLibraries(wxArrayString &libs) { libs = m_libs; }
	void GetLibPath(wxArrayString &libPaths) { libPaths = m_libPath; }
	const wxString &GetName() const { return m_name; }

	void SetIncludePath(const wxArrayString &paths) { m_includePath = paths; }
	void SetCompileOptions(const wxArrayString &options) { m_compileOptions = options; }
	void SetLinkOptions(const wxArrayString &options) { m_linkOptions = options; }
	void SetPreBuildCommands(const wxArrayString &cmds) { m_preBuildCommands = cmds; }
	void SetPostBuildCommands(const wxArrayString &cmds) { m_postBuildCommands = cmds; }
	void SetLibraries(const wxArrayString &libs) { m_libs = libs; }
	void SetLibPath(const wxArrayString &libPaths) { m_libPath = libPaths; }
	void SetName(const wxString &name){ m_name = name; }
};

typedef SmartPtr<BuildConfig> BuildConfigPtr;
#endif // BUILD_CONFIGURATION_H
