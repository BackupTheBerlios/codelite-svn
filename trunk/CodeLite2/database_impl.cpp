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
	
		sql = wxT("CREATE TABLE IF NOT EXIST TOKENS (ID INTEGER PRIMARY KEY, NAME TEXT, PARENT_ID NUMERIC, TYPEREF TEXT);");
		m_db->ExecuteUpdate(sql);

		sql = wxT("CREATE TABLE IF NOT EXIST TOKENS_DATA (ID INTEGER PRIMARY KEY, ACCESS TEXT, FILE TEXT, INHERITS TEXT, KIND TEXT, LINE TEXT, PATTERN TEXT, SCOPE TEXT, SIGNATURE TEXT);");
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
