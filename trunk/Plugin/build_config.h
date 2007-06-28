#ifndef BUILD_CONFIGURATION_H
#define BUILD_CONFIGURATION_H

#include "configuration_object.h"
#include <wx/arrstr.h>
#include "wx/string.h"
#include <list>

#ifndef WXDLLIMPEXP_LE_SDK
#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK
#endif 

class WXDLLIMPEXP_LE_SDK BuildCommand {
	wxString m_command;
	bool m_enabled;

public:
	BuildCommand()
		: m_command(wxEmptyString)
		, m_enabled(false)
	{
	}

	BuildCommand(const wxString &command, bool enabled)
		: m_command(command)
		, m_enabled(enabled)
	{}

	~BuildCommand()
	{}

	const wxString &GetCommand() const {return m_command;}
	bool GetEnabled() const { return m_enabled;}
	void SetCommand(const wxString &command) {m_command = command;}
	void SetEnabled(bool enabled) {m_enabled = enabled;}
};

typedef std::list<BuildCommand> BuildCommandList;

class WXDLLIMPEXP_LE_SDK BuildConfig : public ConfObject {
	wxString m_name;
	wxArrayString m_includePath;
	wxString m_compileOptions;
	wxString m_linkOptions;
	wxArrayString m_libs;
	wxArrayString m_libPath;
	BuildCommandList m_preBuildCommands;
	BuildCommandList m_postBuildCommands;
	bool m_compilerRequired;
	bool m_linkerRequired;

	wxString m_outputFile;
	wxString m_intermediateDirectory;
	wxString m_command;
	wxString m_commandArguments;
	wxString m_workingDirectory;
	wxString m_compilerType;
	wxString m_projectType;
	wxArrayString m_preprocessor;

private:
	void FillFromSmiColonString(wxArrayString &arr, const wxString &str);
	wxString ArrayToSmiColonString(const wxArrayString &array) const;
	void StripSemiColons(wxString &str);

	wxString NormalizePath(const wxString &path) const;

public:
	BuildConfig(wxXmlNode *node);
	virtual ~BuildConfig();
	wxXmlNode *ToXml() const;
	BuildConfig *Clone() const;

	//--------------------------------
	// Setters / Getters
	//--------------------------------
	wxString GetPreprocessor() const;
	void GetPreprocessor(wxArrayString &arr){ arr = m_preprocessor;}
	void SetPreprocessor(const wxString &prepr);
	
	const wxString &GetCompilerType() const { return m_compilerType; }
	void SetCompilerType(const wxString &cmpType) { m_compilerType = cmpType; }

	wxString GetIncludePath() const;
	const wxString &GetCompileOptions() const { return m_compileOptions; }
	const wxString &GetLinkOptions() const { return m_linkOptions; }
	wxString GetLibraries() const;
	wxString GetLibPath() const;
	
	void GetPreBuildCommands(BuildCommandList &cmds) { cmds = m_preBuildCommands; }
	void GetPostBuildCommands(BuildCommandList &cmds) { cmds = m_postBuildCommands; }
	const wxString &GetName() const { return m_name; }
	bool IsCompilerRequired() const { return m_compilerRequired; }
	bool IsLinkerRequired() const { return m_linkerRequired; }
	wxString GetOutputFileName() const { return NormalizePath(m_outputFile); }
	wxString GetIntermediateDirectory() const { return NormalizePath(m_intermediateDirectory); }
	const wxString &GetCommand() const { return m_command; }
	const wxString &GetCommandArguments() const { return m_commandArguments;}
	wxString GetWorkingDirectory() const { return NormalizePath(m_workingDirectory);}

	void SetIncludePath(const wxArrayString &paths) { m_includePath = paths; }
	// set the include path from a semi-colon string
	void SetIncludePath(const wxString &path);
	void SetLibraries(const wxString &libs);
	void SetLibPath(const wxString &path);

	void SetCompileOptions(const wxString &options) { m_compileOptions = options; }
	void SetLinkOptions(const wxString &options) { m_linkOptions = options; }
	void SetPreBuildCommands(const BuildCommandList &cmds) { m_preBuildCommands = cmds; }
	void SetPostBuildCommands(const BuildCommandList &cmds) { m_postBuildCommands = cmds; }
	void SetLibraries(const wxArrayString &libs) { m_libs = libs; }
	void SetLibPath(const wxArrayString &libPaths) { m_libPath = libPaths; }
	void SetName(const wxString &name){ m_name = name; }
	void SetCompilerRequired(bool required) { m_compilerRequired = required; }
	void SetLinkerRequired(bool required) { m_linkerRequired = required; }
	void SetOutputFileName(const wxString &name){ m_outputFile = name; }
	void SetIntermediateDirectory(const wxString &dir){ m_intermediateDirectory = dir; }
	void SetCommand(const wxString &cmd){ m_command = cmd; }
	void SetCommandArguments(const wxString &cmdArgs){ m_commandArguments = cmdArgs;}
	void SetWorkingDirectory(const wxString &dir){ m_workingDirectory = dir;}
};

typedef SmartPtr<BuildConfig> BuildConfigPtr;
#endif // BUILD_CONFIGURATION_H
