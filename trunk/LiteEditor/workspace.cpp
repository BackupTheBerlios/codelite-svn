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

			if( !DoAddProject(projectPath, errMsg) ){
				return false;
			}
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
	
	if( name.IsEmpty() ){
		errMsg = wxT("Invalid workspace name");
		return false;
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
	wxFileName tmp(path + wxT("/") + name + wxT(".project"));
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	// Add an entry to the workspace file
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
	node->AddProperty(wxT("Name"), name);
	node->AddProperty(wxT("Path"), tmp.GetFullPath());

	m_doc.GetRoot()->AddChild(node);
	m_doc.Save(m_fileName.GetFullPath());
	return true;
}

ProjectPtr Workspace::FindProjectByName(const wxString &projName, wxString &errMsg) const
{
	if( !m_doc.IsOk() ){
		errMsg = wxT("No workspace open");
		return NULL;
	}

	std::map<wxString, ProjectPtr>::const_iterator iter = m_projects.find(projName);
	if( iter == m_projects.end() ){
		errMsg = wxT("Invalid project name '");
		errMsg << projName << wxT("'");
		return NULL;
	}
	return iter->second;
}

void Workspace::GetProjectList(wxArrayString &list)
{
	std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
	for(; iter != m_projects.end(); iter++){
		list.Add(iter->first);	// add the name
	}
}

bool Workspace::AddProject(const wxString & path, wxString &errMsg)
{
	if( !m_doc.IsOk() ){
		errMsg = wxT("No workspace open");
		return false;
	}

	wxFileName fn(path);
	if( !fn.FileExists() ){
		errMsg = wxT("File does not exist");
		return false;
	}

	// Try first to find this project in the workspace
	ProjectPtr proj = FindProjectByName(fn.GetName(), errMsg);
	if( !proj ){
		errMsg.Empty();
		bool res = DoAddProject(path, errMsg);		
		if( !res ){
			return false;
		}

		// Add an entry to the workspace filewxFileName tmp(path);
		fn.MakeRelativeTo(m_fileName.GetPath());

		wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
		node->AddProperty(wxT("Name"), fn.GetName());
		node->AddProperty(wxT("Path"), fn.GetFullPath());
		node->AddProperty(wxT("Active"), m_projects.size() == 1 ? wxT("Yes") : wxT("No"));
		m_doc.GetRoot()->AddChild(node);
		return m_doc.Save(m_fileName.GetFullPath());
	}

	errMsg = wxT("A project with this name already exist in the workspace");
	return false;
}


bool Workspace::DoAddProject(const wxString &path, wxString &errMsg)
{
	// Add the project
	ProjectPtr proj(new Project());
	if( !proj->Load(path) ){
		errMsg = wxT("Corrupted project file '");
		errMsg << path << wxT("'");
		return false;
	}
	// Add an entry to the projects map
	m_projects[proj->GetName()] = proj;
	return true;
}

bool Workspace::RemoveProject(const wxString &name, wxString &errMsg)
{
	ProjectPtr proj = FindProjectByName(name, errMsg);
	if( !proj ){
		return false;
	}

	// remove the project from the internal map
	std::map<wxString, ProjectPtr>::iterator iter = m_projects.find(proj->GetName());
	if( iter != m_projects.end() ){
		m_projects.erase(iter);
	}

	// update the xml file
	wxXmlNode *root = m_doc.GetRoot();
	wxXmlNode *child = root->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name ){
			if( child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0){
				// the removed project was active, 
				// select new project to be active
				if( !m_projects.empty() ){
					std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
					SetActiveProject(iter->first, true);
				}
			}
			root->RemoveChild( child );
			delete child;
			break;
		}
		child = child->GetNext();
	}
	return m_doc.Save( m_fileName.GetFullPath() );
}

wxString Workspace::GetActiveProjectName()
{
	if( !m_doc.IsOk() ){
		return wxEmptyString;
	}

	wxXmlNode *root = m_doc.GetRoot();
	wxXmlNode *child = root->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Active"), wxEmptyString).CmpNoCase(wxT("Yes")) == 0){
			return child->GetPropVal(wxT("Name"), wxEmptyString);
		}
		child = child->GetNext();
	}
	return wxEmptyString;
}

void Workspace::SetActiveProject(const wxString &name, bool active)
{
	if( !m_doc.IsOk() )
		return;

	// update the xml file
	wxXmlNode *root = m_doc.GetRoot();
	wxXmlNode *child = root->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Project") && child->GetPropVal(wxT("Name"), wxEmptyString) == name ){
			XmlUtils::UpdateProperty(child, wxT("Active"), active ? wxT("Yes") : wxT("No"));
			break;
		}
		child = child->GetNext();
	}
	m_doc.Save( m_fileName.GetFullPath() );
}
