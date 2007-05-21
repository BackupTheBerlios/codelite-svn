#include "project.h"
#include "xmlutils.h"
#include <wx/tokenzr.h>
#include "wx/arrstr.h"
#include "dirsaver.h"

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
	m_fileName.MakeAbsolute();

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
	//create build settings
	SetSettings(new ProjectSettings(NULL));
	return true;
}

bool Project::Load(const wxString &path)
{
	if( !m_doc.Load(path) ){
		return false;
	}

	m_fileName = path;
	m_fileName.MakeAbsolute();
	return true;
}

wxXmlNode *Project::GetVirtualDir(const wxString &vdFullPath)
{
	wxStringTokenizer tkz(vdFullPath, wxT(":"));
	
	wxXmlNode *parent = m_doc.GetRoot();
	while( tkz.HasMoreTokens() ){
		parent = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), tkz.GetNextToken());
		if( !parent ){
			return NULL;
		}
	}
	return parent;
}

wxXmlNode *Project::CreateVD(const wxString &vdFullPath)
{
	wxXmlNode *oldVd = GetVirtualDir(vdFullPath);
	if( oldVd ){
		// VD already exist
		return oldVd;
	}

	wxStringTokenizer tkz(vdFullPath, wxT(":"));

	wxXmlNode *parent = m_doc.GetRoot();
	size_t count = tkz.CountTokens();
	for(size_t i=0; i<count-1; i++){
		parent = XmlUtils::FindNodeByName(parent, wxT("VirtualDirectory"), tkz.GetNextToken());
		if( !parent ){
			return NULL;
		}
	}

	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("VirtualDirectory"));
	node->AddProperty(wxT("Name"), tkz.GetNextToken());
	
	parent->AddChild(node);
	m_doc.Save(m_fileName.GetFullPath());
	return node;
}

bool Project::AddFile(const wxString &fileName, const wxString &virtualDirPath)
{
	wxXmlNode *vd = GetVirtualDir(virtualDirPath);
	if( !vd ){
		return false;
	}

	// Convert the file path to be relative to 
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());
	
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
	node->AddProperty(wxT("Name"), tmp.GetFullPath());
	vd->AddChild(node);
	return m_doc.Save(m_fileName.GetFullPath());;
}

bool Project::CreateVirtualDir(const wxString &vdFullPath)
{
	return CreateVD(vdFullPath) != NULL;
}

bool Project::DeleteVirtualDir(const wxString &vdFullPath)
{
	wxXmlNode *vd = GetVirtualDir(vdFullPath);
	if( vd ){
		wxXmlNode *parent = vd->GetParent();
		if( parent ){
			parent->RemoveChild( vd );
		}

		delete vd;
		return m_doc.Save(m_fileName.GetFullPath());
	}
	return false;
}

bool Project::RemoveFile(const wxString &fileName, const wxString &virtualDir)
{
	wxXmlNode *vd = GetVirtualDir(virtualDir);
	if( !vd ){
		return false;
	}

	// Convert the file path to be relative to 
	// the project path
	DirSaver ds;

	::wxSetWorkingDirectory(m_fileName.GetPath());
	wxFileName tmp(fileName);
	tmp.MakeRelativeTo(m_fileName.GetPath());

	wxXmlNode *node = XmlUtils::FindNodeByName(vd, wxT("File"), tmp.GetFullPath());
	if( node ){
		node->GetParent()->RemoveChild( node );
		delete node;
	}
	return m_doc.Save(m_fileName.GetFullPath());;
}

wxString Project::GetName() const
{
	return m_doc.GetRoot()->GetPropVal(wxT("Name"), wxEmptyString);
}

ProjectTreePtr Project::AsTree()
{
	ProjectItem item(GetName(), GetName(), wxEmptyString, ProjectItem::TypeProject);
	ProjectTreePtr ptp(new ProjectTree(item.Key(), item));
	
	wxXmlNode *child = m_doc.GetRoot()->GetChildren();
	while( child ){
		RecursiveAdd(child, ptp, ptp->GetRoot());
		child = child->GetNext();
	}
	return ptp;
}

void Project::RecursiveAdd(wxXmlNode *xmlNode, ProjectTreePtr &ptp, ProjectTreeNode *nodeParent)
{
	// Build the key for this node
	std::list<wxString> nameList;

	wxXmlNode *parent = xmlNode->GetParent();
	while( parent ){
		nameList.push_front(parent->GetPropVal(wxT("Name"), wxEmptyString));
		parent = parent->GetParent();
	}

	wxString key;
	for(size_t i=0; i<nameList.size(); i++){
		key += nameList.front();
		key += wxT(":");
		nameList.pop_front();
	}
	key += xmlNode->GetPropVal(wxT("Name"), wxEmptyString);

	// Create the tree node data
	ProjectItem item;
	if( xmlNode->GetName() == wxT("Project") ){
		item = ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeProject);
	} else if( xmlNode->GetName() == wxT("VirtualDirectory") ){
		item = ProjectItem(key, xmlNode->GetPropVal(wxT("Name"), wxEmptyString), wxEmptyString, ProjectItem::TypeVirtualDirectory);
	} else if( xmlNode->GetName() == wxT("File") ){
		wxFileName filename(xmlNode->GetPropVal(wxT("Name"), wxEmptyString));
		item = ProjectItem(key, filename.GetFullName(), filename.GetFullPath(), ProjectItem::TypeFile);
	} else {
		// un-recognised or not viewable item in the tree,
		// skip it and its children
		return;
	}

	ProjectTreeNode *newNode = ptp->AddChild(item.Key(), item, nodeParent);
	// This node has children, add them as well
	wxXmlNode *children = xmlNode->GetChildren();

	while( children ){
		RecursiveAdd(children, ptp, newNode);
		children = children->GetNext();
	}
}

void Project::Save()
{
	if( m_doc.IsOk() )
		m_doc.Save(m_fileName.GetFullPath());
}

void Project::GetFilesByVirtualDir(const wxString &vdFullPath, wxArrayString &files)
{
	wxXmlNode *vd = GetVirtualDir(vdFullPath);
	if( vd ){
		wxXmlNode *child = vd->GetChildren();
		while( child ){
			if( child->GetName() == wxT("File")){
				wxFileName fileName(
					child->GetPropVal(wxT("Name"), wxEmptyString)
					);
				fileName.MakeAbsolute(m_fileName.GetPath());
				files.Add(fileName.GetFullPath());
			}
			child = child->GetNext();
		}
	}
}

void Project::GetFiles(std::vector<wxFileName> &files)
{
	GetFiles(m_doc.GetRoot(), files);
}

void Project::GetFiles(wxXmlNode *parent, std::vector<wxFileName> &files)
{
	if( !parent ){
		return;
	}

	wxXmlNode *child = parent->GetChildren();
	while (child) {
		if(child->GetName() == wxT("File")){
			wxString fileName = child->GetPropVal(wxT("Name"), wxEmptyString);
			wxFileName tmp(fileName);
			files.push_back(tmp);
		} else if(child->GetChildren()){// we could also add a check for VirtualDirectory only 
			GetFiles(child, files);
		}
		child = child->GetNext();
	}
}

ProjectSettingsPtr Project::GetSettings() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
	return new ProjectSettings(node);
}

void Project::SetSettings(ProjectSettingsPtr settings)
{
	wxXmlNode *oldSettings = XmlUtils::FindFirstByTagName(m_doc.GetRoot(), wxT("Settings"));
	if(oldSettings){
		oldSettings->GetParent()->RemoveChild(oldSettings);
		delete oldSettings;
	}
	m_doc.GetRoot()->AddChild(settings->ToXml());
	m_doc.Save(m_fileName.GetFullPath());
}
