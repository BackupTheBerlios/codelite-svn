#include "precompiled_header.h"

#include "tags_database.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

//-------------------------------------------------
// Tags database class implementation
//-------------------------------------------------
TagsDatabase::TagsDatabase(bool memDb)
: m_memDb(memDb)
{
	m_db = new wxSQLite3Database();

	// are we memory database?
	if(m_memDb)
		m_db->Open(wxT(":memory:"));
}

TagsDatabase::~TagsDatabase()
{
	if(m_db)
	{
		wxLogMessage(wxT("closing database..."));
		m_db->Close();
		delete m_db;
		m_db = NULL;
	}
}

void TagsDatabase::OpenDatabase(const wxFileName& fileName)
{
	if(m_memDb)
		return;

	if(m_fileName == fileName)
		return;

	// Did we get a file name to use?
	if(!fileName.IsOk() && !m_fileName.IsOk())
		return;

	// We did not get any file name to use BUT we
	// do have an open database, so we will use it
	if(!fileName.IsOk())
		return;

	if(!m_fileName.IsOk())
	{
		// First time we open the db
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
	}
	else
	{
		// We have both fileName & m_fileName and they 
		// are different, Close previous db
		m_db->Close();
		m_db->Open(fileName.GetFullPath());
		CreateSchema();
		m_fileName = fileName;
	}
}

void TagsDatabase::CreateSchema()
{
	wxString sql;

	// improve performace by using pragma command:
	// (this needs to be done before the creation of the
	// tables and indices)
	try
	{
		sql = _T("PRAGMA synchronous = OFF;");
		m_db->ExecuteUpdate(sql);

		sql = _T("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = _T("PRAGMA default_cache_size = 10000;");
		m_db->ExecuteUpdate(sql);
	
		sql = _T("create  table if not exists tags (ID INTEGER PRIMARY KEY AUTOINCREMENT, PARENTID INTEGER, name string, file string, line integer, kind string, access string, signature string, pattern string, parent string, inherits string, path string, typeref string);");
		m_db->ExecuteUpdate(sql);

		sql = _T("create  table if not exists comments (comment string, file string, line number);");
		m_db->ExecuteUpdate(sql);

		sql = _T("CREATE UNIQUE INDEX IF NOT EXISTS COMMENTS_UNIQ on comments(file, line)");
		m_db->ExecuteUpdate(sql);

		// Create unique index on tags table
		sql = _T("CREATE UNIQUE INDEX IF NOT EXISTS TAGS_UNIQ on tags(kind, path, signature);");
		m_db->ExecuteUpdate(sql);

		// Create search indexes
		sql = _T("CREATE INDEX IF NOT EXISTS TAGS_NAME on tags(name);");
		m_db->ExecuteUpdate(sql);

		sql = _T("CREATE INDEX IF NOT EXISTS TAGS_PARENT on tags(parent);");
		m_db->ExecuteUpdate(sql);

		sql = _T("create table if not exists tags_version (version string);");
		m_db->ExecuteUpdate(sql);

		sql = _T("create unique index if not exists tags_version_uniq on tags_version(version);");
		m_db->ExecuteUpdate(sql);

		sql = wxString(_T("insert into tags_version values ('"));
		sql << GetVersion() << _T("');");
		m_db->ExecuteUpdate(sql);
	}
	catch(...)
	{	
	}
}

wxString TagsDatabase::GetSchemaVersion() const 
{
	// return the current schema version
	try 
	{
		wxString sql;
		wxString version;
		sql = _T("SELECT * FROM TAGS_VERSION");
		wxSQLite3ResultSet rs = m_db->ExecuteQuery( sql );

		if( rs.NextRow() )
			version = rs.GetString(0);
		return version;
	}
	catch(wxSQLite3Exception &e )
	{
		wxUnusedVar(e);
	}
	return wxEmptyString;
}

void TagsDatabase::Store(const std::vector<DbRecordPtr> &records, const wxFileName& path, bool autoCommit)
{
	if(!path.IsOk() && !m_fileName.IsOk() && !m_memDb)
	{
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}

	if(records.empty())
		return;

	OpenDatabase(path);
	try
	{
		// Create the statements before the execution
		wxSQLite3Statement insertStmt = m_db->PrepareStatement(records[0]->GetInsertOneStatement());
		
		std::vector<DbRecordPtr> updateList;

		// AddChild entries to database
		// we break the bug transaction into samller ones of 1000 operations
		// each
		const size_t bulk = 1000;
		if( autoCommit )
			m_db->Begin();

		size_t i=0;
		for(; i<records.size(); i++)
		{
			if(records[i]->Store(insertStmt) == TagExist)
			{
				// Update the record
				updateList.push_back(records[i]);
			}
			else
			{
				// insert succeeded
				if(i % bulk == 0 && autoCommit)
				{
					m_db->Commit();
					m_db->Begin();
				}
			}
		}
		insertStmt.Finalize();

		if( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if(!updateList.empty())
		{
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0]->GetUpdateOneStatement() );
			if( autoCommit )
				m_db->Begin();
			for(size_t i=0; i<updateList.size(); i++)
			{
				updateList[i]->Update(updateStmt);
				if( i % bulk == 0 && autoCommit )
				{
					m_db->Commit();
					m_db->Begin();
				}
			}
			updateStmt.Finalize();
			if( autoCommit )
				m_db->Commit();
		}
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
		if( autoCommit )
			m_db->Rollback();
	}	
}

void TagsDatabase::Store(TagTreePtr tree, const wxFileName& path, bool autoCommit)
{
	if(!path.IsOk() && !m_fileName.IsOk() && !m_memDb)
	{
		// An attempt is made to save the tree into db but no database
		// is provided and none is currently opened to use
		return;
	}
	
	if( !tree )
		return;

	OpenDatabase(path);
	TreeWalker<wxString, TagEntry> walker( tree->GetRoot() );

	try
	{
		// Create the statements before the execution
		TagEntry dummy;
		wxSQLite3Statement insertStmt = m_db->PrepareStatement( dummy.GetInsertOneStatement() );
		
		std::vector<TagEntry> updateList;

		// AddChild entries to database
		if( autoCommit )
			m_db->Begin();

		for(; !walker.End(); walker++)
		{
			// Skip root node
			if(walker.GetNode() == tree->GetRoot())
				continue;

			walker.GetNode()->GetData().SetParentId(walker.GetNode()->GetParent()->GetData().GetId());
			if(walker.GetNode()->GetData().Store(insertStmt) == TagExist)
			{
				// Update the record
				updateList.push_back(walker.GetNode()->GetData());
			}
		}
		insertStmt.Finalize();

		if( autoCommit )
			m_db->Commit();

		// Do we need to update?
		if(!updateList.empty())
		{
			wxSQLite3Statement updateStmt = m_db->PrepareStatement( updateList[0].GetUpdateOneStatement() );
			if( autoCommit )
				m_db->Begin();
			for(size_t i=0; i<updateList.size(); i++)
				updateList[i].Update(updateStmt);
			updateStmt.Finalize();
			if( autoCommit )
				m_db->Commit();
		}
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
		if( autoCommit )
			m_db->Rollback();
	}
}

wxSQLite3ResultSet TagsDatabase::SelectTagsByFile(const wxString& file, const wxFileName& path)
{
	// Incase empty file path is provided, use the current file name
	wxFileName databaseFileName(path);
	path.IsOk() == false ? databaseFileName = m_fileName : databaseFileName = path;
	OpenDatabase(databaseFileName);

	wxString query;
	query = wxString::Format(_T("select * from tags where file='%s';"), file.GetData());
	return m_db->ExecuteQuery(query.GetData());
}


void TagsDatabase::DeleteByFileName(const wxFileName& path, const wxString& fileName, bool autoCommit)
{
	// make sure database is open
	OpenDatabase(path);

	try	
	{
		if( autoCommit )
			m_db->Begin();
		m_db->ExecuteUpdate(wxString::Format(wxT("Delete from tags where File='%s'"), fileName.GetData()));

		if( autoCommit )
			m_db->Commit();
	}
	catch (wxSQLite3Exception& e)
	{
		wxLogMessage(e.GetMessage());
		if( autoCommit )
			m_db->Rollback();
	}
}

wxSQLite3ResultSet TagsDatabase::Query(const wxString& sql, const wxFileName& path)
{
	// make sure database is open
	OpenDatabase(path);

	try	
	{
		return m_db->ExecuteQuery(sql);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
	return wxSQLite3ResultSet();
}

void TagsDatabase::ExecuteUpdate(const wxString& sql)
{
	try	
	{
		m_db->ExecuteUpdate(sql);
	}
	catch (wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
}

const bool TagsDatabase::IsOpen() const
{
	return m_db->IsOpen();
}

void TagsDatabase::LoadToMemory(const wxFileName& fn)
{

	// this function is useful if our database is static and unlikely to be changed
	// A good candidate for it, is the external database (a database for static tags like, stl, wxwidgets symbols etc)
	
	// to load a database into memory, we do the following:
	// - attach to the file database and copy its schema to our memory
	// - copy the whole database content to our memory
	// - dettach the file database
	if(m_db->IsOpen() && !m_memDb)
	{
		// close any opened database and reopen it as in-memory
		wxLogMessage(wxT("closing database..."));
		m_db->Close();
		m_db->Open(_T(":memory:"));
	}

	try
	{
		// copy database schema, we do it by opening a second database
		wxString sql;
		wxSQLite3Database *budb = new wxSQLite3Database();
		budb->Open(fn.GetFullPath());
	
		budb->Begin();
		wxSQLite3ResultSet rs = budb->ExecuteQuery( _T("SELECT sql FROM sqlite_master WHERE sql NOT NULL") );
		while( rs.NextRow() )
		{
			sql = rs.GetString(0);
			m_db->ExecuteUpdate( sql );
		}
		budb->Commit();
		budb->Close();
		delete budb;

		// insert all data from file database into memory db
		sql.Empty();
		sql << _T("ATTACH DATABASE '") << fn.GetFullPath() << _T("' as backup");
		m_db->ExecuteUpdate(sql);

		m_db->Begin();
		sql = _T("insert into tags select project, name, file, line, kind, access, signature, pattern, parent, inherits, path, typeref FROM backup.tags");
		m_db->ExecuteUpdate(sql);
		m_db->Commit();
	}
	catch(wxSQLite3Exception& e)
	{
		wxUnusedVar(e);
	}
}

//void TagsDatabase::Store(const std::vector<TagEntry> &tags, const wxFileName &path, bool autoCommit)
//{
	// convert tags to DbRecord pointer array
//	std::vector<DbRecordPtr> records;
//	for(std::vector<TagEntry>::size_type i=0; i<tags.size(); i++)
//		records.push_back( static_cast<DbRecord*>( new TagEntry( tags[i] ) ) );
//	Store( records, path, autoCommit );
//}


