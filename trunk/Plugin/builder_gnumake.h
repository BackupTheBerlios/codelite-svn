#ifndef BUILDER_GNUMAKE_H
#define BUILDER_GNUMAKE_H

#include "builder.h"
#include "workspace.h"
#include <wx/wfstream.h>
#include <wx/txtstrm.h>

class BuilderGnuMake : public Builder 
{
public:
	BuilderGnuMake();
	virtual ~BuilderGnuMake();

	virtual bool BuildProject(const wxString &project, const wxString &target);
	virtual bool BuildWorkspace(const wxString &target);

	/**
	 * Export the build system specific file (e.g. GNU makefile, Ant file etc) 
	 * to allow users to invoke them manualy from the command line
	 * \param project project to export. If left empty, the file is exported for the whole
	 *			workspace. Anyways, the file will always include its dependencies
	 * \return true on success, false otherwise.
	 */
	virtual bool Export(const wxString &project = wxEmptyString);

private:
	void GenerateMakefile(ProjectPtr proj);
	void CreateConfigsVariables(BuildConfigPtr bldConf, wxTextOutputStream &text);
	void CreateFileTargets(ProjectPtr proj, wxTextOutputStream &text);
	void CreateObjectList(ProjectPtr proj, wxTextOutputStream &text);
	void CreateTargets(BuildConfigPtr bldConf, wxTextOutputStream &text);
	void CreateBuildEventRules(BuildConfigPtr bldConf, wxTextOutputStream &text);

	wxString ParseIncludePath(const wxString &paths);
	wxString ParseLibPath(const wxString &paths);
	wxString ParseLibs(const wxString &libs);
	wxString ParsePreprocessor(const wxString &prep);
};
#endif // BUILDER_GNUMAKE_H
