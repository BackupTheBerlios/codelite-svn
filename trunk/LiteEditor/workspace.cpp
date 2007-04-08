#include "workspace.h"
#include "ctags_manager.h"
#include "dirsaver.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
}

bool Workspace::OpenWorkspace(const wxString &fileName)
{
	// Construct a DirSaver object 
	DirSaver saver;

	m_fileName =  wxFileName(fileName);
	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		return false;
	}

	// Load the database
	wxString dbfile = WorkspaceST::Get()->GetStringProperty(wxT("Database"));
	wxString exDbfile = WorkspaceST::Get()->GetStringProperty(wxT("ExternalDatabase"));
	if( dbfile.IsEmpty() ){
		return false;
	}

	// the database file names are relative to the workspace,
	// convert them to absolute path
	TagsManagerST::Get()->OpenDatabase(m_fileName.GetPath() + wxT("/") + dbfile);

	// Load the external database
	if( exDbfile.IsEmpty() == false ){
		TagsManagerST::Get()->OpenExternalDatabase(m_fileName.GetPath() + wxT("/") + exDbfile);
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
