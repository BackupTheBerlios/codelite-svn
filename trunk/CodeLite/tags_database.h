#ifndef CODELITE_TAGS_DATABASE_H
#define CODELITE_TAGS_DATABASE_H

#include "tag_tree.h"
#include "entry.h"
#include <wx/filename.h>
#include "db_record.h"

const wxString gTagsDatabaseVersion(_T("CodeLite version 0.3 Alpha"));

#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif

/**
TagsDatabase is a wrapper around wxSQLite3 database with tags specific functions. 
It allows caller to query and populate the SQLite database for tags with a set of convinient functions.
this class is responsible for creating the schema for the CodeLite library.

The tables are automatically created once a database is created
  
Database tables:
 
Table Name: TAGS
 
|| Column Name || Type || Description 
|Project	| String | Project name of the entry
|Name	| String | Tag name is appears in ctags output file	
|File	| String | File that this tag was found in
|Line	| Number | Line number  
|Kind	| String | Tag kind, can be one of: function, prototype, class, namespace, variable, member, enum, enumerator, macro, project, union, typedef	
|Access	| String | Can be one of public, private protected
|Signature	| String | For functions, this column holds the function signature
|Pattern	| String | pattern that can be used to located this tag in the file 
|Parent	| String | tag direct parent, can be its class parent (for members or functions), namespace or the literal "<global>"
|Inherits	| String | If this class/struct inherits from other class, it will cotain the name of the base class
|Path	| String | full name including path, (e.g. Project::ClassName::FunctionName
|Typeref| String | Special type of tag, that points to other Tag (i.e. typedef)

Table Name: COMMENTS
 
|| Column Name || Type || Description 
|Comment| String | String comment found in code
|File	| String | File that the comment was found in
|Line	| Number | Line number of the comment in File

Table Name: TAGS_VERSION
  
|| Column Name || Type || Description 
| Version | String | contains the current database schema

 
\date 08-22-2006
\author Eran
\ingroup CodeLite
*/
class WXDLLIMPEXP_CL TagsDatabase
{
	wxSQLite3Database *m_db;
	wxFileName m_fileName;

public:
	/**
	 * Construct a tags database.
	 */
	TagsDatabase(bool memDb = false);

	/**
	 *
	 * Destructor
	 */
	virtual ~TagsDatabase();

	/**
	 * Return the currently opened database.
	 * \return Currently open database
	 */
	const wxFileName& GetDatabaseFileName() const { return m_fileName; }
	
	/**
	 * Open sqlite database.
	 * \param fileName Database file name
	 */
	void OpenDatabase(const wxFileName& fileName);
	
	/**
	 * Create database if not existed already.
	 */
	void CreateSchema();

	/**
	 * Store tree of tags into db.
	 * \param tree Tags tree to store
	 * \param path Database file name
	 * \param autoCommit handle the Store operation inside a transaction or let the user hadle it
	 */
	void Store(TagTreePtr tree, const wxFileName& path, bool autoCommit = true);

	/**
	 * Store vector of database recreds into db.
	 * ASSUMPTION: all records are of same type (i.e. Comments OR TagEntry )
	 * \param records records to store
	 * \param path Database file name
	 * \param autoCommit handle the Store operation inside a transaction or let the user hadle it
	 */
	void Store(const std::vector<DbRecordPtr> &records, const wxFileName& path, bool autoCommit = true);

	/**
	 * Return a result set of tags according to file name.
	 * \param file Source file name
	 * \param path Database file name
	 * \return result set
	 */
	wxSQLite3ResultSet SelectTagsByFile(const wxString& file, const wxFileName& path = wxFileName());

	/**
	 * Delete all entries from database that are related to filename.
	 * \param path Database name
	 * \param fileName File name
	 * \param autoCommit handle the Delete operation inside a transaction or let the user hadle it
	 */
	void DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit = true);

	/**
	 * Begin transaction.
	 */
	void Begin() { return m_db->Begin(); }

	/**
	 * Commit transaction.
	 */
	void Commit() { return m_db->Commit(); }

	/**
	 * Rollback transaction.
	 */
	void Rollback() { return m_db->Begin(); }

	/**
	 * Test whether the database is opened 
	 * \return true if database is attached to a file
	 */
	const bool IsOpen() const;

	/**
	 * Execute a query sql and return result set.
	 * \param sql Select statement
	 * \param path Database file to use
	 * \return result set
	 */
	wxSQLite3ResultSet Query(const wxString& sql, const wxFileName& path = wxFileName());

	/**
	 * Return SQLite3 preapre statement object
	 * \param sql sql
	 * \return wxSQLite3ResultSet object
	 */
	wxSQLite3Statement PrepareStatement( const wxString & sql ) { return m_db->PrepareStatement( sql ); }

	/**
	 * Execute query 
	 * \param sql sql to execute
	 */
	void ExecuteUpdate( const wxString& sql );

	/**
	 * Return the current version of the database library .
	 * \return current version of the database library
	 */
	const wxString& GetVersion() const { return gTagsDatabaseVersion; }

	/**
	 * Load the tags table into memory, to improve performance
	 * \param fn file name 
	 */
	void LoadToMemory(const wxFileName& fn);

	/**
	 * Schema version as appears in TAGS_VERSION table
	 * \return schema's version
	 */
	wxString GetSchemaVersion() const;
};


#endif // CODELITE_TAGS_DATABASE_H
