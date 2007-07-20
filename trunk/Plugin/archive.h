#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "wx/string.h"
#include <wx/arrstr.h>

class wxXmlNode;

/**
 * \class Archive 
 * \brief an auxulariy class which serializes variables into XML format
 * \author Eran
 * \date 07/20/07
 */
class Archive
{
	wxXmlNode *m_root;
	
public:
	Archive();
	virtual ~Archive();
	
	/**
	 * \brief set a root node for this Archive object, all Write operations will append their nodes
	 * to this node 
	 * \param node wxXmlNode to act as the root
	 */
	void SetXmlNode(wxXmlNode *node);
	
	//--------------------
	// Write API
	//--------------------
	void WriteInt(const wxString &name, int value);
	void WriteBool(const wxString &name, bool value);
	void WriteLong(const wxString &name, long value);
	void WriteString(const wxString &name, const wxString &str);
	void WriteArrayString(const wxString &name, const wxArrayString &arr);

	//--------------------
	// Read API
	//--------------------
	void ReadInt(const wxString &name, int &value);
	void ReadBool(const wxString &name, bool &value);
	void ReadLong(const wxString &name, long &value);
	void ReadString(const wxString &name, wxString &value);
	void ReadArrayString(const wxString &name, wxArrayString &arr);
	
private:	
	void WriteSimple(int value, const wxString &typeName, const wxString &name);
	void ReadSimple(int &value, const wxString &typeName, const wxString &name);
};
#endif //ARCHIVE_H
