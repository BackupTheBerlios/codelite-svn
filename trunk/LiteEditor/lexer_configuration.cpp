#include "lexer_configuration.h"
#include "xmlutils.h"

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
					wxString Name = XmlUtils::ReadString(prop, wxT("Name"), wxT("wxSCI_C_DEFAULT"));
					wxString bold = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
					wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
					wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
					long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);

					StyleProperty property = StyleProperty(colour, fontSize, Name, face, bold == wxT("Yes"));
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

