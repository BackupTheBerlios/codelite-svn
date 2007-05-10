#include "build_config.h"
#include "xmlutils.h"

BuildConfig::BuildConfig(wxXmlNode *node)
{
	if( node ){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		wxXmlNode *compile = XmlUtils::FindFirstByTagName(node, wxT("Compile"));
		// read the compile options
		if(compile){
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

		wxXmlNode *linker = XmlUtils::FindFirstByTagName(node, wxT("Link"));
		// read the linker options
		if(linker){
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
	return NULL;
}
