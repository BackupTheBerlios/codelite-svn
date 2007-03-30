##
## CodeLite library makefile
## Author: Eran Ifrah
## 
## This makefile was created to build CodeLite, LiteEditor and its dependencies,
## for the g++ compiler. It was tested using MinGW \ GCC 3.4.5
##
## Feel free to modify it for your own use
##
## Read BuildInfo.txt for more information
 
ifeq ($(type), debug)
WXCFG=gcc_lib\mswd
EXT=d
OBJ_DIR=Debug_gcc
DEBUG= -g 
endif

ifeq ($(type), release)
EXT=
WXCFG=gcc_lib\msw
OBJ_DIR=Release_gcc
OPT=-O2
endif

ifeq ($(type), release_unicode)
EXT=u
WXCFG=gcc_lib\mswu
OBJ_DIR=Release_gcc_unicode
OPT=-O2
endif

ifeq ($(type), debug_unicode)
EXT=ud
WXCFG=gcc_lib\mswud
OBJ_DIR=Debug_gcc_unicode
DEBUG= -g 
endif

WXVER=26
OUTPUT_DIR=lib
##
## Define variables, using wx-config tool
##

CMP=g++ $(DEBUG)
LIBP=-L..\lib

CCFLAGS= $(OPT) $(shell wx-config --cflags --wxcfg=$(WXCFG)) 
WXLIBS=$(shell wx-config --libs --wxcfg=$(WXCFG))

LINK_FLAGS =  $(WXLIBS) -L$(OUTPUT_DIR) -lcodelite$(EXT) -lwxscintilla$(EXT) -lwxsqlite3$(EXT) -Lsdk/sqlite3/lib -lsqlite3   -lwxregexud
SCI_INCLUDE= -Isdk/wxscintilla/include -Isdk/wxscintilla/src/scintilla/include	 -Isdk/wxscintilla/src/scintilla/src
SQLITE_INCLUDE= -Isdk/wxsqlite3/include -Isdk/sqlite3
INCLUDES = -I. $(SQLITE_INCLUDE) $(SCI_INCLUDE) -ICodeLite -ILiteEditor -Isdk/include

##
## Define the object files
##

lib_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.cpp)))))
lib_c_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard CodeLite/*.c)))))
sample_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard LiteEditor/*.cpp)))))
resource_file =  $(OBJ_DIR)/code_parser.o

## 
## Scintilla related
## 
SCI_DEFS = -D__WX__=1 -DSCI_LEXER -DLINK_LEXERS
SCI_OBJ_DIR=$(OBJ_DIR)_scintilla
sci_objects1 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/scintilla/src/*.cxx)))))
sci_objects2 := $(addprefix $(SCI_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxscintilla/src/*.cpp)))))
sci_objects = $(sci_objects1) $(sci_objects2) 

##
## SQLite related
##
SQLITE_OBJ_DIR=$(OBJ_DIR)_sqlite3
## sqlite_objects1 := $(addprefix $(SQLITE_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/sqlite3/*.c)))))
sqlite_objects2 := $(addprefix $(SQLITE_OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard sdk/wxsqlite3/src/*.cpp)))))
sqlite_objects = $(sqlite_objects1) $(sqlite_objects2) 

## our main build target
build : pre_build CodeLite Scintilla SQLite LiteEditor

CodeLite: code_lite_msg $(precompiled_headers) $(lib_cpp_objects) $(lib_c_objects)
	ar rcu $(OUTPUT_DIR)\libcodelite$(EXT).a $(lib_cpp_objects)  $(lib_c_objects) 
	ranlib $(OUTPUT_DIR)\libcodelite$(EXT).a

Scintilla: scintilla_msg $(sci_objects)
	ar rcu $(OUTPUT_DIR)\libwxscintilla$(EXT).a $(sci_objects)
	ranlib $(OUTPUT_DIR)\libwxscintilla$(EXT).a
	
SQLite: sqlite_msg $(sqlite_objects)
	ar rcu $(OUTPUT_DIR)\libwxsqlite3$(EXT).a $(sqlite_objects)
	ranlib $(OUTPUT_DIR)\libwxsqlite3$(EXT).a
	
LiteEditor: lite_editor_msg $(resource_file) $(precompiled_headers) $(sample_cpp_objects) 
	$(CMP) -o $(OUTPUT_DIR)\LiteEditor $(sample_cpp_objects) $(resource_file) $(LINK_FLAGS)

$(OBJ_DIR)/code_parser.o: LiteEditor/code_parser.rc
	windres -o  $(OBJ_DIR)/$(@F) $< $(INCLUDES) -I$(WXWIN)/include 
	
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
		
pre_build:
	-if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	-if not exist $(OUTPUT_DIR) mkdir $(OUTPUT_DIR)
	-if not exist $(SCI_OBJ_DIR) mkdir $(SCI_OBJ_DIR)
	-if not exist $(SQLITE_OBJ_DIR) mkdir $(SQLITE_OBJ_DIR)
	
code_lite_msg:
	@echo ----------------------------------------------------------
	@echo Building CodeLite library
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
		
clean:
	-if exist $(OBJ_DIR)\*.o del $(OBJ_DIR)\*.o
	-rmdir $(OBJ_DIR)
	-if exist $(OUTPUT_DIR)\*.a del $(OUTPUT_DIR)\*.a
	-if exist $(SCI_OBJ_DIR)\*.o del $(SCI_OBJ_DIR)\*.o
	-rmdir $(SCI_OBJ_DIR)
	-if exist $(SQLITE_OBJ_DIR)\*.o del $(SQLITE_OBJ_DIR)\*.o
	-rmdir $(SQLITE_OBJ_DIR)
	
