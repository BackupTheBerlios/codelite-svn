#include "builder_gnumake.h"
#include "configuration_mapping.h"
#include "dirsaver.h"
#include "macros.h"
#include "wx/tokenzr.h"

BuilderGnuMake::BuilderGnuMake()
: Builder(wxT("GNU makefile for g++/gcc"))
{
}

BuilderGnuMake::~BuilderGnuMake()
{
}

bool BuilderGnuMake::BuildProject(const wxString &project, const wxString &target)
{

	return true;
}

bool BuilderGnuMake::BuildWorkspace(const wxString &target)
{
	return true;
}

bool BuilderGnuMake::Export(const wxString &project)
{
	wxString errMsg;
	//get the build matrix
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	
	//extract the selected configuration from
	wxString selConf = matrix->GetSelectedConfigurationName();
	WorkspaceConfigurationPtr wspConf = matrix->GetConfigurationByName(selConf);

	//iterate over the projects that should be built and create makefile
	//for each project
	std::list<ConfigMappingEntry> list = wspConf->GetMapping();
	std::list<ConfigMappingEntry>::iterator iter = list.begin();
	for(; iter != list.end(); iter++){
		wxString projectName = (*iter).m_project;
		wxString configName  = (*iter).m_name; 
		
		TrimString(configName);
		TrimString(projectName);

		ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
		if(proj){
			ProjectSettingsPtr settings = proj->GetSettings();
			if(settings){
				GenerateMakefile(settings, proj->GetFileName().GetPath());
			}
		}
	}
	return true;
}


void BuilderGnuMake::GenerateMakefile(ProjectSettingsPtr settings, const wxString &path)
{
	DirSaver ds;
	//change directory to the project base dir
	::wxSetWorkingDirectory(path);
	
	//create new makefile file
	wxString fn(path);
	fn << wxT("/makefile");
	wxFileOutputStream output(fn);

	if(!output.IsOk())
		return;

	//generate all configurations for this project
	wxTextOutputStream text(output);

	text << wxT("##") << wxT("\n");
	text << wxT("## Auto Generated makefile, please do not edit") << wxT("\n");;
	text << wxT("##") << wxT("\n");
	
	//----------------------------------------------------------
	//copy environment variables to the makefile
	//----------------------------------------------------------
	EnvironmentVarieblesPtr env = WorkspaceST::Get()->GetEnvironmentVariables();
	EnvironmentVariebles::ConstIterator iter = env->Begin();
	for(; iter != env->End(); iter++){
		wxString name = iter->first;
		wxString value = iter->second;
		text << name << wxT("=\"") << value << wxT("\"") << wxT("\n");
	}
	text << wxT("\n");

	//----------------------------------------------------------
	// Create entry for each configuration type
	//----------------------------------------------------------
	ProjectSettingsCookie cookie;
	BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
	while(bldConf){
		GenerateProjectConfiguration(bldConf, text);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
}

void BuilderGnuMake::GenerateProjectConfiguration(BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	text << wxT("## ") << name << wxT("\n");
	text << wxT("ifeq ($(type), ") << name << wxT(")") << wxT("\n");
	text << wxT("OutputFile") << wxT("=\"") << bldConf->GetOutputFileName() << wxT("\"\n");
	text << wxT("IntermediateDirectory") << wxT("=\"") << bldConf->GetIntermediateDirectory() << wxT("\"\n");
	text << wxT("CmpOptions") << wxT("=\"") << bldConf->GetCompileOptions() << wxT("\"\n");
	text << wxT("LinkOptions") << wxT("=\"") << bldConf->GetLinkOptions() << wxT("\"\n");
	text << wxT("IncludePath=") << ParseIncludePath(bldConf->GetIncludePath()) << wxT("\n");
	text << wxT("Libs=") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");
	text << wxT("LibPath=") << ParseLibPath(bldConf->GetLibPath()) << wxT("\n");
	text << wxT("PostBuild=") << wxT("PostBuild_") << name << wxT("\n");
	text << wxT("endif\n\n");
}

wxString BuilderGnuMake::ParseIncludePath(const wxString &paths)
{
	//convert semi-colon delimited string into GNU list of
	//include paths:
	wxString incluedPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"));
	//prepend each include path with -I
	while(tkz.HasMoreTokens()){
		wxString path(tkz.NextToken());
		TrimString(path);
		incluedPath << wxT("-I\"") << path << wxT("\" ");
	}
	return incluedPath;
}

wxString BuilderGnuMake::ParseLibPath(const wxString &paths)
{
	//convert semi-colon delimited string into GNU list of
	//lib path
	wxString libPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"));
	//prepend each include path with -L
	while(tkz.HasMoreTokens()){
		wxString path(tkz.NextToken());
		TrimString(path);
		libPath << wxT("-L\"") << path << wxT("\" ");
	}
	return libPath;
}

wxString BuilderGnuMake::ParseLibs(const wxString &libs)
{
	//convert semi-colon delimited string into GNU list of
	//libs
	wxString slibs(wxEmptyString);
	wxStringTokenizer tkz(libs, wxT(";"));
	//prepend each include path with -l and strip trailing lib string
	//also, if the file contains an extension (.a, .so, .dynlib) remove them as well
	while(tkz.HasMoreTokens()){
		wxString lib(tkz.NextToken());
		TrimString(lib);
		//remove lib prefix
		if(lib.StartsWith("lib")){
			lib = lib.Mid(3);
		}
		//remove suffixes
		lib = lib.BeforeFirst(wxT('.'));
		slibs << wxT("-l") << lib << wxT(" ");
	}
	return slibs;
}
