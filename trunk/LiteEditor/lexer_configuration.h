#ifndef LEXER_CONFIGURATION_H
#define LEXER_CONFIGURATION_H

#include "wx/string.h"
#include <list>
#include "smart_ptr.h"
#include "attribute_style.h"
#include "wx/xml/xml.h"

class LexerConf {
	wxXmlNode *m_element;
	wxString m_name;
	wxString m_keyWords;
	wxString m_extension;
	std::list<StyleProperty> m_properties;

public:
	LexerConf(wxXmlNode *element);
	wxXmlNode *ToXml() const;
	virtual ~LexerConf();

	const wxString &GetName() const { return m_name; }
	const wxString &GetKeyWords() const { return m_keyWords; }
	const wxString &GetFileSpec() const { return m_extension; }
	const std::list<StyleProperty> &GetProperties() const { return m_properties; }
	void SetProperties(std::list<StyleProperty> &properties) { m_properties = properties; }
};

typedef SmartPtr<LexerConf> LexerConfPtr;

#endif // LEXER_CONFIGURATION_H
