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
	wxXmlNode *lexerNode = GetLexerNode(lexer->GetName());
	if( lexerNode ){
		m_doc->GetRoot()->RemoveChild( lexerNode );
		delete lexerNode;
	}
	m_doc->GetRoot()->AddChild( lexer->ToXml() );
	m_doc->Save(m_fileName.GetFullPath());
}

