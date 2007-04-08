#include "workspace.h"
#include "ctags_manager.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
}

bool Workspace::OpenWorkspace(const wxString &name, const wxString &path)
{
	m_fileName =  wxFileName(path, name + wxT(".clw"));
	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		return false;
	}
	return true;
}

bool Workspace::CreateWorkspace(const wxString &name, const wxString &path)
{
	// If we have an open workspace, close it
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
	
	// Create new

	// Open workspace database
	wxFileName dbFileName(path, name + wxT(".tags"));
	TagsManagerST::Get()->OpenDatabase(dbFileName);

	m_fileName = wxFileName(path, name + wxT(".clw"));
	wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Workspace"));   
	m_doc.SetRoot(root);
	m_doc.GetRoot()->AddProperty(wxT("Name"), name);
	m_doc.GetRoot()->AddProperty(wxT("Database"), dbFileName.GetFullPath());
	m_doc.GetRoot()->AddProperty(wxT("ExternalDatabase"), wxEmptyString);
	m_doc.Save(m_fileName.GetFullPath());
	return true;
}

