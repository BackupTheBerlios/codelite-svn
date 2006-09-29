wxSQLite3 component info
------------------------

Website:      http://wxcode.sourceforge.net/components/wxsqlite3
Version:      1.6.0
Description:
wxSQLite3 is a C++ wrapper around the public domain SQLite 3.x database and
is specifically designed for use in programs based on the wxWidgets library.

wxSQLite3 does not try to hide the underlying database, in contrary almost
all special features of the current SQLite3 version 3.3.6 are supported, like
for example the creation of user defined scalar or aggregate functions.

Since SQLite stores strings in UTF-8 encoding, the wxSQLite3 methods provide 
automatic conversion between wxStrings and UTF-8 strings. This works best for   
the Unicode builds of wxWidgets. This feature is not fully tested for ANSI 
builds.


Version history
---------------

 1.6.0 - Added support for user-defined collation sequences
 1.5.3 - Upgrade to SQLite version 3.3.6
         Added support for optional SQLite meta data methods
 1.5.2 - Fixed a bug in wxSQLite3Database::Prepare
         Added wxSQLite3Database::IsOpen for convenience
 1.5.1 - SQLite DLL upgraded to version 3.3.4
 1.5   - Upgrade to SQLite version 3.3.3
         Added support for commit, rollback and update callbacks
 1.4.2 - Optimized code for wxString arguments
 1.4.1 - Fixed a bug in TableExists, eliminated some compiler warnings
         Changed handling of Unicode string conversion
         Added support for different transaction types
 1.4   - Optionally load SQLite library dynamically at run time
 1.3.1 - Corrected wxSQLite3ResultSet::GetInt64, added wxSQLite3Table::GetInt64
 1.3   - Added wxGTK build support
 1.2   - Corrected error in wxSQLite3Table::FindColumnIndex
 1.1   - Upgrade to SQLite version 3.2.7
 1.0   - First public release


Installation
------------

a) wxMSW

When building on win32, you can use the makefiles in the BUILD folder.

SQLite version 3.3.6 DLL is included. The included link library was built
with MS Visual C++ 6. For other compilers it can be necessary to regenerate
the link library based on the sqlite.def file in the LIB folder.

mingw: dlltool -d sqlite3.def -D sqlite3.dll -l sqlite3.a

The directory sqlite3/lib contains the original DLL from the SQLite distribution.

The directory sqlite3/meta contains a special DLL version including support for
the optional SQLite meta data methods.

To get the sample application to work the SQLite DLL to be used has to be copied
to the samples directory.

If you want to use the optional SQLite meta data methods SQLite needs to be
compiled with SQLITE_ENABLE_COLUMN_METADATA (as is the case for the DLL version
in directory sqlite3/meta). Additionally the preprocessor
symbol WXSQLITE3_HAVE_METADATA must be defined when compiling wxSQLite3.

b) wxGTK

When building on an autoconf-based system (like Linux/GNU-based systems),
you can use the existing configure script in the component's ROOT folder or you
can recreate the configure script doing:

  cd build
  ./acregen.sh
  cd ..

  ./configure [here you should use the same flags you used to configure wxWidgets]
  make
 
Type "./configure --help" for more info.
The autoconf-based systems also support a "make install" target which builds
the library and then copies the headers of the component to /usr/local/include
and the lib to /usr/local/lib.

SQLite version 3.3.6 is NOT included. You have to download the current version
of SQLite from http://www.sqlite.org and to install in on your system before you
can install wxSQLite3.

If you want to use the optional SQLite meta data methods SQLite needs to be
compiled with SQLITE_ENABLE_COLUMN_METADATA. Additionally the preprocessor
symbol WXSQLITE3_HAVE_METADATA must be defined when compiling wxSQLite3.


Using statically linked SQLite library on Windows
-------------------------------------------------

If you want or need to compile wxSQLite3 lib in such a way that it does not require
the sqlite3.dll at run-time, you have to build a static sqlite3 library.
No modifications to wxSQLite3 are required. 

The following steps are required:

- You have to download the complete source code for sqlite from
  http://www.sqlite.org/download.html. Look for the ZIP archive since it already
  includes all generated sources and is ready to compile on Windows without the
  need for additional tools.
- Unfortunately no makefile is included. That is, you have to create a makefile
  or project file yourself. You have to build a static library from all C source
  files except shell.c (which is the stand alone tool for sqlite3) and tclsqlite.c
  (which is the tcl wrapper).
- Replace the file sqlite3.lib in the wxSQLite3 distribution by the sqlite3.lib
  created as stated above and compile wxSQLite3 as a static library.
- Don't forget to set USE_DYNAMIC_SQLITE3_LOAD=0. 


Known bugs
----------

None


Authors' info
-------------

Ulrich Telle   utelle@users.sourceforge.net

