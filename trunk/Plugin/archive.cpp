#include "archive.h"
#include <wx/xml/xml.h>
#include "xmlutils.h"

Archive::Archive()
: m_root(NULL)
{
}

Archive::~Archive()
{
}

void Archive::WriteArrayString(const wxString &name, const wxArrayString &arr)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxArrayString"));
	m_root->AddChild(node);
	
	wxString count;
	count << arr.GetCount();
	node->AddProperty(wxT("Name"), name);
	
	//add an entry for each wxString in the array
	for(size_t i=0; i<arr.GetCount(); i++)
	{
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
		node->AddChild(child);
		child->AddProperty(wxT("Value"), arr.Item(i));
	}
}

void Archive::ReadArrayString(const wxString &name, wxArrayString &arr)
{
	if(!m_root){
		return;
	}

	wxXmlNode *node = XmlUtils::FindNodeByName(m_root, wxT("wxArrayString"), name);
	if(node){
		//fill the output array with the values
		arr.Clear();
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("wxString")){
				arr.Add(XmlUtils::ReadString(child, wxT("Value")));
			}
			child = child->GetNext();
		}
	}
}

void Archive::WriteInt(const wxString &name, int value)
{
	if(!m_root){
		return;
	}
	
	WriteSimple(value, wxT("int"), name);
}

void Archive::ReadInt(const wxString &name, int &value)
{
	if(!m_root){
		return;
	}
	
	ReadSimple(value, wxT("int"), name);
}

void Archive::WriteLong(const wxString &name, long value)
{
	if(!m_root){
		return; 
	}
	WriteSimple((int)value, wxT("long"), name);
}

void Archive::ReadLong(const wxString &name, long &value)
{
	if(!m_root){
		return; 
	}
	
	int intValue;
	ReadSimple(intValue, wxT("long"), name);
	value = intValue;
}

void Archive::WriteBool(const wxString &name, bool value)
{
	if(!m_root){
		return;
	}
	WriteSimple((int)value, wxT("bool"), name); 
}

void Archive::ReadBool(const wxString &name, bool &value)
{
	if(!m_root){
		return; 
	}
	
	int intValue;
	ReadSimple(intValue, wxT("bool"), name);
	intValue  == 0 ? value = false : value = true;
}

void Archive::WriteString(const wxString &name, const wxString &str)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("wxString"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), str);
	node->AddProperty(wxT("Name"), name);
}

void Archive::ReadString(const wxString &name, wxString &value)
{
	if(!m_root){
		return;
	}
	wxXmlNode *node = XmlUtils::FindNodeByName(m_root, wxT("wxString"), name);
	if(node){
		value = XmlUtils::ReadString(node, wxT("Value"));
	}
}

void Archive::SetXmlNode(wxXmlNode *node)
{
	m_root = node;
}

void Archive::WriteSimple(int value, const wxString &typeName, const wxString &name)
{
	if(!m_root)
		return;
		
	wxString propValue;
	propValue << value;
	
	wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("typeName"));
	m_root->AddChild(node);
	node->AddProperty(wxT("Value"), propValue);
	node->AddProperty(wxT("Name"), name);
}

void Archive::ReadSimple(int &value, const wxString &typeName, const wxString &name)
{
	if(!m_root)
		return;
		
	wxXmlNode *node = XmlUtils::FindNodeByName(m_root, typeName, name);
	if(node){
		value = XmlUtils::ReadLong(node, wxT("Value"));
	}
}
