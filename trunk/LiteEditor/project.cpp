#include "project.h"

const wxString Project::STATIC_LIBRARY = wxT("STATIC_LIBRARY");
const wxString Project::DYMANIC_LIBRARY = wxT("DYMANIC_LIBRARY");
const wxString Project::EXECUTABLE = wxT("EXECUTABLE");

Project::Project()
{
}

Project::~Project()
{
}

bool Project::Create(const wxString &name, const wxFileName &path, const wxString &projType)
{
	m_fileName = path;
	wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Project"));   
	m_doc.SetRoot(root);
	m_doc.GetRoot()->AddProperty(wxT("Name"), name);
	m_doc.GetRoot()->AddProperty(wxT("Type"), projType);

	// Create the default virtual directory
	wxXmlNode *vd = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	vd->AddProperty(wxT("Name"), wxT("Default"));
	m_doc.GetRoot()->AddChild(vd);

	m_doc.Save(m_fileName.GetFullPath());
	return true;
}

bool Project::Load(const wxFileName &path)
{
	if( !m_doc.Load(path.GetFullPath()) ){
		return false;
	}

	m_fileName = path;
	return true;
}

wxXmlNode *Project::GetVirtualDir(const wxString &name)
{
	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while (child) {
		if (child->GetName() == wxT("VirtualDirectory")) {
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == name){
				return child;
			}
		}
		child = child->GetNext();
	}

	// No match found, create new node and return it
	return CreateVD(name);
}

wxXmlNode *Project::CreateVD(const wxString &name)
{
	wxXmlNode *node = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	node->AddProperty(wxT("Name"), name);
	m_doc.GetRoot()->AddChild(node);
	return node;
}

bool Project::AddFile(const wxFileName &fileName, const wxString &virtualDir)
{
	wxXmlNode *vd = GetVirtualDir(virtualDir);
	if( !vd ){
		return false;
	}

	// Convert the file path to be relative to 
	// the project path
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	wxXmlNode *node = new wxXmlNode(vd, wxXML_ELEMENT_NODE, wxT("File"));
	node->AddProperty(wxT("Name"), tmp.GetFullPath());
	vd->AddChild(node);
	return true;
}

bool Project::CreateVirtualDir(const wxString &name)
{
	return CreateVD(name) != NULL;
}

bool Project::DeleteVirtualDir(const wxString &name)
{
	return true;
}


bool Project::RemoveFile(const wxFileName &fileName)
{
	return true;
}

