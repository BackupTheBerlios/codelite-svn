#ifndef LEXER_CONFIGURATION_H
#define LEXER_CONFIGURATION_H

#include "wx/string.h"
#include "attribute_style.h"
#include "wx/xml/xml.h"
#include "configuration_object.h"

class LexerConf : public ConfObject {
	StylePropertyList m_properties;
	int m_lexerId;
	wxString m_name;
	wxString m_keyWords;
	wxString m_extension;

public:
	LexerConf(wxXmlNode *element);
	wxXmlNode *ToXml() const;
	virtual ~LexerConf();

	/**
	 * Get the lexer ID, which should be in sync with values of Scintilla
	 * \return 
	 */
	int GetLexerId() const { return m_lexerId; }

	/**
	 * Set the lexer ID
	 * \param id 
	 */
	void SetLexerId(int id) { m_lexerId = id; }

	/**
	 * Return the lexer description as described in the XML file
	 */
	const wxString &GetName() const { return m_name; }
	/**
	 * Return the lexer keywords
	 * \return 
	 */
	const wxString &GetKeyWords() const { return m_keyWords; }

	/**
	 * File patterns that this lexer should apply to
	 */
	const wxString &GetFileSpec() const { return m_extension; }
	/**
	 * Return a list of the lexer properties
	 * \return 
	 */
	const StylePropertyList &GetProperties() const { return m_properties; }
	/**
	 * Set the lexer properties
	 * \param &properties 
	 */
	void SetProperties(StylePropertyList &properties) { m_properties = properties; }
	/**
	 * Set file spec for the lexer
	 * \param &spec 
	 */
	void SetFileSpec(const wxString &spec) { m_extension = spec; }
};

typedef SmartPtr<LexerConf> LexerConfPtr;

#endif // LEXER_CONFIGURATION_H
