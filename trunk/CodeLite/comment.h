#ifndef CODELITE_COMMENT_H
#define CODELITE_COMMENT_H

#include "db_record.h"

namespace codelite
{
class Comment : public DbRecord
{
	wxString m_comment;
	wxString m_file;
	int m_line;

public:
	/**
	 * construct a Comment object
	 * \param comment comment string
	 * \param file comment file name
	 * \param line comment line number
	 */
	Comment(const wxString &comment, const wxString & file, const int line);

	/**
	 * Copy constructor
	 */
	Comment(const Comment& rhs);

	/**
	 * Construct Comment object from database record
	 * \param rs resutl set
	 */
	Comment(wxSQLite3ResultSet& rs);
	
	/**
	 * Destructor
	 */
	virtual ~Comment(){};

	/// asgginment operator
	Comment& operator=(const Comment& rhs);

	/// accessors
	/**
	 * Get the file name
	 * \return file name
	 */
	const wxString & GetFile() const { return m_file; }

	/**
	 * Get string comment
	 * \return comment
	 */
	const wxString & GetComment() const { return m_comment; }

	/**
	 * Get the line number
	 * \return line number
	 */
	const int & GetLine() const { return m_line; }

	/// Database operation
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
};

typedef SmartPtr<Comment> CommentPtr;

} // codelite
#endif // CODELITE_COMMENT_H
