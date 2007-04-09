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

	/// Inserts the child node after before_node in the children list. If before_node is NULL, then child is prepended to the list of children and becomes the first child of this node. 
	// Returns true if before_node has been found and the child node has been inserted.
	/// \param parent		the parent node 
	/// \param child		the node to add
	/// \param insertBefore	insert the node before this node, if set to NULL, insert it as first child
	static bool InsertChild(wxXmlNode *parent, wxXmlNode *child, wxXmlNode *insertBefore);
};

#endif // XMLUTILS_H