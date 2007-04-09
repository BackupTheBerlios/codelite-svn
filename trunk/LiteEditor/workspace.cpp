#include "workspace.h"
#include "ctags_manager.h"
#include "project.h"

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
	m_fileName =  wxFileName(fileName);

	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		return false;
	}

	// This function sets the working directory to the workspace directory!
	::wxSetWorkingDirectory(m_fileName.GetPath());

	// Load all projects
	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while(child){
		if(child->GetName() == wxT("Project")){
			wxString projectPath = child->GetPropVal(wxT("Path"), wxEmptyString);
			ProjectPtr proj(new Project());
			proj->Load(projectPath);
			m_projects[proj->GetName()] = proj;
		}
		child = child->GetNext();
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
	// If we have an open workspace, close it
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
	
	// Create new
	// Open workspace database
	m_fileName = wxFileName(path, name + wxT(".workspace"));

	// This function sets the working directory to the workspace directory!
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

bool Workspace::CreateProject(const wxString &name, const wxString &path, const wxString &type)
{
	ProjectPtr proj(new Project());
	proj->Create(name, path, type);
	m_projects[name] = proj;

	// make the project path to be relative to the workspace
	wxFileName tmp(path);
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	// find the last project in this workspace and append the node after it
	wxXmlNode *last_proj = NULL;
	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Project")){
			last_proj = child;
		}
		child = child->GetNext();
	}
	
	// Add an entry to the workspace file
	wxXmlNode *node = new wxXmlNode(m_doc.GetRoot(), wxXML_ELEMENT_NODE, wxT("Project"));
	node->AddProperty(wxT("Name"), name);
	node->AddProperty(wxT("Path"), tmp.GetFullPath());
	if( !last_proj ){
		m_doc.GetRoot()->AddChild(node);
	} else {
		m_doc.GetRoot()->InsertChild(node, last_proj);
	}
	m_doc.Save(m_fileName.GetFullPath());
	return true;
}
