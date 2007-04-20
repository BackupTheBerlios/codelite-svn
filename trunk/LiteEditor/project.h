#ifndef PROJECT_H
#define PROJECT_H

#include "wx/wx.h"
#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"
#include <tree.h>
#include "smart_ptr.h"
#include <list>

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
	{
	}

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
 * \ingroup LiteEditor
 *
 *
 * \date 04-15-2007
 *
 * \author Eran
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \todo 
 *
 * \bug 
 *
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
	const wxFileName &GetFileName() const { return m_fileName; }

	// Ctor - Dtor

	// default constructor
	Project();
	virtual ~Project();

	/**
	 * \return project name
	 */
	wxString GetName() const;

	//-----------------------------------
	// Project operations
	//-----------------------------------
	/**
	 * Load project from file
	 * \param path 
	 * \return 
	 */
	bool Load(const wxString &path); 
	/**
	 * Create new project 
	 * \param name project name
	 * \param path path of the file excluding  the file name (e.g. C:\)
	 * \param projType project type: Project::STATIC_LIBRARY, Project::DYMANIC_LIBRARY, Project::EXECUTABLE
	 * \return 
	 */
	bool Create(const wxString &name, const wxString &path, const wxString &projType = Project::STATIC_LIBRARY);

	/**
	 * Add file to the project
	 * \param fileName file full name and path
	 * \param virtualDir owner virtual directory, if the virtual directory does not exist, a new one will be created
	 *        and the file will be placed under it
	 * \return 
	 */
	bool AddFile(const wxString &fileName, const wxString &virtualDir = wxEmptyString);
	/**
	 * Remove file from the project
	 * \param fileName file full path
	 * \param virtualDir owner virtual directory
	 * \return 
	 */
	bool RemoveFile(const wxString &fileName, const wxString &virtualDir = wxEmptyString);

	/**
	 * Create new virtual directory
	 * \param vdFullPath VD path to add
	 * \return 
	 */
	bool CreateVirtualDir(const wxString &vdFullPath);

	/**
	 * remove a virtual directory
	 * \param vdFullPath VD path to remove
	 * \return 
	 */
	bool DeleteVirtualDir(const wxString &vdFullPath);

	/**
	 * Return list of files by a virtual directory
	 * \param vdFullPath virtual directory
	 * \param files [output] list of files under this vdFullPath. The files format are in absolute path!
	 */
	void GetFilesByVirtualDir(const wxString &vdFullPath, wxArrayString &files);

	/**
	 * Save project settings
	 */
	void Save();


	/**
	 * Return list of files in this project
	 * \param files 
	 */
	void GetFiles(std::vector<wxFileName> &files);

	/**
	 * Return list of files in this project
	 * \param files 
	 */
	void GetFiles(wxArrayString &files);

	//-----------------------------------
	// visual operations
	//-----------------------------------
	ProjectTreePtr AsTree();

private:
	// Recursive helper function
	void RecursiveAdd(wxXmlNode *xmlNode, ProjectTreePtr &ptp, ProjectTreeNode *nodeParent);

	// Return the node representing a virtual dir by name
	// if no such virtual dir exist, create it.
	wxXmlNode *GetVirtualDir(const wxString &vdFullPath);

	// Create virtual dir and return its xml node
	wxXmlNode *CreateVD(const wxString &vdFullPath);

	void GetFiles(wxXmlNode *parent, std::vector<wxFileName> &files);
};

typedef SmartPtr<Project> ProjectPtr;
#endif // PROJECT_H
