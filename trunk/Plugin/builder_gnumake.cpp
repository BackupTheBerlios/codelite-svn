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
	if(wspConf){
		std::list<ConfigMappingEntry> list = wspConf->GetMapping();
		std::list<ConfigMappingEntry>::iterator iter = list.begin();
		for(; iter != list.end(); iter++){
			wxString projectName = (*iter).m_project;
			wxString configName  = (*iter).m_name; 

			TrimString(configName);
			TrimString(projectName);

			ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(projectName, errMsg);
			if(proj){
				GenerateMakefile(proj);
			}
		}
		return true;
	}
	return false;
}


void BuilderGnuMake::GenerateMakefile(ProjectPtr proj)
{
	ProjectSettingsPtr settings = proj->GetSettings();
	wxString path = proj->GetFileName().GetPath();

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
		text << name << wxT("=") << value << wxT("") << wxT("\n");
	}
	text << wxT("\n");

	//----------------------------------------------------------
	// Create entry for each configuration type
	//----------------------------------------------------------
	ProjectSettingsCookie cookie;
	BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
	while(bldConf){
		CreateConfigsVariables(bldConf, text);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
	
	//-----------------------------------------------------------
	// Create a list of object that should be built according to 
	// projects' file list and create their targets as well
	//-----------------------------------------------------------
	CreateTargets(proj, text);

	//-----------------------------------------------------------
	// Create build events per configuration type
	//-----------------------------------------------------------
	bldConf = settings->GetFirstBuildConfiguration(cookie);
	while(bldConf){
		CreateBuildEventRules(bldConf, text);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
}

void BuilderGnuMake::CreateTargets(ProjectPtr proj, wxTextOutputStream &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files);

	text << wxT("Objects=");

	int counter = 1;
	for(size_t i=0; i<files.size(); i++){
		if( !IsSourceFile(files[i].GetExt()) )
			continue;

		text << wxT("$(IntermediateDirectory)/") << files[i].GetName() << wxT(".o ");
		if(counter % 10 == 0){
			text << wxT("\\\n\t");
		}
		counter++;
	}
	text << wxT("\n\n");

	//create the main target
	text << wxT("##\n");
	text << wxT("## Main Build Traget \n");
	text << wxT("##\n");
	text << wxT("Main: StartMsg $(PreBuild) $(Objects) $(PostBuild)\n");
	if(proj->GetType() == Project::STATIC_LIBRARY){
		//create a static library
		text << wxT("\t") << wxT("ar rcu $(OutputFile) $(Objects)\n");
	}else if(proj->GetType() == Project::DYNAMIC_LIBRARY){
		//create a shared library
		text << wxT("\t") << wxT("$(CompilerName) $(LinkOptions) -o $(OutputFile) $(Objects) $(LibPath) $(Libs)\n");
	}else if(proj->GetType() == Project::EXECUTABLE){
		//create an executable
		text << wxT("\t") << wxT("$(CompilerName) $(LinkOptions) -o $(OutputFile) $(Objects) $(LibPath) $(Libs)\n");
	}
	text << wxT("\n\n");

	//create a nice start message to user
	text << wxT("##\n");
	text << wxT("## Startup message \n");
	text << wxT("##\n");
	text << wxT("StartMsg:\n");
	text << wxT("\t@echo Building project: ") << proj->GetName() << wxT("\n");
	text << wxT("\n\n");

	//create rule per object
	text << wxT("##\n");
	text << wxT("## Objects\n");
	text << wxT("##\n");

	for(size_t i=0; i<files.size(); i++){
		if( !IsSourceFile(files[i].GetExt()) )
			continue;

		wxString objectName = files[i].GetName() << wxT(".o");
		wxString fileName   = files[i].GetFullPath();
		text << wxT("$(IntermediateDirectory)/") << objectName << wxT(": ") << fileName << wxT("\n");
		text << wxT("\t") << wxT("$(CompilerName) $(CmpOptions) -c ") << fileName << wxT(" -o ") << wxT("$(IntermediateDirectory)/") << objectName << wxT("\n\n");
	}

	//add clean target
	text << wxT("##\n");
	text << wxT("## Clean\n");
	text << wxT("##\n");
	text << wxT("clean:\n");
	text << wxT("\t$(RM) $(Objects)\n");
	text << wxT("\n");
}

void BuilderGnuMake::CreateBuildEventRules(BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	BuildCommandList cmds;
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	//generate prebuild commands
	text << wxT("## ") <<wxT("\n");
	text << wxT("## Build Events: (Post & Pre build commands)\n");
	text << wxT("## ") << wxT("\n");

	cmds.clear();
	text << wxT("PreBuild_") << name << wxT(":\n");
	text << wxT("\t@echo Executing Pre Build commands ...\n");
	bldConf->GetPreBuildCommands(cmds);
	BuildCommandList::iterator iter = cmds.begin();
	for(; iter != cmds.end(); iter++){
		if(iter->GetEnabled()){
			text << wxT("\t@") << iter->GetCommand() << wxT("\n");
		}
	}
	text << wxT("\t@echo Done\n\n");

	//generate postbuild commands
	cmds.clear();
	text << wxT("PostBuild_") << name << wxT(":\n");
	text << wxT("\t@echo Executing Post Build commands ...\n");
	bldConf->GetPostBuildCommands(cmds);
	iter = cmds.begin();
	for(; iter != cmds.end(); iter++){
		if(iter->GetEnabled()){
			text << wxT("\t@") << iter->GetCommand() << wxT("\n");
		}
	}
	text << wxT("\t@echo Done\n\n");
}

void BuilderGnuMake::CreateConfigsVariables(BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	text << wxT("## ") << name << wxT("\n");
	text << wxT("ifeq ($(type), ") << name << wxT(")") << wxT("\n");
	text << wxT("CompilerName") << wxT("=") << bldConf->GetCompilerName() << wxT("\n");
	text << wxT("OutputFile") << wxT("=") << bldConf->GetOutputFileName() << wxT("\n");
	text << wxT("IntermediateDirectory") << wxT("=") << bldConf->GetIntermediateDirectory() << wxT("\n");
	text << wxT("CmpOptions") << wxT("=") << bldConf->GetCompileOptions() << wxT("\n");
	text << wxT("LinkOptions") << wxT("=") << bldConf->GetLinkOptions() << wxT("\n");
	text << wxT("IncludePath=") << ParseIncludePath(bldConf->GetIncludePath()) << wxT("\n");
	text << wxT("Libs=") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");
	text << wxT("LibPath=") << ParseLibPath(bldConf->GetLibPath()) << wxT("\n");
	text << wxT("PostBuild=") << wxT("PostBuild_") << name << wxT("\n");
	text << wxT("PreBuild=") << wxT("PreBuild_") << name << wxT("\n");
	text << wxT("endif\n\n");

	//create the intermediate directories
	wxMkDir(bldConf->GetIntermediateDirectory());
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
		incluedPath << wxT("-I") << path << wxT(" ");
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
		libPath << wxT("-L") << path << wxT(" ");
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
		if(lib.StartsWith(wxT("lib"))){
			lib = lib.Mid(3);
		}
		//remove suffixes
		lib = lib.BeforeFirst(wxT('.'));
		slibs << wxT("-l") << lib << wxT(" ");
	}
	return slibs;
}
