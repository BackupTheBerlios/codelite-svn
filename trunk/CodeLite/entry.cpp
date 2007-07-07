#include "precompiled_header.h"

#include "entry.h"
#include "tokenizer.h"
#include "language.h"

#ifdef __VISUALC__
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

TagEntry::TagEntry(const tagEntry& entry)
{
	Create(entry);
}

TagEntry::TagEntry()
: m_path(wxEmptyString)
, m_file(wxEmptyString)
, m_lineNumber(-1)
, m_pattern(wxEmptyString)
, m_kind(wxT("<unknown>"))
, m_parent(wxEmptyString)
, m_name(wxEmptyString)
, m_position(wxNOT_FOUND)
, m_id(wxNOT_FOUND)
, m_parentId(wxNOT_FOUND)
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
	m_id = rhs.m_id;
	m_parentId = rhs.m_parentId;
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
		m_parentId == rhs.m_parentId &&
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

void TagEntry::Create(const wxString &fileName, 
					  const wxString &name, 
					  int lineNumber, 
					  const wxString &pattern, 
					  const wxString &kind, 
					  std::map<wxString, wxString>& extFields)
{
	SetPosition( wxNOT_FOUND );
	SetName( name );
	SetLine( lineNumber );
	SetKind( kind.IsEmpty() ? wxT("<unknown>") : kind );
	SetPattern( pattern );
	SetFile( fileName );
	SetId(-1);
	SetParentId(-1);

	m_extFields = extFields;
	wxString path;
	
	// Check if we can get full name (including path)
	path = GetExtField(wxT("class"));
	UpdatePath( path ) ;

	path = GetExtField(wxT("interface"));
	UpdatePath( path ) ;

	path = GetExtField(wxT("struct"));
	UpdatePath( path ) ;

	path = GetExtField(wxT("union"));
	UpdatePath( path ) ;

	path = GetExtField(wxT("namespace"));
	UpdatePath( path ) ;

	path = GetExtField(wxT("enum"));
	UpdatePath( path ) ;

	// If there is no path, path is set to name
	if( GetPath().IsEmpty() )
		SetPath( GetName() );

	// Get the parent name
	StringTokenizer tok(GetPath(), wxT("::"));
	wxString parent;

	(tok.Count() < 2) ? parent = wxT("<global>") : parent = tok[tok.Count()-2];
	SetParent(parent);
}

void TagEntry::Create(const tagEntry& entry)
{
	// Get other information from the string data and store it into map
	for (int i = 0;  i < entry.fields.count;  ++i)
	{
		wxString key = _U(entry.fields.list[i].key);
		wxString value = _U(entry.fields.list[i].value);
		m_extFields[key] = _U(value);
	}
	Create(	_U(entry.file), 
			_U(entry.name), 
			entry.address.lineNumber, 
			_U(entry.address.pattern), 
			_U(entry.kind), 
			m_extFields);
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
	if( GetKind() == wxT("prototype"))
		key << wxT("[prototype] ");
	key << GetPath() << GetSignature();
	return key;
}

wxString TagEntry::GetDisplayName() const
{
	wxString name;
	name << GetName() << GetSignature();
	if(GetKind() == wxT("prototype"))
	{
		name << wxT(": [prototype]");
	}
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
	m_id = rs.GetInt(0);
	m_parentId = rs.GetInt(1);
	m_name = rs.GetString(2);
	m_file = rs.GetString(3);
	m_lineNumber = rs.GetInt(4);
	m_kind = rs.GetString(5);
	m_extFields[wxT("access")] = rs.GetString(6);
	m_extFields[wxT("signature")] = rs.GetString(7);
	m_pattern = rs.GetString(8);
	m_parent  = rs.GetString(9);
	m_extFields[wxT("inherits")] = rs.GetString(10);
	m_path = rs.GetString(11);
	m_extFields[wxT("typeref")] = rs.GetString(12);
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
		insertPerepareStmnt.Bind(1, GetParentId());
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
		if(exc.ErrorCodeAsString(exc.GetErrorCode()) == wxT("SQLITE_CONSTRAINT"))
			return TagExist;
		wxLogMessage(exc.GetMessage());
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
		updatePerepareStmnt.Bind(1, GetParentId());
		updatePerepareStmnt.Bind(2, GetName());
		updatePerepareStmnt.Bind(3, GetFile());
		updatePerepareStmnt.Bind(4, GetLine());
		updatePerepareStmnt.Bind(5, GetAccess());
		updatePerepareStmnt.Bind(6, GetPattern());
		updatePerepareStmnt.Bind(7, GetParent());
		updatePerepareStmnt.Bind(8, GetInherits());
		updatePerepareStmnt.Bind(9, GetTyperef());
		updatePerepareStmnt.Bind(10, GetKind());
		updatePerepareStmnt.Bind(11, GetSignature());
		updatePerepareStmnt.Bind(12, GetPath());
		updatePerepareStmnt.ExecuteUpdate();
		updatePerepareStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		wxLogMessage(exc.GetMessage());
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
		deletePreparedStmnt.Bind(1, GetKind());			// Kind
		deletePreparedStmnt.Bind(2, GetSignature());	// Signature
		deletePreparedStmnt.Bind(3, GetPath());			// Path
		deletePreparedStmnt.ExecuteUpdate();
		deletePreparedStmnt.Reset();
	}
	catch(wxSQLite3Exception& exc)
	{
		wxLogMessage(exc.GetMessage());
		return TagError;
	}
	return TagOk;
}

wxString TagEntry::GetScopeName() const
{
	wxString path(GetPath());

	// Is this is a global tag?
	if(GetParent() == wxT("<global>"))
		return wxT("<global>");

	// Get the scope name (if we have one)
	if(path.IsEmpty())
		return wxEmptyString;
	else
	{
		// Remove the last token
		wxString suffix, scopeName;
		suffix << wxT("::") << GetName();

		if(path.EndsWith(suffix, &scopeName))
		{
			return scopeName;
		}
		return wxEmptyString;
	}
}

wxString TagEntry::GetKind() const {
	 // if the tree element is a typeref, use the actual type
	// and not the kind that ctags thinks it is
	wxString kind = TypeFromTyperef();
	if( kind.IsEmpty() ){
		kind = m_kind;
	}
	return kind;
}


const bool TagEntry::IsContainer() const
{
	return	GetKind() == wxT("class")  || 
			GetKind() == wxT("struct") || 
			GetKind() == wxT("union")  || 
			GetKind() == wxT("namespace") ||
			GetKind() == wxT("project");
}

void TagEntry::UpdatePath(wxString & path)
{
	if(!path.IsEmpty())
	{
		wxString name(path);
		name += wxT("::");
		name += GetName();
		SetPath(name);
	}
	path.Empty();
}

wxString TagEntry::TypeFromTyperef() const
{
	wxString typeref = GetTyperef();
	if( typeref.IsEmpty() == false )
	{
		wxString name = typeref.BeforeFirst(wxT(':'));
		return name;		
	}
	return wxEmptyString;
}


wxString TagEntry::NameFromTyperef() const
{
	wxString typeref = GetTyperef();
	if( typeref.IsEmpty() == false )
	{
		wxString name = typeref.AfterFirst(wxT(':'));
		return name;		
	}

	// incase our entry is a typedef, and it is not marked as typeref, 
	// try to get the real name from the pattern
	if( GetKind() == wxT("typedef"))
	{
		CppScanner sc;
		sc.SetText( _C(GetPattern()) );
		
		// search for the 'typedef' keyword
		int type = 0;
		while( (type = sc.yylex()) != 0 )
		{
			wxString token = _U(sc.YYText());
			if( token == wxT("typedef") )
			{
				// the real name is the next token
				if( sc.yylex() == 0 )
					return wxEmptyString;
				token = _U(sc.YYText());

				// remove any template sepcialization (if any)
				wxString name = token.BeforeFirst(wxT('<'));
				return name;
			}
		}
	}
	return wxEmptyString;
}

wxString TagEntry::GetDeleteOneStatement()
{
	return wxT("DELETE FROM TAGS WHERE Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetUpdateOneStatement()
{
	return wxT("UPDATE TAGS SET ParentId=?, Name=?, File=?, Line=?, Access=?, Pattern=?, Parent=?, Inherits=?, Typeref=? WHERE Kind=? AND Signature=? AND Path=?");
}

wxString TagEntry::GetInsertOneStatement()
{
	return wxT("INSERT INTO TAGS VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");	
}

