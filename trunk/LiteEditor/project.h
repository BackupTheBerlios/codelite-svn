#ifndef PROJECT_H
#define PROJECT_H

#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"
#include "smart_ptr.h"
#include <list>
#include <tree.h>
#include "smart_ptr.h"

/**
 * \class ProjectItem
 * a node item that represents a displayable project item
 * 'Displayable' items are items that will be shown in the
 * FileView control
 *
 * \author Eran
 */
class ProjectItem
{
public:
	// The visible items
	enum {
		TypeVirtualDirectory,
		TypeProject,
		TypeFile
	};

public:
	wxString m_key;
	wxString m_displayName;
	wxString m_file;
	int m_kind;

public:
	//---------------------------------------------------------------
	// Constructors, destructor and assignment operator
	//---------------------------------------------------------------
	ProjectItem(const wxString &key, const wxString &displayName, const wxString &file, int kind) 
		: m_key(key)
		, m_displayName(displayName)
		, m_file(file)
		, m_kind(kind)
	{}

	ProjectItem() : m_key(wxEmptyString), m_displayName(wxEmptyString), m_file(wxEmptyString), m_kind(TypeProject)
	{}

	virtual ~ProjectItem()
	{}

	ProjectItem(const ProjectItem& item){
		*this = item;
	}

	ProjectItem &operator=(const ProjectItem &item){
		if(this == &item){
			return *this;
		}

		m_key = item.m_key;
		m_displayName = item.m_displayName;
		m_file = item.m_file;
		m_kind = item.m_kind;
		return *this;
	}
	
	//-----------------------------------------
	// Setters / Getters
	//-----------------------------------------
	const wxString &GetDisplayName() const { return m_displayName; }
	const wxString &GetFile() const { return m_file; }
	int GetKind() const { return m_kind; }

	void SetDisplayName(const wxString &displayName) { m_displayName = displayName; }
	void SetFile(const wxString &file) { m_file = file; }
	void SetKind(int kind) { m_kind = kind; }

	//------------------------------------------
	// operations
	const wxString& Key() const { return m_key; }
};

// useful typedefs
typedef Tree<wxString, ProjectItem> ProjectTree;
typedef SmartPtr<ProjectTree> ProjectTreePtr;
typedef TreeNode<wxString, ProjectItem> ProjectTreeNode;

/**
 * \class Project
 * A representation of project
 */
class Project 
{
public:
	static const wxString STATIC_LIBRARY;
	static const wxString DYMANIC_LIBRARY;
	static const wxString EXECUTABLE;

private:
	wxXmlDocument m_doc;
	wxFileName m_fileName;

public:
	// Ctor - Dtor

	// default constructor
	Project();
	virtual ~Project();

	/// Return project name
	wxString GetName() const;

	//-----------------------------------
	// Project operations
	//-----------------------------------
	bool Load(const wxString &path);
	bool Create(const wxString &name, const wxString &path, const wxString &projType = Project::STATIC_LIBRARY);
	bool AddFile(const wxString &fileName, const wxString &virtualDir = wxEmptyString);
	bool RemoveFile(const wxString &fileName);
	bool CreateVirtualDir(const wxString &name);
	bool DeleteVirtualDir(const wxString &name);

	//-----------------------------------
	// visual operations
	//-----------------------------------
	ProjectTreePtr AsTree();

private:
	// Return the node representing a virtual dir by name
	// if no such virtual dir exist, create it.
	wxXmlNode *GetVirtualDir(const wxString &name);

	// Create virtual dir and return its xml node
	wxXmlNode *CreateVD(const wxString &name);
};

typedef SmartPtr<Project> ProjectPtr;
#endif // PROJECT_H
