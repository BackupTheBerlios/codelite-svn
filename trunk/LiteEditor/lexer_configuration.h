#ifndef LEXER_CONFIGURATION_H
#define LEXER_CONFIGURATION_H

#include "wx/string.h"
#include "smart_ptr.h"
#include "attribute_style.h"
#include "wx/xml/xml.h"

class LexerConf {
	StylePropertyList m_properties;
	wxXmlNode *m_element;
	wxString m_name;
	wxString m_keyWords;
	wxString m_extension;

public:
	LexerConf(wxXmlNode *element);
	wxXmlNode *ToXml() const;
	virtual ~LexerConf();

	const wxString &GetName() const { return m_name; }
	const wxString &GetKeyWords() const { return m_keyWords; }
	const wxString &GetFileSpec() const { return m_extension; }
	const StylePropertyList &GetProperties() const { return m_properties; }
	void SetProperties(StylePropertyList &properties) { m_properties = properties; }
	void SetFileSpec(const wxString &spec) { m_extension = spec; }
};

typedef SmartPtr<LexerConf> LexerConfPtr;

#endif // LEXER_CONFIGURATION_H
