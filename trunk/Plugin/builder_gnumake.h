#ifndef BUILDER_GNUMAKE_H
#define BUILDER_GNUMAKE_H

#include "builder.h"
#include "workspace.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK BuilderGnuMake : public Builder 
{
public:
	BuilderGnuMake();
	virtual ~BuilderGnuMake();

	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. 
	 * \param errMsg output
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project, wxString &errMsg);
	virtual wxString GetBuildCommand(const wxString &project);
	virtual wxString GetCleanCommand(const wxString &project);

private:
	void GenerateMakefile(ProjectPtr proj);
	void CreateConfigsVariables(BuildConfigPtr bldConf, wxTextOutputStream &text);
	void CreateFileTargets(ProjectPtr proj, wxTextOutputStream &text);
	void CreateObjectList(ProjectPtr proj, wxTextOutputStream &text);
	void CreateTargets(const wxString &type, BuildConfigPtr bldConf, wxTextOutputStream &text);
	void CreateBuildEventRules(BuildConfigPtr bldConf, wxTextOutputStream &text);

	wxString ParseIncludePath(const wxString &paths);
	wxString ParseLibPath(const wxString &paths);
	wxString ParseLibs(const wxString &libs);
	wxString ParsePreprocessor(const wxString &prep);
};
#endif // BUILDER_GNUMAKE_H
