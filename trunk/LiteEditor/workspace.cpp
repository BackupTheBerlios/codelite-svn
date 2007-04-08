#include "workspace.h"
#include "ctags_manager.h"
#include "dirsaver.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
}

bool Workspace::OpenWorkspace(const wxString &fileName)
{
	m_fileName =  wxFileName(fileName);
	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		return false;
	}
	return true;
}

bool Workspace::CreateWorkspace(const wxString &name, const wxString &path)
{
	// Construct a DirSaver object 
	DirSaver saver;

	// If we have an open workspace, close it
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
	
	// Create new
	// Open workspace database
	m_fileName = wxFileName(path, name + wxT(".clw"));
	::wxSetWorkingDirectory(m_fileName.GetPath());

	wxFileName dbFileName(wxT("./") + name + wxT(".tags"));
	TagsManagerST::Get()->OpenDatabase(dbFileName);
	
	wxXmlNode *root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("CodeLite_Workspace"));   
	m_doc.SetRoot(root);
	m_doc.GetRoot()->AddProperty(wxT("Name"), name);
	m_doc.GetRoot()->AddProperty(wxT("Database"), dbFileName.GetFullPath());
	m_doc.GetRoot()->AddProperty(wxT("ExternalDatabase"), wxEmptyString);
	m_doc.Save(m_fileName.GetFullPath());

	return true;
}

wxString Workspace::GetStringProperty(const wxString &propName)
{
	if( !m_doc.IsOk() )
		return wxEmptyString;

	wxXmlNode *rootNode = m_doc.GetRoot();
	if( !rootNode )
		return wxEmptyString;

	return rootNode->GetPropVal(propName, wxEmptyString);
}
