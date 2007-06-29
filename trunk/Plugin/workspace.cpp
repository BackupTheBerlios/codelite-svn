#include "workspace.h"
#include "ctags_manager.h"
#include "project.h"
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "macros.h"

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
	if( m_doc.IsOk() ){
		m_doc.Save(m_fileName.GetFullPath());
	}
}

wxString Workspace::GetName() const
{ 
	if(m_doc.IsOk()){
		return XmlUtils::ReadString(m_doc.GetRoot(), wxT("Name"));
	}
	return wxEmptyString;
}

bool Workspace::OpenWorkspace(const wxString &fileName, wxString &errMsg)
{
	m_fileName = wxFileName(fileName);
	m_doc.Load(m_fileName.GetFullPath());
	if( !m_doc.IsOk() ){
		errMsg = wxT("Corrupted workspace file");
		return false;
	}

	// reset the internal cache objects
	m_projects.clear();

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
	wxFileName fn(dbfile);
	wxLogMessage(wxT("Loading database: ") + m_fileName.GetPath() + wxFileName::GetPathSeparator() + fn.GetFullName());
	TagsManagerST::Get()->OpenDatabase(m_fileName.GetPath() + wxFileName::GetPathSeparator() + fn.GetFullName());
	
	wxString msg;
	msg.Printf("%x", TagsManagerST::Get()->GetDatabase());
	wxLogMessage(msg + (TagsManagerST::Get()->GetDatabase()->IsOpen() ? wxT("OPEND") : wxT("CLOSED")));

	
	// Load the external database
	if( exDbfile.IsEmpty() == false ){
		TagsManagerST::Get()->OpenExternalDatabase(m_fileName.GetPath() + wxFileName::GetPathSeparator() + exDbfile);
	}

	msg.Printf("%x", TagsManagerST::Get()->GetDatabase());
	wxLogMessage(msg + (TagsManagerST::Get()->GetDatabase()->IsOpen() ? wxT("OPEND") : wxT("CLOSED")));
	return true;
}

BuildMatrixPtr Workspace::GetBuildMatrix() const {
	return new BuildMatrix( XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("BuildMatrix")) );
}

void Workspace::SetBuildMatrix(BuildMatrixPtr mapping){
	wxXmlNode *parent = m_doc.GetRoot();
	wxXmlNode *oldMapping = XmlUtils::FindFirstByTagName(parent, wxT("BuildMatrix"));
	if(oldMapping){
		parent->RemoveChild(oldMapping);
		delete oldMapping;
	}
	parent->AddChild(mapping->ToXml());
	m_doc.Save(m_fileName.GetFullPath());
}

bool Workspace::CreateWorkspace(const wxString &name, const wxString &path, const CtagsOptions &options, wxString &errMsg)
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

	m_doc.Save(m_fileName.GetFullPath());
	//create an empty build matrix
	SetBuildMatrix(new BuildMatrix(NULL));
	SaveCtagsOptions( options );
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

void Workspace::AddProjectToBuildMatrix(ProjectPtr prj)
{
	BuildMatrixPtr matrix = GetBuildMatrix();
	std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
	for(; iter !=  wspList.end(); iter++){
		WorkspaceConfiguration::ConfigMappingList prjList = (*iter)->GetMapping();

		ProjectSettingsCookie cookie;
		BuildConfigPtr prjBldConf = prj->GetSettings()->GetFirstBuildConfiguration(cookie);
		if( !prjBldConf ){
			// the project does not have any settings, create new one and add it
			prj->SetSettings(new ProjectSettings(NULL));
			prjBldConf = prj->GetSettings()->GetFirstBuildConfiguration(cookie);
		}
		ConfigMappingEntry entry(prj->GetName(), prjBldConf->GetName());
		prjList.push_back(entry);
		(*iter)->SetConfigMappingList(prjList);
		matrix->SetConfiguration((*iter));
	}
	SetBuildMatrix(matrix);
}

void Workspace::RemoveProjectFromBuildMatrix(ProjectPtr prj)
{
	BuildMatrixPtr matrix = GetBuildMatrix();
	std::list<WorkspaceConfigurationPtr> wspList = matrix->GetConfigurations();
	std::list<WorkspaceConfigurationPtr>::iterator iter = wspList.begin();
	for(; iter !=  wspList.end(); iter++){
		WorkspaceConfiguration::ConfigMappingList prjList = (*iter)->GetMapping();

		WorkspaceConfiguration::ConfigMappingList::iterator it = prjList.begin();
		for(; it != prjList.end(); it++){
			if((*it).m_project == prj->GetName()){
				prjList.erase(it);
				break;
			}
		}

		(*iter)->SetConfigMappingList(prjList);
		matrix->SetConfiguration((*iter));
	}
	SetBuildMatrix(matrix);
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
	wxFileName tmp(path + wxFileName::GetPathSeparator() + name + wxT(".project"));
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	// Add an entry to the workspace file
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Project"));
	node->AddProperty(wxT("Name"), name);
	node->AddProperty(wxT("Path"), tmp.GetFullPath());
	
	m_doc.GetRoot()->AddChild(node);

	if( m_projects.size() == 1 ){
		SetActiveProject(name, true);
	}

	m_doc.Save(m_fileName.GetFullPath());
	AddProjectToBuildMatrix(proj);
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
		m_doc.Save(m_fileName.GetFullPath());

		AddProjectToBuildMatrix(FindProjectByName(fn.GetName(), errMsg));
		return true;
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

	//remove the associated build configuration with this
	//project
	RemoveProjectFromBuildMatrix(proj);

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

bool Workspace::CreateVirtualDirectory(const wxString &vdFullPath, wxString &errMsg)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));
	wxString projName = tkz.GetNextToken();

	wxString fixedPath;
	// Construct new path excluding the first token
	size_t count = tkz.CountTokens();

	for(size_t i=0; i<count-1; i++){
		fixedPath += tkz.GetNextToken();
		fixedPath += wxT(":");
	}
	fixedPath += tkz.GetNextToken();

	ProjectPtr proj = FindProjectByName(projName, errMsg);
	return proj->CreateVirtualDir(fixedPath);
}

bool Workspace::RemoveVirtualDirectory(const wxString &vdFullPath, wxString &errMsg)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));
	wxString projName = tkz.GetNextToken();

	wxString fixedPath;
	// Construct new path excluding the first token
	size_t count = tkz.CountTokens();

	for(size_t i=0; i<count-1; i++){
		fixedPath += tkz.GetNextToken();
		fixedPath += wxT(":");
	}
	fixedPath += tkz.GetNextToken();

	ProjectPtr proj = FindProjectByName(projName, errMsg);
	return proj->DeleteVirtualDir(fixedPath);
}

void Workspace::Save()
{
	if( m_doc.IsOk() ){
		std::map<wxString, ProjectPtr>::iterator iter = m_projects.begin();
		for(; iter != m_projects.end(); iter ++){
			iter->second->Save();
		}
		m_doc.Save(m_fileName.GetFullPath());
	}
}

bool Workspace::AddNewFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));
	wxString projName = tkz.GetNextToken();
	wxString fixedPath;
	// Construct new path excluding the first token
	size_t count = tkz.CountTokens();

	for(size_t i=0; i<count-1; i++){
		fixedPath += tkz.GetNextToken();
		fixedPath += wxT(":");
	}
	fixedPath += tkz.GetNextToken();

	ProjectPtr proj = FindProjectByName(projName, errMsg);
	if( !proj ){
		errMsg = wxT("No such project");
		return false;
	}

	return proj->AddFile(fileName, fixedPath);
}

bool Workspace::RemoveFile(const wxString &vdFullPath, const wxString &fileName, wxString &errMsg)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));
	wxString projName = tkz.GetNextToken();
	wxString fixedPath;

	// Construct new path excluding the first token
	size_t count = tkz.CountTokens();

	for(size_t i=0; i<count-1; i++){
		fixedPath += tkz.GetNextToken();
		fixedPath += wxT(":");
	}
	fixedPath += tkz.GetNextToken();

	ProjectPtr proj = FindProjectByName(projName, errMsg);
	if( !proj ){
		errMsg = wxT("No such project");
		return false;
	}

	return proj->RemoveFile(fileName, fixedPath);
}


CtagsOptions Workspace::LoadCtagsOptions() const 
{
	CtagsOptions options;
	if( m_doc.IsOk() == false )
		return options;

	wxXmlNode *ctagsNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Ctags"));
	if( !ctagsNode ){
		return options;
	}
	
	wxXmlNode *node = NULL;
	node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("Macros"));
	if( node ){
		options.SetIgnoreMacros(node->GetNodeContent());
	}

	node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("Language"));
	if( node ){
		options.SetLanguage(node->GetNodeContent());
	}

	node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("FileSpec"));
	if( node ){
		options.SetFileSpec(node->GetNodeContent());
	}

	node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("ParseComments"));
	if( node ){
		options.SetParseComments(node->GetNodeContent() == wxT("yes"));
	}
	return options;
}

void Workspace::SaveCtagsOptions(const CtagsOptions &options)
{
	if( m_doc.IsOk() == false )
		return;

	wxXmlNode *node = NULL;
	wxXmlNode *ctagsNode = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Ctags"));

	if( !ctagsNode ){
		// create new node
		ctagsNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Ctags"));
		m_doc.GetRoot()->AddChild( ctagsNode );

		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		node->AddProperty(wxT("Name"), wxT("Macros"));
		XmlUtils::SetNodeContent(node, options.GetIgnoreMacros());
		ctagsNode->AddChild(node);
		
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		node->AddProperty(wxT("Name"), wxT("Language"));
		XmlUtils::SetNodeContent(node, options.GetLanguage());
		ctagsNode->AddChild(node);

		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		node->AddProperty(wxT("Name"), wxT("FileSpec"));
		XmlUtils::SetNodeContent(node, options.GetFileSpec());
		ctagsNode->AddChild(node);

		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		node->AddProperty(wxT("Name"), wxT("ParseComments"));
		XmlUtils::SetNodeContent(node, BoolToString(options.GetParseComments()));
		ctagsNode->AddChild(node);

	} else {
		node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("Macros"));
		if( node ){
			XmlUtils::SetNodeContent(node, options.GetIgnoreMacros());
		}

		node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("Language"));
		if( node ){
			XmlUtils::SetNodeContent(node, options.GetLanguage());
		}

		node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("FileSpec"));
		if( node ){
			XmlUtils::SetNodeContent(node, options.GetFileSpec());
		}

		node = XmlUtils::FindNodeByName(ctagsNode, wxT("Option"), wxT("ParseComments"));
		if( node ){
			XmlUtils::SetNodeContent(node, BoolToString(options.GetParseComments()));
		}
	}

	m_doc.Save(m_fileName.GetFullPath());
}

EnvironmentVarieblesPtr Workspace::GetEnvironmentVariables() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
	return new EnvironmentVariebles(node);
}

void Workspace::SetEnvironmentVariables(EnvironmentVarieblesPtr env)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Environment"));
	if(node){
		m_doc.GetRoot()->RemoveChild(node);
		delete node;
	}
	m_doc.GetRoot()->AddChild(env->ToXml());
	m_doc.Save(m_fileName.GetFullPath());
}

BuildConfigPtr Workspace::GetProjSelBuildConf(const wxString &projectName) const
{
	BuildMatrixPtr matrix = GetBuildMatrix();
	wxString workspaceConfig = matrix->GetSelectedConfigurationName();
	wxString projConf = matrix->GetProjectSelectedConf(workspaceConfig, projectName);

	//Get the project setting and retrieve the selected configuration
	wxString errMsg;
	ProjectPtr proj = FindProjectByName(projectName, errMsg);
	if( proj ){
		ProjectSettingsPtr settings = proj->GetSettings();
		if(settings){
			return settings->GetBuildConfiguration(projConf);
		}
	}
	return NULL;
}
