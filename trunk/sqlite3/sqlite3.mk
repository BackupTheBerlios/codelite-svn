##
## Auto Generated makefile, please do not edit
##
WXSTABLE=C:/wxWidgets-2.8.4
WorkspaceName=LiteEditor
WorkspacePath=C:\Development\C++\LiteEditor

## ReleaseANSI
ifeq ($(type), ReleaseANSI)
LinkerName=gcc
ArchiveTool=ar rcu
SharedObjectLinkerName=gcc
ObjectSuffix=.o
DebugSwitch=-g
IncludeSwitch=-I
LibrarySwitch=-l
OutputSwitch=-o
LibraryPathSwitch=-L
PreprocessorSwitch=-D
SourceSwitch=-c
CompilerName=gcc
OutputFile=..\lib\gcc\libsqlite3.a
IntermediateDirectory=.\Release
Preprocessors=
CmpOptions=-O2 $(Preprocessors)
LinkOptions=-O2
IncludePath=$(IncludeSwitch). 
Libs=
LibPath=$(LibraryPathSwitch). $(LibraryPathSwitch).\Release 
Target=ReleaseANSI
endif

## ReleaseANSIStaticLibrary
ifeq ($(type), ReleaseANSIStaticLibrary)
LinkerName=gcc
ArchiveTool=ar rcu
SharedObjectLinkerName=gcc
ObjectSuffix=.o
DebugSwitch=-g
IncludeSwitch=-I
LibrarySwitch=-l
OutputSwitch=-o
LibraryPathSwitch=-L
PreprocessorSwitch=-D
SourceSwitch=-c
CompilerName=gcc
OutputFile=..\lib\gcc_lib\libsqlite3.a
IntermediateDirectory=.\Release_static
Preprocessors=
CmpOptions=-O2 $(Preprocessors)
LinkOptions=
IncludePath=$(IncludeSwitch). 
Libs=
LibPath=
Target=ReleaseANSIStaticLibrary
endif

Objects=$(IntermediateDirectory)\sqlite3$(ObjectSuffix) 

##
## Main Build Tragets 
##
BuildTarget: $(Target)

ReleaseANSI: StartMsg PreBuild_ReleaseANSI $(Objects) ReleaseANSI_link PostBuild_ReleaseANSI

ReleaseANSI_link:
	$(ArchiveTool) $(OutputFile) $(Objects)


ReleaseANSIStaticLibrary: StartMsg PreBuild_ReleaseANSIStaticLibrary $(Objects) ReleaseANSIStaticLibrary_link PostBuild_ReleaseANSIStaticLibrary

ReleaseANSIStaticLibrary_link:
	$(ArchiveTool) $(OutputFile) $(Objects)


##
## Startup message 
##
StartMsg:
	@echo ----------Building project:[ sqlite3 ]----------


##
## Objects
##
$(IntermediateDirectory)\sqlite3$(ObjectSuffix): sqlite3.c
	$(CompilerName) $(SourceSwitch) sqlite3.c $(CmpOptions)   $(OutputSwitch) $(IntermediateDirectory)\sqlite3$(ObjectSuffix) $(IncludePath) 

##
## Clean
##
clean:
	-if exist $(IntermediateDirectory)\sqlite3$(ObjectSuffix) del $(IntermediateDirectory)\sqlite3$(ObjectSuffix)
	-if exist $(OutputFile) del $(OutputFile)
	-if exist $(OutputFile) del $(OutputFile)

## 
## Build Events: (Post & Pre build commands)
## 
PreBuild_ReleaseANSI:

PostBuild_ReleaseANSI:

## 
## Build Events: (Post & Pre build commands)
## 
PreBuild_ReleaseANSIStaticLibrary:

PostBuild_ReleaseANSIStaticLibrary:

