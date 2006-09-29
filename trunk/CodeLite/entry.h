#ifndef CODELITE_ENTRY_H
#define CODELITE_ENTRY_H

#include "db_record.h"
#include <wx/treectrl.h>
#include "readtags.h"
#include <wx/string.h>
#include <map>

namespace codelite
{

/**
 * TagEntry is a persistent object which is capable of storing and loading itself from 
 * various inputs:
 * - tagEntry (ctags structure)
 * - wxSQLite3ResultSet - from the database
 * 
 * It contains all the knowledge of storing and retrieving itself from the database
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 11-11-2006
 * \author Eran
 */
class TagEntry : public DbRecord
{
	wxString m_project;		///< Project this tag is belong to
	wxString m_path;		///< Tag full path
	wxString m_file;		///< File this tag is found
	int		 m_lineNumber;	///< Line number
	wxString m_pattern;		///< A pattern that can be used to locate the tag in the file
	wxString m_kind;		///< Member, function, class, typedef etc.
	wxString m_parent;		///< Direct parent 
	wxTreeItemId m_hti;		///< Handle to tree item
	wxString m_name;		///< Tag name (short name, excluding any scope names)
	std::map<wxString, wxString> m_extFields; ///< Additional extension fields
	
public:
	/**
	 * Construct a TagEntry from tagEntry struct and a project name.
	 * \param entry Tag entry
	 * \param project Project name
	 */
	TagEntry(const readtags::tagEntry& entry, const wxString& project);

	/**
	 * Default constructor.
	 */
	TagEntry();	

	/**
	 * Copy constructor.
	 */
	TagEntry(const TagEntry& rhs);
	
	/**
	 * Construct a tag entry from db record.
	 * \param rs Result set
	 */
	TagEntry(wxSQLite3ResultSet& rs);

	/**
	 * \param rhs Source to copy from (right hand side)
	 * \return this
	 */
	TagEntry& operator=(const TagEntry& rhs);

	/**
	 * Compare two TagEntry objects.
	 * \param rhs Right hand side
	 * \return true if identical, false otherwise
	 */
	bool operator==(const TagEntry& rhs);

	/**
	 *	Destructor
	 */
	virtual ~TagEntry();

	/**
	 * Construct a TagEntry from tagEntry struct.
	 * \param entry Tag entry
	 * \param project Project name
	 */
	void Create(const readtags::tagEntry& entry, const wxString& project);

	/**
	 * Construct a TagEntry from values.
	 * \param fileName File name
	 * \param name Tag name
	 * \param lineNumber Tag line number
	 * \param pattern Pattern
	 * \param kind Tag kind (class, struct, etc)
	 * \param extFields Map of extenstion fields (key:value)
	 * \param project Project name
	 */
	void Create(const char *fileName, 
				const char *name, 
				int lineNumber, 
				const char *pattern, 
				const char *kind, 
				std::map<wxString, wxString>& extFields, 
				const wxString& project);

	/**
	 * Test if this entry has been initialised.
	 * \return true if this tag entry has been initialised 
	 */
	const bool IsOk() const { return GetKind() != wxT("<unknown>"); }

	/**
	 * Test of this tag is a container (class, union, struct or namespace
	 */
	const bool IsContainer() const;

	//------------------------------------------
	// Operations
	//------------------------------------------
	const wxString& GetName() const { return m_name;}
	void SetName(const wxString& name) { m_name = name; }

	const wxString& GetPath() const { return m_path;}
	void SetPath(const wxString& path) { m_path = path; }

	const wxString& GetFile() const { return m_file;}
	void SetFile(const wxString& file) { m_file = file;}

	int GetLine() const { return m_lineNumber;}
	void SetLine(int line) { m_lineNumber = line; }

	const wxString& GetPattern() const { return m_pattern; }
	void SetPattern(const wxString& pattern) { m_pattern = pattern; }

	const wxString& GetKind() const { return m_kind; }
	void SetKind(const wxString& kind) { m_kind = kind; }

	const wxString& GetParent() const { return m_parent; }
	void SetParent(const wxString& parent) { m_parent = parent; }

	const wxString& GetProject() const { return m_project; }
	void SetProject(const wxString& project) { m_project = project; }

	wxTreeItemId& GetTreeItemId() { return m_hti; }
	void SetTreeItemId(wxTreeItemId& hti) { m_hti = hti; }

	wxString GetAccess() const { return GetExtField(wxT("access"));}
	wxString GetSignature() const { return GetExtField(wxT("signature")); }
	wxString GetInherits() const { return GetExtField(wxT("inherits")); }
	wxString GetTyperef() const { return GetExtField(wxT("typeref")); }

	/**
	 * \return Scope name of the tag.
	 * If path is empty in db or contains just the project name, it will return the literal <global>.
	 * For project tags, an empty string is returned.
	 */
	wxString GetScopeName() const;

	/**
	 * Generate a Key for this tag based on its attributes
	 * \return tag key 
	 */
	wxString Key() const;

	/**
	 * Generate a display name for this tag to be used by the symbol tree
	 * \return tag display name 
	 */
	wxString GetDisplayName() const;

	/**
	 * Return the actual name as described in the 'typeref' field
	 * \return real name or wxEmptyString
	 */
	wxString NameFromTyperef() const;
	//------------------------------------------
	// Extenstion fields
	//------------------------------------------
	wxString GetExtField(const wxString& extField) const 
	{ 
		std::map<wxString, wxString>::const_iterator iter = m_extFields.find(extField);
		if(iter == m_extFields.end())
			return wxEmptyString;
		return iter->second;
	}
	
	//------------------------------------------
	// Misc
	//------------------------------------------
	void Print();


	//------------------------------------------
	// Database operations
	//------------------------------------------
	/**
	 * Save this record into db.
	 * \param insertPreparedStmnt Prepared statement for insert operation
	 * \return TagOk, TagExist, TagError
	 */
	virtual int Store(wxSQLite3Statement& insertPreparedStmnt);

	/**
	 * Update this record into db.
	 * \param insertPreparedStmnt Prepared statement for insert operation
	 * \return TagOk, TagError
	 */
	virtual int Update(wxSQLite3Statement& updatePreparedStmnt);

	/**
	 * Delete this record from db.
	 * \param deletePreparedStmnt Prepared statement for delete operation
	 * \return TagOk, TagError
	 */
	virtual int Delete(wxSQLite3Statement& deletePreparedStmnt);

	
	/**
	 * \return delete preapred statement
	 */
	virtual wxString GetDeleteOneStatement();

	/**
	 * \return update preapred statement
	 */
	virtual wxString GetUpdateOneStatement();

	/**
	 * \return insert preapred statement
	 */
	virtual wxString GetInsertOneStatement();

private:
	/**
	 * Update the path with full path (e.g. namespace::class)
	 * \param path path to add
	 */
	void UpdatePath(wxString & path);
};

} // codelite

#endif // CODELITE_ENTRY_H
