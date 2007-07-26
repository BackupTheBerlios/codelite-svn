#include "archive.h"
#include <wx/xml/xml.h>

//helper functions
static wxXmlNode *FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name)
{
	if(!parent){
		return NULL;
	}

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

Archive::Archive()
: m_root(NULL)
{
}

Archive::~Archive()
{
}

void Archive::Write(const wxString &name, const wxArrayString &arr)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Name"), name);
	
	//add an entry for each wxString in the array
	for(size_t i=0; i<arr.GetCount(); i++)
	{
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
		node->AddChild(child);
		child->AddProperty(wxT("Value"), arr.Item(i));
	}
}

void Archive::Read(const wxString &name, wxArrayString &arr)
{
	if(!m_root){
		return;
	}

	wxXmlNode *node = FindNodeByName(m_root, wxT("wxArrayString"), name);
	if(node){
		//fill the output array with the values
		arr.Clear();
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("wxString")){
				wxString value;
				value = child->GetPropVal(wxT("Value"), wxEmptyString);
				arr.Add(value);
			}
			child = child->GetNext();
		}
	}
}

void Archive::Write(const wxString &name, int value)
{
	if(!m_root){
		return;
	}
	
	WriteSimple(value, wxT("int"), name);
}

void Archive::Read(const wxString &name, int &value)
{
	if(!m_root){
		return;
	}

	long v;
	ReadSimple(v, wxT("int"), name);
	value = v;
}

void Archive::Write(const wxString &name, long value)
{
	if(!m_root){
		return; 
	}
	WriteSimple(value, wxT("long"), name);
}

void Archive::Read(const wxString &name, long &value)
{
	if(!m_root){
		return; 
	}
	ReadSimple(value, wxT("long"), name);
}

void Archive::Write(const wxString &name, bool value)
{
	if(!m_root){
		return;
	}
	WriteSimple(value ? 1 : 0, wxT("bool"), name); 
}

void Archive::Read(const wxString &name, bool &value)
{
	if(!m_root){
		return; 
	}
	
	long v;
	ReadSimple(v, wxT("bool"), name);
	v  == 0 ? value = false : value = true;
}

void Archive::Write(const wxString &name, const wxString &str)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), str);
	node->AddProperty(wxT("Name"), name);
}

void Archive::Read(const wxString &name, wxString &value)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = FindNodeByName(m_root, wxT("wxString"), name);
	if(node){
		value = node->GetPropVal(wxT("Value"), wxEmptyString);
	}
}

void Archive::Read(const wxString &name, size_t &value)
{
	long v = 0;
	Read(name, v);
	value = v;
}

void Archive::Read(const wxString &name, wxFileName &fileName)
{
	wxString value;
	Read(name, value);
	fileName = wxFileName(value);
}

void Archive::Write(const wxString &name, size_t value)
{
	Write(name, (long)value);
}

void Archive::Write(const wxString &name, const wxFileName &fileName)
{
	Write(name, fileName.GetFullPath());
}

void Archive::SetXmlNode(wxXmlNode *node)
{
	m_root = node;
}

void Archive::WriteSimple(long value, const wxString &typeName, const wxString &name)
{
	if(!m_root)
		return;
		
	wxString propValue;
	propValue << value;
	
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, typeName);
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), propValue);
	node->AddProperty(wxT("Name"), name);
}

void Archive::ReadSimple(long &value, const wxString &typeName, const wxString &name)
{
	if(!m_root)
		return;
	
	value = 0;
	wxXmlNode *node = FindNodeByName(m_root, typeName, name);
	if(node){
		wxString val = node->GetPropVal(wxT("Value"), wxEmptyString);
		val.ToLong(&value);
	}
}
