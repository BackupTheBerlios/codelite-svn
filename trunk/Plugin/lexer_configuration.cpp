#include "lexer_configuration.h"
#include "xmlutils.h"
#include "macros.h"

LexerConf::LexerConf(wxXmlNode *element)
: m_element(element)
{
	if( m_element ){
		// read the lexer name
		m_name = m_element->GetPropVal(wxT("Name"), wxEmptyString);

		// load key words
		wxXmlNode *node = XmlUtils::FindFirstByTagName(m_element, wxT("KeyWords"));
		if( node ){
			m_keyWords = node->GetNodeContent();
		}

		// load extensions
		node = XmlUtils::FindFirstByTagName(m_element, wxT("Extensions"));
		if( node ){
			m_extension = node->GetNodeContent();
		}

		// load properties
		// Search for <properties>
		node = XmlUtils::FindFirstByTagName(m_element, wxT("Properties"));
		if( node )
		{
			// We found the element, read the attributes
			wxXmlNode* prop = node->GetChildren();
			while( prop )
			{
				if(prop->GetName() == wxT("Property")){
					// Read the font attributes
					wxString Name = XmlUtils::ReadString(prop, wxT("Name"), wxT("DEFAULT"));
					wxString bold = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
					wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
					wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
					long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);

					StyleProperty property = StyleProperty(colour, fontSize, Name, face, bold.CmpNoCase(wxT("Yes")) == 0);
					m_properties.push_back( property );
				}
				prop = prop->GetNext();
			}
		}
	}
}

LexerConf::~LexerConf()
{
}

wxXmlNode *LexerConf::ToXml() const
{
	//convert the lexer back xml node
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Lexer"));
	//set the lexer name
	node->AddProperty(wxT("Name"), GetName());
	
	//set the keywords node
	wxXmlNode *keyWords = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords"));
	XmlUtils::SetNodeContent(keyWords, GetKeyWords());
	node->AddChild(keyWords);

	//set the extensions node
	wxXmlNode *extesions = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Extensions"));
	XmlUtils::SetNodeContent(extesions, GetFileSpec());
	node->AddChild(extesions);
	
	//set the properties
	wxXmlNode *properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Properties"));
	std::list<StyleProperty>::const_iterator iter = m_properties.begin();
	for(; iter != m_properties.end(); iter ++){
		StyleProperty p = (*iter);
		wxXmlNode *property = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Property"));
		property->AddProperty(wxT("Name"), p.GetName());
		property->AddProperty(wxT("Bold"), BoolToString(p.IsBold()));
		property->AddProperty(wxT("Face"), p.GetFaceName());
		property->AddProperty(wxT("Colour"), p.GetFgColour());

		wxString strSize;
		strSize << p.GetFontSize();
		property->AddProperty(wxT("Size"), strSize);
		properties->AddChild(property);
	}
	node->AddChild( properties );
	return node;
}

