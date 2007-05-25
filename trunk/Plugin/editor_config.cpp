#include "editor_config.h"
#include <wx/xml/xml.h>
#include "xmlutils.h"

EditorConfig::EditorConfig()
{
	m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
	delete m_doc;
}

bool EditorConfig::Load(const wxFileName &filename)
{
	m_fileName = filename;
	m_fileName.MakeAbsolute();
	return m_doc->Load(m_fileName.GetFullPath());
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if( lexersNode ){
		return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName);
	}
	return NULL;
}

wxXmlNode* EditorConfig::GetCompilerNode(const wxString& name) const
{
	wxXmlNode *cmpsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmpsNode ){
		return XmlUtils::FindNodeByName(cmpsNode, wxT("Compiler"), name);
	}
	return NULL;
}

LexerConfPtr EditorConfig::GetLexer(const wxString &lexerName) {
	return new LexerConf(GetLexerNode(lexerName));
}

wxString EditorConfig::LoadPerspective(const wxString &Name) const
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return wxEmptyString;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Perspective") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == Name){
				return child->GetPropVal(wxT("Value"), wxEmptyString);
			}
		}
		child = child->GetNext();
	}

	return wxEmptyString;
}

void EditorConfig::SavePerspective(const wxString &name, const wxString &pers)
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Perspective") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == name){
				XmlUtils::UpdateProperty(child, wxT("Value"), pers);
				m_doc->Save(m_fileName.GetFullPath());
				return;
			}
		}
		child = child->GetNext();
	}

	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Perspective"));
	newChild->AddProperty(wxT("Name"), name);
	newChild->AddProperty(wxT("Value"), pers);
	layoutNode->AddChild(newChild);
	m_doc->Save(m_fileName.GetFullPath());
}

long EditorConfig::LoadNotebookStyle(wxString &nbName)
{
	long style = wxNOT_FOUND;
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Notebook") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == nbName){
				wxString strStyle = child->GetPropVal(wxT("Style"), wxEmptyString);
				strStyle.ToLong(&style);
				break;
			}
		}
		child = child->GetNext();
	}
	return style;
}

void EditorConfig::SaveNotebookStyle(wxString &nbName, long style)
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Notebook") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == nbName){
				wxString strStyle;
				strStyle << style;
				XmlUtils::UpdateProperty(child, wxT("Style"), strStyle);
				return;
			}
		}
		child = child->GetNext();
	}

	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Notebook"));
	newChild->AddProperty(wxT("Name"), nbName);

	wxString strStyle;
	strStyle << style;
	newChild->AddProperty(wxT("Style"), strStyle);
	layoutNode->AddChild(newChild);
	m_doc->Save(m_fileName.GetFullPath());
}

LexerConfPtr EditorConfig::GetFirstLexer(EditorConfigCookie &cookie)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if( lexersNode ){
		cookie.parent = lexersNode;
		cookie.child  = NULL;
		return GetNextLexer(cookie);
	}
	return NULL;
}

LexerConfPtr EditorConfig::GetNextLexer(EditorConfigCookie &cookie)
{
	if( cookie.parent == NULL ){
		return NULL;
	}

	if( cookie.child == NULL ){
		cookie.child = cookie.parent->GetChildren();
	}

	while( cookie.child ){
		if( cookie.child->GetName() == wxT("Lexer") ){
			wxXmlNode *n = cookie.child;
			// advance the child to the next child and bail out
			cookie.child = cookie.child->GetNext();

			// incase we dont have more childs to iterate
			// reset the parent as well so the next call to GetNexeLexer() will fail
			if( cookie.child == NULL ){
				cookie.parent = NULL;
			}
			return new LexerConf(n);
		}
		cookie.child = cookie.child->GetNext();
	}
	return NULL;
}

void EditorConfig::SetLexer(LexerConfPtr lexer)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if( !lexersNode ){
		return;
	}

	wxXmlNode *lexerNode = GetLexerNode(lexer->GetName());
	if( lexerNode ){
		lexersNode->RemoveChild( lexerNode );
		delete lexerNode;
	}
	lexersNode->AddChild( lexer->ToXml() );
	m_doc->Save(m_fileName.GetFullPath());
}

OptionsConfigPtr EditorConfig::GetOptions() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	// node can be null ...
	return new OptionsConfig(node);
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
	// locate the current node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	if( node ){
		m_doc->GetRoot()->RemoveChild(node);
		delete node;
	}

	m_doc->GetRoot()->AddChild(opts->ToXml());
	m_doc->Save(m_fileName.GetFullPath());
}

void EditorConfig::SetCompiler(CompilerPtr cmp)
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

CompilerPtr EditorConfig::GetCompiler(const wxString &name) const
{
	return new Compiler(GetCompilerNode(name));
}

CompilerPtr EditorConfig::GetFirstCompiler(EditorConfigCookie &cookie)
{
	wxXmlNode *cmps = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Compilers"));
	if( cmps ){
		cookie.parent = cmps;
		cookie.child  = NULL;
		return GetNextCompiler(cookie);
	}
	return NULL;
}

CompilerPtr EditorConfig::GetNextCompiler(EditorConfigCookie &cookie)
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

bool EditorConfig::IsCompilerExist(const wxString &name) const
{
	wxXmlNode *node = GetCompilerNode(name);
	return node != NULL;
}

void EditorConfig::DeleteCompiler(const wxString &name)
{
	wxXmlNode *node = GetCompilerNode(name);
	if(node){
		node->GetParent()->RemoveChild(node);
		delete node;
		m_doc->Save(m_fileName.GetFullPath());
	}
}
