#include "build_config.h"
#include "xmlutils.h"

BuildConfig::BuildConfig(wxXmlNode *node)
{
	if( node ){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		wxXmlNode *compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
		// read the compile options
		if(compile){
			m_compilerRequired = XmlUtils::ReadBool(compile, wxT("Required"), true);
			wxXmlNode *child = compile->GetChildren();
			while(child) {
				if(child->GetName() == wxT("IncludePath")){
					m_includePath.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if(child->GetName() == wxT("Option")){
					m_compileOptions.Add(XmlUtils::ReadString(child, wxT("Value")));
				}
				child = child->GetNext();
			}
		}

		wxXmlNode *linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
		// read the linker options
		if(linker){
			m_linkerRequired = XmlUtils::ReadBool(linker, wxT("Required"), true);
			wxXmlNode *child = linker->GetChildren();
			while(child) {
				if(child->GetName() == wxT("Library")){
					m_libs.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if(child->GetName() == wxT("Option")){
					m_linkOptions.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if(child->GetName() == wxT("LibraryPath")){
					m_libPath.Add(XmlUtils::ReadString(child, wxT("Value")));
				}
				child = child->GetNext();
			}
		}

		// read the prebuild commands
		wxXmlNode *preBuild = XmlUtils::FindFirstByTagName(node, wxT("PreBuild"));
		if(preBuild){
			wxXmlNode *child = preBuild->GetChildren();
			while(child) {
				if(child->GetName() == wxT("Command")){
					m_preBuildCommands.Add(child->GetNodeContent());
				}
				child = child->GetNext();
			}
		}
		// read the postbuild commands
		wxXmlNode *postBuild = XmlUtils::FindFirstByTagName(node, wxT("PostBuild"));
		if(postBuild){
			wxXmlNode *child = postBuild->GetChildren();
			while(child) {
				if(child->GetName() == wxT("Command")){
					m_postBuildCommands.Add(child->GetNodeContent());
				}
				child = child->GetNext();
			}
		}
	}
}

BuildConfig::~BuildConfig()
{
}

wxXmlNode *BuildConfig::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Configuration"));
	node->AddProperty(wxT("Name"), m_name);
	//create the compile node
	wxXmlNode *compile = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	compile->AddProperty(wxT("Required"), m_compilerRequired ? wxT("yes") : wxT("no"));
	node->AddChild(compile);
	size_t i=0;
	for(i=0; i<m_compileOptions.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		option->AddProperty(wxT("Value"), m_compileOptions.Item(i));
		compile->AddChild(option);
	}

	for(i=0; i<m_includePath.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
		option->AddProperty(wxT("Value"), m_includePath.Item(i));
		compile->AddChild(option);
	}
	//add the link node
	wxXmlNode *link = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Linker"));
	compile->AddProperty(wxT("Required"), m_linkerRequired ? wxT("yes") : wxT("no"));
	node->AddChild(link);
	for(i=0; i<m_linkOptions.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Option"));
		option->AddProperty(wxT("Value"), m_linkOptions.Item(i));
		link->AddChild(option);
	}

	for(i=0; i<m_libPath.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LibraryPath"));
		option->AddProperty(wxT("Value"), m_libPath.Item(i));
		link->AddChild(option);
	}

	for(i=0; i<m_libPath.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Library"));
		option->AddProperty(wxT("Value"), m_libs.Item(i));
		link->AddChild(option);
	}

	//add prebuild commands
	wxXmlNode *preBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PreBuild"));
	node->AddChild(preBuild);
	for(i=0; i<m_preBuildCommands.GetCount(); i++){
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		XmlUtils::SetNodeContent(command, m_preBuildCommands.Item(i));
		preBuild->AddChild(command);
	}

	//add postbuild commands
	wxXmlNode *postBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PostBuild"));
	node->AddChild(postBuild);
	for(i=0; i<m_postBuildCommands.GetCount(); i++){
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		XmlUtils::SetNodeContent(command, m_postBuildCommands.Item(i));
		postBuild->AddChild(command);
	}
	return node;
}
