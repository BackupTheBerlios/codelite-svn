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

wxXmlNode *XmlUtils::FindFirstByTagName(const wxXmlNode *parent, const wxString &tagName)
{
	if( !parent ){
		return NULL;
	}

	wxXmlNode *child = parent->GetChildren();
	while( child ){
		if( child->GetName() == tagName){
			return child;
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

void XmlUtils::UpdateProperty(wxXmlNode *node, const wxString &name, const wxString &value)
{
	wxXmlProperty *prop = node->GetProperties();
	while (prop){
		if( prop->GetName() == name ){
			prop->SetValue(value);
			return;
		}
		prop = prop->GetNext();
	}

	// No such property, create new one and add it
	node->AddProperty(name, value);
}
