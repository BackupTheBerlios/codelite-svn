## Author: Eran Ifrah
 
 
## 
## Set default build set
## as ANSI-deubg
##
WXCFG=--unicode=no --debug=yes 
EXT=d
OBJ_DIR=Debug_gcc
DEBUG= -g 

##
## Override default settings by typing: make type=[release | release_unicode | debug_unicode]
##
ifeq ($(type), release)
WXCFG=--unicode=no  --debug=no
EXT=
OBJ_DIR=Release_gcc
OPT=-O3
DEBUG=
endif

ifeq ($(type), release_unicode)
WXCFG=--unicode=yes --debug=no
EXT=u
OBJ_DIR=Release_gcc_unicode
OPT=-O3 -DREGEXP_UNICODE
DEBUG=
endif

ifeq ($(type), debug_unicode)
WXCFG=--unicode=yes --debug=yes
EXT=ud
OBJ_DIR=Debug_gcc_unicode
DEBUG= -g 
OPT=-DREGEXP_UNICODE
endif

WXVER=26
OUTPUT_DIR=../lib

#PROFILER= -pg 
##
## Define variables, using wx-config tool
##
CMP=g++ $(DEBUG) $(OPT) 

CCFLAGS= -D__WX__ -Wall $(TRACE_FLAG) -I. -DWXUSINGDLL -DWX_PRECOMP -DNO_GCC_PRAGMA -DXTHREADS -D_REENTRANT -DXUSE_MTSAFE_API -pthread $(shell wx-config --cxxflags $(WXCFG)) $(PROFILER) -fno-strict-aliasing -pthread 

SQLITE_INCLUDE= -I../sdk/wxsqlite3/include -I../sdk/wxsqlite3/sqlite3/include
INCLUDES = -I. $(SQLITE_INCLUDE) -I../CodeLite -ILiteEditor 

##
## Define the object files
##
lib_cpp_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard *.cpp)))))
lib_c_objects := $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(wildcard *.c)))))

## our main build target
build : pre_build Plugin

Plugin: $(lib_cpp_objects) $(lib_c_objects)
	ar rcu $(OUTPUT_DIR)/libplugin$(EXT).a $(lib_cpp_objects)  $(lib_c_objects) 
	ranlib $(OUTPUT_DIR)/libplugin$(EXT).a

$(OBJ_DIR)/%.o: %.cpp 
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

$(OBJ_DIR)/%.o: %.c
	$(CMP) $(CCFLAGS) $(INCLUDES) -c $< -o $(OBJ_DIR)/$(@F)

pre_build:
	test -d  $(OBJ_DIR) || mkdir $(OBJ_DIR)
	test -d  $(OUTPUT_DIR) || mkdir $(OUTPUT_DIR)

clean:
	$(RM) Debug_gcc/*.o Release_gcc/*.o Release_gcc_unicode/*. Debug_gcc_unicode/*.o lib/*.o lib/*.a
	$(RM) -R Debug_gcc Release_gcc Release_gcc_unicode Debug_gcc_unicode 
