#include "workspace.h"
#include "ctags_manager.h"
#include "project.h"
#include "xmlutils.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
}

bool Workspace::OpenWorkspace(const wxString &fileName, wxString &errMsg)
{
	m_fileName = wxFileName(fileName);
	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		errMsg = wxT("Corrupted workspace file");
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
	wxString dbfile = GetStringProperty(wxT("Database"), errMsg);
	wxString exDbfile = GetStringProperty(wxT("ExternalDatabase"), errMsg);
	if( dbfile.IsEmpty() ){
		errMsg = wxT("Missing 'Database' value in workspace '");
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

bool Workspace::CreateWorkspace(const wxString &name, const wxString &path, wxString &errMsg)
{
	// If we have an open workspace, close it
	if( m_doc.IsOk() ){
		if ( !m_doc.Save(m_fileName.GetFullPath()) ){
			errMsg = wxT("Failed to save current workspace");
			return false;
		}
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

wxString Workspace::GetStringProperty(const wxString &propName, wxString &errMsg)
{
	if( !m_doc.IsOk() ){
		errMsg = wxT("No workspace open");
		return wxEmptyString;
	}

	wxXmlNode *rootNode = m_doc.GetRoot();
	if( !rootNode ){
		errMsg = wxT("Corrupted workspace file");
		return wxEmptyString;
	}

	return rootNode->GetPropVal(propName, wxEmptyString);
}

bool Workspace::CreateProject(const wxString &name, const wxString &path, const wxString &type, wxString &errMsg)
{
	if( !m_doc.IsOk() ){
		errMsg = wxT("No workspace open");
		return false;
	}

	ProjectPtr proj(new Project());
	proj->Create(name, path, type);
	m_projects[name] = proj;

	// make the project path to be relative to the workspace
	wxFileName tmp(path);
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	// Add an entry to the workspace file
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
	node->AddProperty(wxT("Name"), name);
	node->AddProperty(wxT("Path"), tmp.GetFullPath());

	m_doc.GetRoot()->AddChild(node);
	m_doc.Save(m_fileName.GetFullPath());
	return true;
}
