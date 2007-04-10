#ifndef PROJECT_H
#define PROJECT_H

#include "wx/string.h"
#include <wx/xml/xml.h>
#include "wx/filename.h"
#include "smart_ptr.h"
#include <list>


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

private:
	// Return the node representing a virtual dir by name
	// if no such virtual dir exist, create it.
	wxXmlNode *GetVirtualDir(const wxString &name);

	// Create virtual dir and return its xml node
	wxXmlNode *CreateVD(const wxString &name);
};

typedef SmartPtr<Project> ProjectPtr;
#endif // PROJECT_H
