#include "build_settings_config.h"
#include "xmlutils.h"

BuildSettingsConfig::BuildSettingsConfig()
{
	m_doc = new wxXmlDocument();
}

BuildSettingsConfig::~BuildSettingsConfig()
{
	delete m_doc;
}

bool BuildSettingsConfig::Load(const wxFileName &filename)
{
	m_fileName = filename;
	m_fileName.MakeAbsolute();
	return m_doc->Load(m_fileName.GetFullPath());
}

wxXmlNode* BuildSettingsConfig::GetCompilerNode(const wxString& name) const
{
	wxXmlNode *cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmpsNode ){
		return XmlUtils::FindNodeByName(cmpsNode, wxT("Compiler"), name);
	}
	return NULL;
}

void BuildSettingsConfig::SetCompiler(CompilerPtr cmp)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if(node){
		wxXmlNode *oldCmp = NULL;
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("Compiler") && XmlUtils::ReadString(child, wxT("Name")) == cmp->GetName()){
				oldCmp = child;
				break;
			}
			child = child->GetNext();
		}
		if(oldCmp){
			node->RemoveChild(oldCmp);
			delete oldCmp;
		}
		node->AddChild(cmp->ToXml());

	} else {
		wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Compilers"));
		m_doc->GetRoot()->AddChild(node);
		node->AddChild(cmp->ToXml());
	}
	
	m_doc->Save(m_fileName.GetFullPath());
}

CompilerPtr BuildSettingsConfig::GetCompiler(const wxString &name) const
{
	return new Compiler(GetCompilerNode(name));
}

CompilerPtr BuildSettingsConfig::GetFirstCompiler(BuildSettingsConfigCookie &cookie)
{
	wxXmlNode *cmps = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmps ){
		cookie.parent = cmps;
		cookie.child  = NULL;
		return GetNextCompiler(cookie);
	}
	return NULL;
}

CompilerPtr BuildSettingsConfig::GetNextCompiler(BuildSettingsConfigCookie &cookie)
{
	if( cookie.parent == NULL ){
		return NULL;
	}

	if( cookie.child == NULL ){
		cookie.child = cookie.parent->GetChildren();
	}

	while( cookie.child ){
		if( cookie.child->GetName() == wxT("Compiler") ){
			wxXmlNode *n = cookie.child;
			// advance the child to the next child and bail out
			cookie.child = cookie.child->GetNext();

			// incase we dont have more childs to iterate
			// reset the parent as well so the next call to GetNexeLexer() will fail
			if( cookie.child == NULL ){
				cookie.parent = NULL;
			}
			return new Compiler(n);
		}
		cookie.child = cookie.child->GetNext();
	}
	return NULL;
}

bool BuildSettingsConfig::IsCompilerExist(const wxString &name) const
{
	wxXmlNode *node = GetCompilerNode(name);
	return node != NULL;
}

void BuildSettingsConfig::DeleteCompiler(const wxString &name)
{
	wxXmlNode *node = GetCompilerNode(name);
	if(node){
		node->GetParent()->RemoveChild(node);
		delete node;
		m_doc->Save(m_fileName.GetFullPath());
	}
}

