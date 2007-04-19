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
					long   fontSize;
					wxString colour;

					// Read the font attributes
					wxString name = prop->GetPropVal(wxT("name"), wxT("wxSCI_C_DEFAULT"));
					wxString bold = prop->GetPropVal(wxT("bold"), wxT("no"));
					wxString size = prop->GetPropVal(wxT("size"), wxT("10"));
					wxString face = prop->GetPropVal(wxT("face"), wxT("Courier"));

					size.ToLong(&fontSize);
					colour = prop->GetPropVal(wxT("colour"), wxT("black"));

					AttributeStyle style = AttributeStyle(colour, fontSize, name, face, bold == wxT("yes"));
					styles.push_back( style );

					// read next attibute
					prop = prop->GetNext();
				}
			}
			node = node->GetNext();
		}
	}
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
	}

	// If we reached here, there is no such node ... so create one
	wxXmlNode *parent = XmlUtils::FindLastByTagName(layoutNode, wxT("Notebook"));
	if( !parent ){
		return;
	}

	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Notebook"));
	newChild->AddProperty(wxT("Name"), nbName);

	wxString strStyle;
	strStyle << style;
	newChild->AddProperty(wxT("Style"), strStyle);
	parent->AddChild(newChild);
}

