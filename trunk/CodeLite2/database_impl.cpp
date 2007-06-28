#include "database_impl.h"
#include <wx/wxsqlite3.h>
#include <wx/log.h>

DatabaseImpl::DatabaseImpl()
{
	m_db = new wxSQLite3Database();
}

DatabaseImpl::~DatabaseImpl()
{
	m_db->Close();
	delete m_db;
}

bool DatabaseImpl::OpenDatabase(const wxFileName &fileName)
{
	//close old database first
	if(m_db->IsOpen())
	{
		m_db->Close();
	}

	//open it
	m_db->Open(fileName.GetFullPath());
	if(m_db->TableExists(wxT("TOKEN")) == false)
	{
		//create database schema
		CreateSchema();
	}
	return true;
}

void DatabaseImpl::CreateSchema()
{
	wxString sql;
	// improve performace by using pragma command:
	// (this needs to be done before the creation of the
	// tables and indices)
	try
	{
		sql = wxT("PRAGMA synchronous = OFF;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA temp_store = MEMORY;");
		m_db->ExecuteUpdate(sql);

		sql = wxT("PRAGMA default_cache_size = 10000;");
		m_db->ExecuteUpdate(sql);
	
		sql = wxT("CREATE TABLE IF NOT EXIST TOKENS (\
				   ID INTEGER PRIMARY KEY, \
				   PARENT_ID INTEGER, \
				   NAME TEXT, \
				   TYPEREF TEXT,\
				   ACCESS TEXT, \
				   FILE TEXT, \
				   INHERITS TEXT, \
				   KIND INTEGER, \
				   LINE INTEGER, \
				   PATTERN TEXT, \
				   SCOPE TEXT, \
				   SIGNATURE TEXT);");
		m_db->ExecuteUpdate(sql);
	}
	catch(wxSQLite3Exception &e)
	{	
		wxLogMessage(e.GetMessage());
	}
}

bool DatabaseImpl::IsTokenExist(const wxString &name) const
{
	return true;
}

void DatabaseImpl::AddToken(clTokenPtr token)
{
}
