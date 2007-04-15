#ifndef XMLUTILS_H
#define XMLUTILS_H

#include "wx/xml/xml.h"

/// A collection of XML utils
class XmlUtils {
public:
	/// Find a child node by name by iterating the parent children. NULL if no childs exist
	/// \param parent  the parent node whom to be searched
	/// \param tagName the element tag name
	/// \param name	   the element 'Name' property value to search
	static wxXmlNode *FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name);

	/// Find the last child node of parent with a given name. NULL if no childs exist
	/// \param parent  the parent node whom to be searched
	/// \param name	   the element's tag name 
	static wxXmlNode *FindLastByTagName(const wxXmlNode *parent, const wxString &tagName);

	
	/**
	 * update node property. If multiple properties with the same name exist, this function updates
     * the first occurance
	 * \param node 
	 * \param name property name
	 * \param value new value to set
	 */
	static void UpdateProperty(wxXmlNode *node, const wxString &name, const wxString &value);
};

#endif // XMLUTILS_H
