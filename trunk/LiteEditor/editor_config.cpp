#include "editor_config.h"
#include <wx/xml/xml.h>

EditorConfig::EditorConfig()
{
	m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
	delete m_doc;
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexer)
{
	wxXmlNode *child = m_doc->GetRoot()->GetChildren();
	while( child )
	{
		// Search for lexer element
		if( child->GetName() == lexer )
			return child;
		child = child->GetNext();
	}
	return NULL;
}

void EditorConfig::LoadWords(const wxFileName& fileName, const wxString& lexer, wxString& words)
{
	if( !m_doc->IsOk() )
		if (!m_doc->Load(fileName.GetFullPath()))
			return ;
		
	wxXmlNode *lexerNode = GetLexerNode(lexer);
	if( lexerNode )
	{
		// Search for <key_words>
		wxXmlNode* node = lexerNode->GetChildren();
		while( node )
		{
			if( node->GetName() == _T("key_words") )
			{
				wxXmlNode *n = node->GetChildren();
				while (n)
				{
					if (n->GetType() == wxXML_TEXT_NODE ||
						n->GetType() == wxXML_CDATA_SECTION_NODE)
					{
						words = n->GetContent();
						return;
					}
					n = n->GetNext();
				}
				return;
			}
			node = node->GetNext();
		}
	}
}

void EditorConfig::LoadStyle(const wxFileName& fileName, const wxString& lexer, std::vector<AttributeStyle>& styles)
{
	if( !m_doc->IsOk() )
		if (!m_doc->Load(fileName.GetFullPath()))
			return ;

	wxXmlNode *lexerNode = GetLexerNode(lexer);
	if( lexerNode )
	{
		// Search for <properties>
		wxXmlNode* node = lexerNode->GetChildren();
		while( node )
		{
			if( node->GetName() == _T("Properties") )
			{
				// We found the element, read the attributes
				wxXmlNode* prop = node->GetChildren();
				while( prop )
				{
					long   fontSize;
					wxString colour;

					// Read the font attributes
					wxString name = prop->GetPropVal(_T("name"), _T("wxSCI_C_DEFAULT"));
					wxString bold = prop->GetPropVal(_T("bold"), _T("no"));
					wxString size = prop->GetPropVal(_T("size"), _T("10"));
					wxString face = prop->GetPropVal(_T("face"), _T("Courier"));

					size.ToLong(&fontSize);
					colour = prop->GetPropVal(_T("colour"), _T("black"));

					AttributeStyle style = AttributeStyle(colour, fontSize, name, face, bold == _T("yes"));
					styles.push_back( style );

					// read next attibute
					prop = prop->GetNext();
				}
			}
			node = node->GetNext();
		}
	}
}
