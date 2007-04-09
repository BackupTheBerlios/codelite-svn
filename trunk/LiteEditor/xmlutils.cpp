#include "xmlutils.h"

wxXmlNode *XmlUtils::FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name)
{
	wxXmlNode *child = parent->GetChildren();
	while( child ){
		if( child->GetName() == tagName){
			if( child->GetPropVal(wxT("Name"), wxEmptyString) == name){
				return child;
			}
		}
		child = child->GetNext();
	}
	return NULL;
}

wxXmlNode *XmlUtils::FindLastByTagName(const wxXmlNode *parent, const wxString &tagName)
{
	wxXmlNode *last_node = NULL;
	wxXmlNode *child = parent->GetChildren();
	while( child ){
		if( child->GetName() == tagName){
			last_node = child;
		}
		child = child->GetNext();
	}
	return last_node;
}

