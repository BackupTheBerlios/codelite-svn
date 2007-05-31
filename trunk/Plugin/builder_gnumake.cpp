#include "project.h"
#include "builder_gnumake.h"
#include "configuration_mapping.h"
#include "dirsaver.h"
#include "wx/tokenzr.h"
#include "editor_config.h"
#include "macros.h"

BuilderGnuMake::BuilderGnuMake()
: Builder(wxT("GNU makefile for g++/gcc"))
{
}

BuilderGnuMake::~BuilderGnuMake()
{
}

bool BuilderGnuMake::Export(const wxString &project, wxString &errMsg)
{
	if(project.IsEmpty()){
		return false;
	}
	
	ProjectPtr proj = WorkspaceST::Get()->FindProjectByName(project, errMsg);
	wxArrayString depsArr = proj->GetDependencies();
	//iterate over the dependencies projects and generate makefile
	for(size_t i=0; i<depsArr.GetCount(); i++){
		ProjectPtr dependProj = WorkspaceST::Get()->FindProjectByName(depsArr.Item(i), errMsg);
		GenerateMakefile(dependProj);
	}
	//generate makefile for the project itself
	GenerateMakefile(proj);
	return true;
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
	fn << PATH_SEP << wxT("makefile");
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
	
	// create a list of objects
	CreateObjectList(proj, text);

	//-----------------------------------------------------------
	// create the build targets
	//-----------------------------------------------------------
	bldConf = settings->GetFirstBuildConfiguration(cookie);
	while(bldConf){
		CreateTargets(settings->GetProjectType(), bldConf, text);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
	
	//-----------------------------------------------------------
	// Create a list of targets that should be built according to 
	// projects' file list 
	//-----------------------------------------------------------
	CreateFileTargets(proj, text);

	//-----------------------------------------------------------
	// Create build events per configuration type
	//-----------------------------------------------------------
	bldConf = settings->GetFirstBuildConfiguration(cookie);
	while(bldConf){
		CreateBuildEventRules(bldConf, text);
		bldConf = settings->GetNextBuildConfiguration(cookie);
	}
}
void BuilderGnuMake::CreateObjectList(ProjectPtr proj, wxTextOutputStream &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files);

	text << wxT("Objects=");

	int counter = 1;
	for(size_t i=0; i<files.size(); i++){
		if( !IsSourceFile(files[i].GetExt()) )
			continue;

		text << wxT("$(IntermediateDirectory)") << PATH_SEP << files[i].GetName() << wxT("$(ObjectSuffix) ");
		if(counter % 10 == 0){
			text << wxT("\\\n\t");
		}
		counter++;
	}
	text << wxT("\n\n");
}

void BuilderGnuMake::CreateFileTargets(ProjectPtr proj, wxTextOutputStream &text)
{
	std::vector<wxFileName> files;
	proj->GetFiles(files);
	
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

		wxString objectName = files[i].GetName() << wxT("$(ObjectSuffix)");
		wxString fileName   = files[i].GetFullPath();
		text << wxT("$(IntermediateDirectory)") << PATH_SEP << objectName << wxT(": ") << fileName << wxT("\n");
		text << wxT("\t") << wxT("$(CompilerName) $(CmpOptions) $(SourceSwitch) ") << fileName << wxT(" $(OutputSwitch) ") << wxT("$(IntermediateDirectory)") << PATH_SEP << objectName << wxT("\n\n");
	}

	//add clean target
	text << wxT("##\n");
	text << wxT("## Clean\n");
	text << wxT("##\n");
	text << wxT("clean:\n");

	if(wxGetOsVersion() & wxOS_WINDOWS){
		//windows clean command
		for(size_t i=0; i<files.size(); i++){
			if( !IsSourceFile(files[i].GetExt()) )
				continue;

			wxString objectName = files[i].GetName() << wxT("$(ObjectSuffix)");
			text << wxT("\t") << wxT("-if exist ") << wxT("$(IntermediateDirectory)") << PATH_SEP << objectName << wxT(" del ") << wxT("$(IntermediateDirectory)") << PATH_SEP << objectName << wxT("\n");
		}
		//delete the output file as well
		text << wxT("\t") << wxT("-if exist ") << wxT("$(OutputFile)") << wxT(" del ") << wxT("$(OutputFile)") << wxT("\n");;
	}else{
		//linux 
		for(size_t i=0; i<files.size(); i++){
			if( !IsSourceFile(files[i].GetExt()) )
				continue;

			wxString objectName = files[i].GetName() << wxT("$(ObjectSuffix)");
			text << wxT("\t") << wxT("$(RM) ") << wxT("$(IntermediateDirectory)") << PATH_SEP << objectName << wxT("\n");
		}
		//delete the output file as well
		text << wxT("\t") << wxT("$(RM) ") << wxT("$(OutputFile)\n");
	}
	text << wxT("\n");
}

void BuilderGnuMake::CreateTargets(const wxString &type, BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	//create the main target
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	text << wxT("##\n");
	text << wxT("## Main Build Tragets \n");
	text << wxT("##\n");
	text << name << wxT(": StartMsg PreBuild_") << name << wxT(" ") << wxT("$(Objects) ") << name << wxT("_link ") << wxT("PostBuild_") << name << wxT("\n\n");
	text << name << wxT("_link:\n");
	if(type == Project::STATIC_LIBRARY){
		//create a static library
		text << wxT("\t") << wxT("$(ArchiveTool) $(OutputFile) $(Objects)\n");
	}else if(type == Project::DYNAMIC_LIBRARY){
		//create a shared library
		text << wxT("\t") << wxT("$(SharedObjectLinkerName) $(LinkOptions) $(OutputSwitch) $(OutputFile) $(Objects) $(LibPath) $(Libs)\n");
	}else if(type == Project::EXECUTABLE){
		//create an executable
		text << wxT("\t") << wxT("$(LinkerName) $(LinkOptions) $(OutputSwitch) $(OutputFile) $(Objects) $(LibPath) $(Libs)\n");
	}
	text << wxT("\n\n");
}

void BuilderGnuMake::CreateBuildEventRules(BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	BuildCommandList cmds;
	BuildCommandList::iterator iter;
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);

	//generate prebuild commands
	text << wxT("## ") <<wxT("\n");
	text << wxT("## Build Events: (Post & Pre build commands)\n");
	text << wxT("## ") << wxT("\n");

	cmds.clear();
	bldConf->GetPreBuildCommands(cmds);
	text << wxT("PreBuild_") << name << wxT(":\n");
	if(!cmds.empty()){
		text << wxT("\t@echo Executing Pre Build commands ...\n");
		iter = cmds.begin();
		for(; iter != cmds.end(); iter++){
			if(iter->GetEnabled()){
				text << wxT("\t") << iter->GetCommand() << wxT("\n");
			}
		}
		text << wxT("\t@echo Done\n");
	}
	text << wxT("\n");

	//generate postbuild commands
	cmds.clear();
	bldConf->GetPostBuildCommands(cmds);
	
	text << wxT("PostBuild_") << name << wxT(":\n");
	if(!cmds.empty()){
		text << wxT("\t@echo Executing Post Build commands ...\n");
		iter = cmds.begin();
		for(; iter != cmds.end(); iter++){
			if(iter->GetEnabled()){
				text << wxT("\t") << iter->GetCommand() << wxT("\n");
			}
		}
		text << wxT("\t@echo Done\n");
	}
	text << wxT("\n");
}	

void BuilderGnuMake::CreateConfigsVariables(BuildConfigPtr bldConf, wxTextOutputStream &text)
{
	wxString name = bldConf->GetName();
	name = NormalizeConfigName(name);
	
	wxString cmpType = bldConf->GetCompilerType();
	CompilerPtr cmp = EditorConfigST::Get()->GetCompiler(cmpType);

	text << wxT("## ") << name << wxT("\n");
	text << wxT("ifeq ($(type), ") << name << wxT(")") << wxT("\n");
	text << wxT("LinkerName=") << cmp->GetTool(wxT("LinkerName")) << wxT("\n");
	text << wxT("ArchiveTool=") << cmp->GetTool(wxT("ArchiveTool")) << wxT("\n");
	text << wxT("SharedObjectLinkerName=") << cmp->GetTool(wxT("SharedObjectLinkerName")) << wxT("\n");
	text << wxT("ObjectSuffix=") << cmp->GetObjectSuffix() << wxT("\n");
	text << wxT("DebugSwitch=") << cmp->GetSwitch(wxT("Debug")) << wxT("\n");
	text << wxT("IncludeSwitch=") << cmp->GetSwitch(wxT("Include")) << wxT("\n");
	text << wxT("LibrarySwitch=") << cmp->GetSwitch(wxT("Library")) << wxT("\n");
	text << wxT("OutputSwitch=") << cmp->GetSwitch(wxT("Output")) << wxT("\n");
	text << wxT("LibraryPathSwitch=") << cmp->GetSwitch(wxT("LibraryPath")) << wxT("\n");
	text << wxT("PreprocessorSwitch=") << cmp->GetSwitch(wxT("Preprocessor")) << wxT("\n");
	text << wxT("SourceSwitch=") << cmp->GetSwitch(wxT("Source")) << wxT("\n");
	text << wxT("CompilerName") << wxT("=") << cmp->GetTool(wxT("CompilerName")) << wxT("\n");
	text << wxT("OutputFile") << wxT("=") << bldConf->GetOutputFileName() << wxT("\n");
	text << wxT("IntermediateDirectory") << wxT("=") << bldConf->GetIntermediateDirectory() << wxT("\n");
	text << wxT("Preprocessors=") << ParsePreprocessor(bldConf->GetPreprocessor()) << wxT("\n");
	text << wxT("CmpOptions") << wxT("=") << bldConf->GetCompileOptions() << wxT(" $(Preprocessors)") << wxT("\n");
	text << wxT("LinkOptions") << wxT("=") << bldConf->GetLinkOptions() << wxT("\n");
	text << wxT("IncludePath=") << ParseIncludePath(bldConf->GetIncludePath()) << wxT("\n");
	text << wxT("Libs=") << ParseLibs(bldConf->GetLibraries()) << wxT("\n");
	text << wxT("LibPath=") << ParseLibPath(bldConf->GetLibPath()) << wxT("\n");
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
		incluedPath << wxT("$(IncludeSwitch)") << path << wxT(" ");
	}
	return incluedPath;
}

wxString BuilderGnuMake::ParseLibPath(const wxString &paths)
{
	//convert semi-colon delimited string into GNU list of
	//lib path
	wxString libPath(wxEmptyString);
	wxStringTokenizer tkz(paths, wxT(";"));
	//prepend each include path with libpath switch
	while(tkz.HasMoreTokens()){
		wxString path(tkz.NextToken());
		TrimString(path);
		libPath << wxT("$(LibraryPathSwitch)") << path << wxT(" ");
	}
	return libPath;
}

wxString BuilderGnuMake::ParsePreprocessor(const wxString &prep)
{
	wxString preprocessor(wxEmptyString);
	wxStringTokenizer tkz(prep, wxT(";"));
	//prepend each include path with libpath switch
	while(tkz.HasMoreTokens()){
		wxString p(tkz.NextToken());
		TrimString(p);
		preprocessor << wxT("$(PreprocessorSwitch)") << p << wxT(" ");
	}
	return preprocessor;
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
		slibs << wxT("$(LibrarySwitch)") << lib << wxT(" ");
	}
	return slibs;
}

wxString BuilderGnuMake::GetBuildCommand(const wxString &project)
{
	wxString errMsg, cmd;
	//generate the makefile
	Export(project, errMsg);
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
	//TODO:: replace the hardcoded mingw32-make with a configurable value 
	cmd << wxT("mingw32-make ") << wxT("type=") << type;
	return cmd;
}

wxString BuilderGnuMake::GetCleanCommand(const wxString &project)
{
	wxString errMsg, cmd;
	//generate the makefile
	Export(project, errMsg);
	BuildMatrixPtr matrix = WorkspaceST::Get()->GetBuildMatrix();
	wxString type = Builder::NormalizeConfigName(matrix->GetSelectedConfigurationName());
	//TODO:: replace the hardcoded mingw32-make with a configurable value 
	cmd << wxT("mingw32-make ") << wxT("type=") << type << wxT(" clean");
	return cmd;
}
