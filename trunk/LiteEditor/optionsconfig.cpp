#include "optionsconfig.h"
#include "xmlutils.h"

OptionsConfig::OptionsConfig(wxXmlNode *node)
{
	if( node ){
		m_displayFoldMargin = XmlUtils::ReadBool(node, wxT("DisplayFoldMargin"));
		m_underlineFoldLine = XmlUtils::ReadBool(node, wxT("UnderlineFoldedLine"));
		m_foldStyle = XmlUtils::ReadString(node, wxT("FoldStyle"));
		m_displayBookmarkMargin = XmlUtils::ReadBool(node, wxT("DisplayBookmarkMargin"));
	}
}

OptionsConfig::~OptionsConfig(void)
{
}

wxXmlNode *OptionsConfig::ToXml() const
{
	return NULL;
}
