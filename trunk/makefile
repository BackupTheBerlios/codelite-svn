##
## CodeLite library makefile
## Author: Eran Ifrah
## 
## This makefile was created to build CodeLite, LiteEditor and its dependencies,
## for the g++ compiler. It was tested using gmake & GCC 4.1
## Testted on Ubunto Dapper, Fedora core 4 & 5
##
## Feel free to modify it for your own use
##
## Read BuildInfo.txt for more information
 
## 
## Set default build set
## as ANSI-deubg
##
WXCFG=--unicode=no --debug=yes 
EXT=d
OBJ_DIR=Debug_gcc
DEBUG= -g 
EXE_NAME=LiteEditorD

##
## Override default settings by typing: make type=[release | release_unicode | debug_unicode]
##
ifeq ($(type), release)
EXE_NAME=LiteEditorR
WXCFG=--unicode=no  --debug=no
EXT=
OBJ_DIR=Release_gcc
OPT=-O3
DEBUG=
endif

ifeq ($(type), release_unicode)
EXE_NAME=LiteEditorUR
WXCFG=--unicode=yes --debug=no
EXT=u
OBJ_DIR=Release_gcc_unicode
OPT=-O3 -DREGEXP_UNICODE
DEBUG=
endif

ifeq ($(type), debug_unicode)
EXE_NAME=LiteEditorUD
WXCFG=--unicode=yes --debug=yes
EXT=ud
OBJ_DIR=Debug_gcc_unicode
DEBUG= -g 
OPT= -DREGEXP_UNICODE
endif

WXVER=26
OUTPUT_DIR=lib

#PROFILER= -pg 
##
## Define variables, using wx-config tool
##

CMP=g++ $(DEBUG) $(OPT) 
LIBP=-L../lib

WXLIBS= $(shell wx-config --libs std $(WXCFG)) -L/usr/X11R6/lib -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 
TRACE_FLAG=## Comment this if you dont want messages in log window
CCFLAGS= -D__WX__ -Wall $(TRACE_FLAG) -I. -DWXUSINGDLL -DWX_PRECOMP -DNO_GCC_PRAGMA -DXTHREADS -D_REENTRANT -DXUSE_MTSAFE_API -pthread $(shell wx-config --cxxflags $(WXCFG)) $(PROFILER) -fno-strict-aliasing -pthread -DYY_NEVER_INTERACTIVE=1

LINK_FLAGS =  $(WXLIBS) -L$(OUTPUT_DIR) -Lsdk/wxflatnotebook/lib -lwxflatnotebook$(EXT) -lcodelite$(EXT) -lplugin$(EXT) -lwxsqlite3$(EXT) -lwxscintilla$(EXT) -Lsdk/sqlite3/lib -lsqlite3 $(PROFILER) -pthread  -lz -ldl -lm  -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lfontconfig -lXext -lXrender -lXinerama -lXi -lXrandr -lXcursor -lXfixes -lX11 -lgobject-2.0 -lgmodule-2.0 -ldl -lgthread-2.0 -lglib-2.0 -lXinerama -lpng -lz

SCI_INCLUDE= -Isdk/wxscintilla/include -Isdk/wxscintilla/src/scintilla/include	 -Isdk/wxscintilla/src/scintilla/src
SQLITE_INCLUDE= -Isdk/wxsqlite3/include -Isdk/wxsqlite3/sqlite3/include
INCLUDES = -I. $(SQLITE_INCLUDE) $(SCI_INCLUDE) -IPlugin -ICodeLite -ILiteEditor -ILiteEditor/include/ -Isdk/wxflatnotebook/include 

##
## Define the object files
##

lib_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.cpp)))))
lib_c_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.c)))))
sample_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard LiteEditor/*.cpp)))))
precompiled_headers := precompiled_header.h.gch

## 
## Scintilla related
## 
SCI_DEFS = -DGTK -DSCI_LEXER -DLINK_LEXERS
SCI_OBJ_DIR=$(OBJ_DIR)_scintilla
sci_objects1 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/scintilla/src/*.cxx)))))
sci_objects2 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/*.cpp)))))
sci_objects = $(sci_objects1) $(sci_objects2) 

##
## SQLite related
##
SQLITE_OBJ_DIR=$(OBJ_DIR)_sqlite3
sqlite_objects2 := $(addprefix $(SQLITE_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxsqlite3/src/*.cpp)))))
sqlite_objects = $(sqlite_objects1) $(sqlite_objects2) 

## our main build target
build : pre_build pre_comp CodeLite Scintilla SQLite wxFlatNotebook SDK LiteEditor ctags 

CodeLite: code_lite_msg $(lib_cpp_objects) $(lib_c_objects)
	ar rcu $(OUTPUT_DIR)/libcodelite$(EXT).a $(lib_cpp_objects)  $(lib_c_objects) 
	ranlib $(OUTPUT_DIR)/libcodelite$(EXT).a

Scintilla: scintilla_msg $(sci_objects)
	ar rcu $(OUTPUT_DIR)/libwxscintilla$(EXT).a $(sci_objects)
	ranlib $(OUTPUT_DIR)/libwxscintilla$(EXT).a
	
SQLite: sqlite_msg $(sqlite_objects)
	ar rcu $(OUTPUT_DIR)/libwxsqlite3$(EXT).a $(sqlite_objects)
	ranlib $(OUTPUT_DIR)/libwxsqlite3$(EXT).a
	
wxFlatNotebook: wxFlatNotebook_msg
	cd sdk/wxflatnotebook/build/wxFlatNotebook && $(MAKE)

SDK: sdk_msg
	cd Plugin/ && $(MAKE)

ctags: ctags_msg
	cd sdk/ctags/ && $(MAKE)
	
LiteEditor: lite_editor_msg $(precompiled_headers) $(sample_cpp_objects) 
	$(CMP) -o Runtime/$(EXE_NAME) $(sample_cpp_objects) $(LINK_FLAGS)

$(OBJ_DIR)/%.o: CodeLite/%.cpp 
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

$(OBJ_DIR)/%.o: CodeLite/%.c
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

$(OBJ_DIR)/%.o: LiteEditor/%.cpp 
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

$(SCI_OBJ_DIR)/%.o: sdk/wxscintilla/src/%.cpp 
	$(CMP) $(CCFLAGS) $(SCI_DEFS) $(INCLUDES) -c $< -o $(SCI_OBJ_DIR)/$(@F)

$(SCI_OBJ_DIR)/%.o: sdk/wxscintilla/src/scintilla/src/%.cxx 
	$(CMP) $(CCFLAGS) $(SCI_DEFS) $(INCLUDES) -c $< -o $(SCI_OBJ_DIR)/$(@F)

$(SQLITE_OBJ_DIR)/%.o: 	sdk/sqlite3/%.c		
	$(CMP) $(CCFLAGS) -DNO_TCL $(INCLUDES) -c $< -o $(SQLITE_OBJ_DIR)/$(@F)

$(SQLITE_OBJ_DIR)/%.o: 	sdk/wxsqlite3/src/%.cpp		
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(SQLITE_OBJ_DIR)/$(@F)

pre_comp: $(precompiled_headers) 

precompiled_header.h.gch: CodeLite/precompiled_header.h
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(@F)
		
pre_build:
	test -d  $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d  $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)
	test -d  $(SCI_OBJ_DIR) || mkdir $(SCI_OBJ_DIR)
	test -d  $(SQLITE_OBJ_DIR) || mkdir $(SQLITE_OBJ_DIR)

ctags_msg:
	@echo ----------------------------------------------------------
	@echo Building ctags-le 
	@echo ---------------------------------------------------------- 

code_lite_msg:
	@echo ----------------------------------------------------------
	@echo Building CodeLite library
	@echo ---------------------------------------------------------- 

sdk_msg:
	@echo ----------------------------------------------------------
	@echo Building SDK library
	@echo ---------------------------------------------------------- 

sqlite_msg:
	@echo ----------------------------------------------------------
	@echo Building SQLite library
	@echo ---------------------------------------------------------- 

scintilla_msg:
	@echo ----------------------------------------------------------
	@echo Building Scintilla library
	@echo ---------------------------------------------------------- 
	
lite_editor_msg:
	@echo ----------------------------------------------------------
	@echo Building LiteEditor program 
	@echo ---------------------------------------------------------- 
		
wxFlatNotebook_msg:
	@echo ----------------------------------------------------------
	@echo Building wxFlatNotebook library
	@echo ---------------------------------------------------------- 
		
clean:
	$(RM) Debug_gcc/*.o Release_gcc/*.o Release_gcc_unicode/*. Debug_gcc_unicode/*.o lib/*.o lib/*.a
	$(RM) Debug_gcc_sqlite3/*.o Release_gcc_sqlite3/*.o Release_gcc_unicode_sqlite3/*. Debug_gcc_unicode_sqlite3/*.o lib/*.o lib/*.a
	$(RM) Debug_gcc_scintilla/*.o Release_gcc_scintilla/*.o Release_gcc_unicode_scintilla/*. Debug_gcc_unicode_sinctilla/*.o lib/*.o lib/*.a
	$(RM) -R Debug_gcc Release_gcc Release_gcc_unicode Debug_gcc_unicode Debug_gcc_sqlite3 Release_gcc_sqlite3 Release_gcc_unicode_sqlite3 Debug_gcc_unicode_sqlite3 Debug_gcc_scintilla Release_gcc_scintilla Release_gcc_unicode_scintilla Debug_gcc_unicode_scintilla	
	$(RM) precompiled_header.h.gch
	cd sdk/wxflatnotebook/build/wxFlatNotebook/ && $(MAKE) clean
	cd Plugin/ && $(MAKE) clean

install:
	@echo installing LiteEditor
	@\rm -f /usr/local/bin/LiteEditor
	@echo copying Runtime/$(EXE_NAME) to /usr/local/bin/
	@cp Runtime/$(EXE_NAME) /usr/local/bin/
	@ln -s /usr/local/bin/$(EXE_NAME) /usr/local/bin/LiteEditor
	@echo cp Runtime/le_dos2unix.sh /usr/local/bin/
	@cp Runtime/le_dos2unix.sh /usr/local/bin/
	@echo copying ctags-le to /usr/local/bin
	@cp sdk/ctags/ctags-le /usr/local/bin/
	@echo $(HOME)
	@mkdir -p $(HOME)/".liteeditor/config/"
	@mkdir -p $(HOME)/".liteeditor/rc/"
	@mkdir -p $(HOME)/".liteeditor/templates/"
	@mkdir -p $(HOME)/".liteeditor/lexers/"
	@\cp -pr Runtime/config/*.xml $(HOME)/".liteeditor/config/"
	@\cp -pr Runtime/rc/*.xrc $(HOME)/".liteeditor/rc/"
	@\cp -pr Runtime/templates/*.project $(HOME)/".liteeditor/templates/"
	@\cp -pr Runtime/lexers/*.xml $(HOME)/".liteeditor/lexers/"
	@chmod -R 777 $(HOME)/".liteeditor"
