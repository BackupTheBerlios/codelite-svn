#include "precompiled_header.h"

#include "entry.h"
#include "tokenizer.h"
#include "language.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

TagEntry::TagEntry(const tagEntry& entry, const wxString& project)
{
	Create(entry, project);
}

TagEntry::TagEntry()
: m_project(wxEmptyString)
, m_path(wxEmptyString)
, m_file(wxEmptyString)
, m_lineNumber(-1)
, m_pattern(wxEmptyString)
, m_kind(_T("<unknown>"))
, m_parent(wxEmptyString)
, m_name(wxEmptyString)
, m_position(wxNOT_FOUND)
{
}

TagEntry::~TagEntry()
{
}

TagEntry::TagEntry(const TagEntry& rhs)
{
	*this = rhs;
}

TagEntry& TagEntry::operator=(const TagEntry& rhs)
{
	m_project = rhs.m_project;
	m_file = rhs.m_file;
	m_kind = rhs.m_kind;
	m_parent = rhs.m_parent;
	m_pattern = rhs.m_pattern;
	m_lineNumber = rhs.m_lineNumber;
	m_name = rhs.m_name;
	m_path = rhs.m_path;
	m_hti = rhs.m_hti;
	m_position = rhs.m_position;
	m_extFields = std::map<wxString, wxString>(rhs.m_extFields);
	return *this;
}

bool TagEntry::operator ==(const TagEntry& rhs)
{
	//Note: tree item id is not used in this function!
	return	
		m_project == rhs.m_project && 
		m_file == rhs.m_file &&
		m_kind == rhs.m_kind &&
		m_parent == rhs.m_parent &&
		m_pattern == rhs.m_pattern &&
		m_lineNumber == rhs.m_lineNumber &&
		m_name == rhs.m_name &&
		m_path == rhs.m_path &&
		GetInherits() == rhs.GetInherits() &&
		GetAccess() == rhs.GetAccess() &&
		GetSignature() == rhs.GetSignature() &&
		GetPosition() == rhs.GetPosition() &&
		GetTyperef() == rhs.GetTyperef();
}

void TagEntry::Create(const char *fileName, const char *name, int lineNumber, const char *pattern, const char *kind, std::map<wxString, wxString>& extFields, const wxString& project)
{
	SetPosition( wxNOT_FOUND );
	SetName( _U(name) );
	SetLine( lineNumber );
	SetKind( kind == NULL ? _T("<unknown>") : _U(kind) );
	SetPattern( _U(pattern) );
	SetFile( _U(fileName) );
	SetProject( _U(project) );

	m_extFields = extFields;
	wxString path;
	
	// Check if we can get full name (including path)
	path = GetExtField(_T("class"));
	UpdatePath( path ) ;

	path = GetExtField(_T("struct"));
	UpdatePath( path ) ;

	path = GetExtField(_T("union"));
	UpdatePath( path ) ;

	path = GetExtField(_T("namespace"));
	UpdatePath( path ) ;

	path = GetExtField(_T("enum"));
	UpdatePath( path ) ;

	// If there is no path, path is set to name
	if( GetPath().IsEmpty() )
		SetPath( GetName() );

	// Get the parent name
	StringTokenizer tok(GetPath(), _T("::"));
	wxString parent;

	(tok.Count() < 2) ? parent = _T("<global>") : parent = tok[tok.Count()-2];
	SetParent(parent);

	// If we have a project, add it to the tag name
	if(!project.IsEmpty())
	{
		SetPath( project + _T("::") + GetPath() );
	}
}

void TagEntry::Create(const tagEntry& entry, const wxString& project)
{
	// Get other information from the string data and store it into map
	for (int i = 0;  i < entry.fields.count;  ++i)
	{
		wxString key = _U(entry.fields.list[i].key);
		wxString value = _U(entry.fields.list[i].value);
		m_extFields[key] = _U(value);
	}
	Create(entry.file, entry.name, entry.address.lineNumber, entry.address.pattern, entry.kind, m_extFields, project);
}

void TagEntry::Print()
{
	std::cout << "======================================" << std::endl;
	std::cout << "Name:\t\t" << GetName() << std::endl;
	std::cout << "File:\t\t" << GetFile() << std::endl;
	std::cout << "Line:\t\t" << GetLine() << std::endl;
	std::cout << "Pattern\t\t" << GetPattern() << std::endl;
	std::cout << "Kind:\t\t" << GetKind() << std::endl;
	std::cout << "Parent:\t\t" << GetParent() << std::endl;
	
	std::cout << " ---- Ext fields: ---- " << std::endl;
	std::map<wxString, wxString>::const_iterator iter = m_extFields.begin();
	for(; iter != m_extFields.end(); iter++)
		std::cout << iter->first << ":\t\t" << iter->second << std::endl;
	std::cout << "======================================" << std::endl;
}

wxString TagEntry::Key() const 
{
	wxString key;
	if( GetKind() == _T("prototype"))
		key << _T("[prototype] ");
	key << GetPath() << GetSignature();
	return key;
}

wxString TagEntry::GetDisplayName() const
{
	wxString name;
	name << GetName() << GetSignature();
	return name;
}

wxString TagEntry::GetFullDisplayName() const
{
	wxString name;

	if( GetParent() == wxT("<global>") ){
		name << GetDisplayName(); 
	} else {
		name << GetParent() << wxT("::") << GetName() << GetSignature();
	}
		
	return name;
}

TagEntry::TagEntry(wxSQLite3ResultSet& rs)
{
	m_extFields.clear();
	m_project = rs.GetString(0);
	m_name = rs.GetString(1);
	m_file = rs.GetString(2);
	m_lineNumber = rs.GetInt(3);
	m_kind = rs.GetString(4);
	m_extFields[_T("access")] = rs.GetString(5);
	m_extFields[_T("signature")] = rs.GetString(6);
	m_pattern = rs.GetString(7);
	m_parent  = rs.GetString(8);
	m_extFields[_T("inherits")] = rs.GetString(9);
	m_path = rs.GetString(10);
	m_extFields[_T("typeref")] = rs.GetString(11);
	m_position = wxNOT_FOUND;
}

//----------------------------------------------------------------------------
// Database operations
//----------------------------------------------------------------------------

int TagEntry::Store(wxSQLite3Statement& insertPerepareStmnt)
{
	// If this node is a dummy, (IsOk() == false) we dont insert it to database
	if( !IsOk() )
		return TagOk;

	try
	{
		// see TagsDatabase::GetInsertOneStatement() for the order of binding
		insertPerepareStmnt.Bind(1, GetProject());
		insertPerepareStmnt.Bind(2, GetName());
		insertPerepareStmnt.Bind(3, GetFile());
		insertPerepareStmnt.Bind(4, GetLine());
		insertPerepareStmnt.Bind(5, GetKind());
		insertPerepareStmnt.Bind(6, GetAccess());
		insertPerepareStmnt.Bind(7, GetSignature());
		insertPerepareStmnt.Bind(8, GetPattern());
		insertPerepareStmnt.Bind(9, GetParent());
		insertPerepareStmnt.Bind(10, GetInherits());
		insertPerepareStmnt.Bind(11, GetPath());
		insertPerepareStmnt.Bind(12, GetTyperef());
		insertPerepareStmnt.ExecuteUpdate();
		insertPerepareStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		if(exc.ErrorCodeAsString(exc.GetErrorCode()) == _T("SQLITE_CONSTRAINT"))
			return TagExist;
		return TagError;
	}
	return TagOk;
}

int TagEntry::Update(wxSQLite3Statement& updatePerepareStmnt)
{
	// If this node is a dummy, (IsOk() == false) we dont update it to database
	if( !IsOk() )
		return TagOk;

	try
	{
		// see TagsDatabase::GetUpdateOneStatement() function for the order of binding
		updatePerepareStmnt.Bind(1, GetName());
		updatePerepareStmnt.Bind(2, GetFile());
		updatePerepareStmnt.Bind(3, GetLine());
		updatePerepareStmnt.Bind(4, GetAccess());
		updatePerepareStmnt.Bind(5, GetPattern());
		updatePerepareStmnt.Bind(6, GetParent());
		updatePerepareStmnt.Bind(7, GetInherits());
		updatePerepareStmnt.Bind(8, GetTyperef());
		updatePerepareStmnt.Bind(9, GetProject());
		updatePerepareStmnt.Bind(10, GetKind());
		updatePerepareStmnt.Bind(11, GetSignature());
		updatePerepareStmnt.Bind(12, GetPath());
		updatePerepareStmnt.ExecuteUpdate();
		updatePerepareStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		std::cerr << "Error: " << exc.ErrorCodeAsString(exc.GetErrorCode()) << std::endl;
		return TagError;
	}
	return TagOk;
}

int TagEntry::Delete(wxSQLite3Statement &deletePreparedStmnt)
{
	// Delete this record from database.
	// Delete is done using the index
	try
	{
		deletePreparedStmnt.Bind(1, GetProject());		// Project
		deletePreparedStmnt.Bind(2, GetKind());			// Kind
		deletePreparedStmnt.Bind(3, GetSignature());	// Signature
		deletePreparedStmnt.Bind(4, GetPath());			// Path
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		std::cerr << "Error: " << exc.ErrorCodeAsString(exc.GetErrorCode()) << std::endl;
		return TagError;
	}
	return TagOk;
}

wxString TagEntry::GetScopeName() const
{
	wxString scopeName(GetPath());

	if(GetKind() == _T("project"))
		return wxEmptyString;

	// Is this is a global tag?
	if(GetParent() == _T("<global>"))
		return _T("<global>");

	// Get the scope name (if we have one)
	if(scopeName.IsEmpty())
		return wxEmptyString;
	else
	{
		if(!GetProject().IsEmpty())
		{
			// Remove the project name from the path
			wxString prefix(GetProject() + _T("::"));
			scopeName.StartsWith(prefix.GetData(), &scopeName);
		}

		// Remove the last token
		StringTokenizer tok(scopeName, _T("::"));
		scopeName.Empty();
	
		for(int i=0; i<tok.Count()-1; i++)
			scopeName += tok[i];
	}
	return scopeName;
}

const bool TagEntry::IsContainer() const
{
	return	GetKind() == _T("class")  || 
			GetKind() == _T("struct") || 
			GetKind() == _T("union")  || 
			GetKind() == _T("namespace") ||
			GetKind() == _T("project");
}

void TagEntry::UpdatePath(wxString & path)
{
	if(!path.IsEmpty())
	{
		wxString name(path);
		name += _T("::");
		name += GetName();
		SetPath(name);
	}
	path.Empty();
}

wxString TagEntry::NameFromTyperef() const
{
	wxString typeref = GetTyperef();
	if( typeref.IsEmpty() == false )
	{
		wxString name = typeref.AfterFirst(_T(':'));
		return name;		
	}

	// incase our entry is a typedef, and it is not marked as typeref, 
	// try to get the real name from the pattern
	if( GetKind() == _T("typedef"))
	{
		CppScanner sc;
		sc.SetText( _C(GetPattern()) );
		
		// search for the 'typedef' keyword
		int type = 0;
		while( (type = sc.yylex()) != 0 )
		{
			wxString token = _U(sc.YYText());
			if( token == _T("typedef") )
			{
				// the real name is the next token
				if( sc.yylex() == 0 )
					return wxEmptyString;
				token = _U(sc.YYText());

				// remove any template sepcialization (if any)
				wxString name = token.BeforeFirst(_T('<'));
				return name;
			}
		}
	}
	return wxEmptyString;
}

wxString TagEntry::GetDeleteOneStatement()
{
	return _T("DELETE FROM TAGS WHERE Project=? AND Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetUpdateOneStatement()
{
	return _T("UPDATE TAGS SET Name=?, File=?, Line=?, Access=?, Pattern=?, Parent=?, Inherits=?, Typeref=? WHERE Project=? AND Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetInsertOneStatement()
{
	return _T("INSERT INTO TAGS VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");	
}

