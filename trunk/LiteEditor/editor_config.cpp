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
	return m_doc->Load(filename.GetFullPath());
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if( lexersNode ){
		return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName);
	}
	return NULL;
}

void EditorConfig::LoadWords(const wxString& lexer, wxString& words)
{
	if( !m_doc->IsOk() )
		return ;
		
	wxXmlNode *lexerNode = GetLexerNode(lexer);
	if( lexerNode )
	{
		wxXmlNode *node = XmlUtils::FindFirstByTagName(lexerNode, wxT("KeyWords"));
		if( node ){
			words = node->GetNodeContent();
			return;
		}
	}
}

void EditorConfig::LoadStyle(const wxString& lexer, std::vector<AttributeStyle>& styles)
{
	if( !m_doc->IsOk() )
		return ;

	wxXmlNode *lexerNode = GetLexerNode(lexer);
	if( lexerNode )
	{
		// Search for <properties>
		wxXmlNode* node = lexerNode->GetChildren();
		while( node )
		{
			if( node->GetName() == wxT("Properties") )
			{
				// We found the element, read the attributes
				wxXmlNode* prop = node->GetChildren();
				while( prop )
				{
					// Read the font attributes
					wxString Name = XmlUtils::ReadString(prop, wxT("Name"), wxT("wxSCI_C_DEFAULT"));
					wxString bold = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
					wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
					wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
					long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);
					
					AttributeStyle style = AttributeStyle(colour, fontSize, Name, face, bold == wxT("Yes"));
					styles.push_back( style );

					// read next attibute
					prop = prop->GetNext();
				}
			}
			node = node->GetNext();
		}
	}
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

CtagsOptions EditorConfig::LoadCtagsOptions() const 
{
	CtagsOptions options;
	wxXmlNode *ctagsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Ctags"));
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

	return options;
}

void EditorConfig::SaveCtagsOptions(const CtagsOptions &options)
{
	wxXmlNode *node = NULL;
	wxXmlNode *ctagsNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Ctags"));

	if( !ctagsNode ){
		// create new node
		ctagsNode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Ctags"));
		m_doc->GetRoot()->AddChild( ctagsNode );

		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
		node->AddProperty(wxT("Name"), wxT("Macros"));
		node->SetContent(options.GetIgnoreMacros());
		ctagsNode->AddChild(node);
		
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
		node->AddProperty(wxT("Name"), wxT("Language"));
		node->SetContent(options.GetLanguage());
		ctagsNode->AddChild(node);

		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
		node->AddProperty(wxT("Name"), wxT("FileSpec"));
		node->SetContent(options.GetFileSpec());
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
	}

	m_doc->Save(m_fileName.GetFullPath());
}
