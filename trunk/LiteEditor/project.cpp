#include "project.h"
#include "xmlutils.h"

const wxString Project::STATIC_LIBRARY = wxT("Static Library");
const wxString Project::DYMANIC_LIBRARY = wxT("Dynamic Library");
const wxString Project::EXECUTABLE = wxT("Executable");

Project::Project()
{
}

Project::~Project()
{
}

bool Project::Create(const wxString &name, const wxString &path, const wxString &projType)
{
	m_fileName = path + wxT("/") + name + wxT(".project");
	wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Project"));   
	m_doc.SetRoot(root);
	m_doc.GetRoot()->AddProperty(wxT("Name"), name);
	m_doc.GetRoot()->AddProperty(wxT("Type"), projType);

	// Create the default virtual directories
	wxXmlNode *srcNode = NULL, *headNode = NULL;

	srcNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	srcNode->AddProperty(wxT("Name"), wxT("Source Files"));
	m_doc.GetRoot()->AddChild(srcNode);

	headNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	headNode->AddProperty(wxT("Name"), wxT("Header Files"));
	m_doc.GetRoot()->AddChild(headNode);

	m_doc.Save(m_fileName.GetFullPath());
	return true;
}

bool Project::Load(const wxString &path)
{
	if( !m_doc.Load(path) ){
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
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	node->AddProperty(wxT("Name"), name);
	
	m_doc.GetRoot()->AddChild(node);
	return node;
}

bool Project::AddFile(const wxString &fileName, const wxString &virtualDir)
{
	wxXmlNode *vd = GetVirtualDir(virtualDir);
	if( !vd ){
		return false;
	}

	// Convert the file path to be relative to 
	// the project path
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
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
	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while( child ){
		if( child->GetName() == wxT("VirtualDirectory")){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == name){
				m_doc.GetRoot()->RemoveChild(child);
				delete child;
				return true;
			}
		}
	}
	return false;
}

bool Project::RemoveFile(const wxString &fileName)
{
	wxUnusedVar(fileName);
	return true;
}

wxString Project::GetName() const
{
	return m_doc.GetRoot()->GetPropVal(wxT("Name"), wxEmptyString);
}
