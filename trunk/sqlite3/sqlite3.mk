##
## Auto Generated makefile, please do not edit
##
WXSTABLE=C:/wxWidgets-2.8.4
WorkspaceName=LiteEditor
WorkspacePath=C:\Development\C++\LiteEditor

## Debug
ifeq ($(type), Debug)
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
OutputFile=..\..\lib\gcc\libsqlite3.a
IntermediateDirectory=.\Debug
Preprocessors=
CmpOptions=-g $(Preprocessors)
LinkOptions=-O0
IncludePath=$(IncludeSwitch). 
Libs=
LibPath=$(LibraryPathSwitch). $(LibraryPathSwitch).\Debug 
Target=Debug
endif

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

Objects=$(IntermediateDirectory)\sqlite3$(ObjectSuffix) 

##
## Main Build Tragets 
##
BuildTarget: $(Target)

Debug: StartMsg PreBuild_Debug $(Objects) Debug_link PostBuild_Debug

Debug_link:
	$(ArchiveTool) $(OutputFile) $(Objects)


ReleaseANSI: StartMsg PreBuild_ReleaseANSI $(Objects) ReleaseANSI_link PostBuild_ReleaseANSI

ReleaseANSI_link:
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
PreBuild_Debug:

PostBuild_Debug:

## 
## Build Events: (Post & Pre build commands)
## 
PreBuild_ReleaseANSI:

PostBuild_ReleaseANSI:

