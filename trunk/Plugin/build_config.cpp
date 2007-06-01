#include "build_config.h"
#include "xmlutils.h"
#include "wx/tokenzr.h"
#include "macros.h"
#include "project.h"
#include "editor_config.h"
#include "build_settings_config.h"

BuildConfig::BuildConfig(wxXmlNode *node)
{
	if( node ){
		m_name = XmlUtils::ReadString(node, wxT("Name"));
		m_compilerType = XmlUtils::ReadString(node, wxT("CompilerType"));
		wxXmlNode *compile = XmlUtils::FindFirstByTagName(node, wxT("Compiler"));
		
		// read the compile options
		if(compile){
			m_compilerRequired = XmlUtils::ReadBool(compile, wxT("Required"), true);
			m_compileOptions = XmlUtils::ReadString(compile, wxT("Options"));
			wxXmlNode *child = compile->GetChildren();
			while(child) {
				if(child->GetName() == wxT("IncludePath")){
					m_includePath.Add(XmlUtils::ReadString(child, wxT("Value")));
				} else if(child->GetName() == wxT("Preprocessor")){
					m_preprocessor.Add(XmlUtils::ReadString(child, wxT("Value")));
				} 
				child = child->GetNext();
			}
		}

		wxXmlNode *linker = XmlUtils::FindFirstByTagName(node, wxT("Linker"));
		// read the linker options
		if(linker){
			m_linkerRequired = XmlUtils::ReadBool(linker, wxT("Required"), true);
			m_linkOptions = XmlUtils::ReadString(linker, wxT("Options"));
			wxXmlNode *child = linker->GetChildren();
			while(child) {
				if(child->GetName() == wxT("Library")){
					m_libs.Add(XmlUtils::ReadString(child, wxT("Value")));
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
					bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));
					
					BuildCommand cmd(child->GetNodeContent(), enabled);
					m_preBuildCommands.push_back(cmd);
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
					bool enabled = XmlUtils::ReadBool(child, wxT("Enabled"));
					BuildCommand cmd(child->GetNodeContent(), enabled);
					m_postBuildCommands.push_back(cmd);
				}
				child = child->GetNext();
			}
		}

		wxXmlNode *general = XmlUtils::FindFirstByTagName(node, wxT("General"));
		if(general){
			m_outputFile = XmlUtils::ReadString(general, wxT("OutputFile"));
			m_intermediateDirectory = XmlUtils::ReadString(general, wxT("IntermediateDirectory"), wxT("."));
			m_command = XmlUtils::ReadString(general, wxT("Command"));
			m_commandArguments = XmlUtils::ReadString(general, wxT("CommandArguments"));
			m_workingDirectory = XmlUtils::ReadString(general, wxT("WorkingDirectory"), wxT("."));
		}

	}else{
		//create default project settings
		m_name = wxT("Debug");
		m_compilerRequired = true;
		m_includePath.Add(wxT("."));
		m_compileOptions = wxT("-g");
		m_linkOptions = wxT("-O0");
		m_libPath.Add(wxT("."));
		m_libPath.Add(wxT("Debug"));
		m_linkerRequired = true;
		m_intermediateDirectory = wxT("./Debug");
		m_workingDirectory = wxT("./Debug");
		m_projectType = Project::EXECUTABLE;

		BuildSettingsConfigCookie cookie;
		CompilerPtr cmp = BuildSettingsConfigST::Get()->GetFirstCompiler(cookie);
		if(cmp){
			m_compilerType = cmp->GetName();
		}
	}
}

BuildConfig::~BuildConfig()
{
}

wxString BuildConfig::NormalizePath(const wxString &path) const
{
	wxFileName fn(path);
	return fn.GetFullPath();
}

BuildConfig *BuildConfig::Clone() const
{
	wxXmlNode *node = ToXml();
	BuildConfig *cloned = new BuildConfig(node);
	delete node;
	return cloned;
}

wxXmlNode *BuildConfig::ToXml() const
{
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Configuration"));
	node->AddProperty(wxT("Name"), m_name);
	node->AddProperty(wxT("CompilerType"), m_compilerType);

	wxXmlNode *general = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("General"));
	general->AddProperty(wxT("OutputFile"), m_outputFile);
	general->AddProperty(wxT("IntermediateDirectory"), m_intermediateDirectory);
	general->AddProperty(wxT("Command"), m_command );
	general->AddProperty(wxT("CommandArguments"), m_commandArguments);
	general->AddProperty(wxT("WorkingDirectory"), m_workingDirectory);
	node->AddChild(general);

	//create the compile node
	wxXmlNode *compile = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compiler"));
	compile->AddProperty(wxT("Required"), BoolToString(m_compilerRequired));
	compile->AddProperty(wxT("Options"), m_compileOptions);
	node->AddChild(compile);

	size_t i=0;
	for(i=0; i<m_includePath.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("IncludePath"));
		option->AddProperty(wxT("Value"), m_includePath.Item(i));
		compile->AddChild(option);
	}

	for(i=0; i<m_preprocessor.GetCount(); i++){
		wxXmlNode *prep = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Preprocessor"));
		prep->AddProperty(wxT("Value"), m_preprocessor.Item(i));
		compile->AddChild(prep);
	}

	//add the link node
	wxXmlNode *link = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Linker"));
	link->AddProperty(wxT("Required"), BoolToString(m_linkerRequired));
	link->AddProperty(wxT("Options"), m_linkOptions);
	node->AddChild(link);

	for(i=0; i<m_libPath.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("LibraryPath"));
		option->AddProperty(wxT("Value"), m_libPath.Item(i));
		link->AddChild(option);
	}

	for(i=0; i<m_libs.GetCount(); i++){
		wxXmlNode *option = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Library"));
		option->AddProperty(wxT("Value"), m_libs.Item(i));
		link->AddChild(option);
	}

	//add prebuild commands
	wxXmlNode *preBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PreBuild"));
	node->AddChild(preBuild);

	BuildCommandList::const_iterator iter = m_preBuildCommands.begin();
	for(; iter != m_preBuildCommands.end(); iter++){
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
		XmlUtils::SetNodeContent(command, iter->GetCommand());
		preBuild->AddChild(command);
	}

	//add postbuild commands
	wxXmlNode *postBuild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("PostBuild"));
	node->AddChild(postBuild);
	iter = m_postBuildCommands.begin();
	for(; iter != m_postBuildCommands.end(); iter++){
		wxXmlNode *command = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Command"));
		command->AddProperty(wxT("Enabled"), BoolToString(iter->GetEnabled()));
		XmlUtils::SetNodeContent(command, iter->GetCommand());
		postBuild->AddChild(command);
	}

	return node;
}

void BuildConfig::SetPreprocessor(const wxString &pre)
{
	FillFromSmiColonString(m_preprocessor, pre);
}

void BuildConfig::SetIncludePath(const wxString &path)
{
	FillFromSmiColonString(m_includePath, path);
}

void BuildConfig::SetLibraries(const wxString &libs)
{
	FillFromSmiColonString(m_libs, libs);
}

void BuildConfig::SetLibPath(const wxString &paths)
{
	FillFromSmiColonString(m_libPath, paths);
}

void BuildConfig::FillFromSmiColonString(wxArrayString &arr, const wxString &str)
{
	arr.clear();
	wxStringTokenizer tkz(str, wxT(";"));
	while(tkz.HasMoreTokens()){
		wxString token = tkz.NextToken();
		arr.Add(token.Trim());
	}
}
// Utils function
wxString BuildConfig::ArrayToSmiColonString(const wxArrayString &array) const{
	wxString result;
	for(size_t i=0; i<array.GetCount(); i++){
		result += NormalizePath(array.Item(i));
		result += wxT(";");
	}
	return result.BeforeLast(wxT(';'));
}

wxString BuildConfig::GetLibPath() const 
{
	return ArrayToSmiColonString(m_libPath);
}

wxString BuildConfig::GetLibraries() const 
{
	return ArrayToSmiColonString(m_libs);
}

wxString BuildConfig::GetIncludePath() const 
{
	return ArrayToSmiColonString(m_includePath);
}

wxString BuildConfig::GetPreprocessor() const 
{
	return ArrayToSmiColonString(m_preprocessor);
}
