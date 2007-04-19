#ifndef LITEEDITOR_EDITOR_CONFIG_H
#define LITEEDITOR_EDITOR_CONFIG_H

#include "singleton.h"
#include <wx/filename.h>
#include <vector>
#include "wx/xml/xml.h"

class AttributeStyle
{
	wxString m_fgColour;
	long m_fontSize;
	wxString m_name;
	wxString m_faceName;
	bool m_bold;

public:
	AttributeStyle(const wxString& fgColour, const int fontSize, const wxString& name, const wxString& face, bool bold)
		: m_fgColour(fgColour)
		, m_fontSize(fontSize)
		, m_name(name)
		, m_faceName(face)
		, m_bold(bold)
	{
	};

	AttributeStyle()
		: m_fgColour(_T("BLACK"))
		, m_fontSize(10)
		, m_name(wxEmptyString)
		, m_faceName(_T("Courier"))
		, m_bold(false)
	{
	};

	AttributeStyle(const AttributeStyle& rhs)
	{
		*this = rhs;
	};

	AttributeStyle& operator=(const AttributeStyle& rhs)
	{
		m_fgColour = rhs.m_fgColour;
		m_faceName = rhs.m_faceName;
		m_bold = rhs.m_bold;
		m_fontSize = rhs.m_fontSize;
		m_name = rhs.m_name;
		return *this;
	};

	virtual ~AttributeStyle()
	{
	};

	const wxString& GetFgColour() const { return m_fgColour; }
	const long GetFontSize() const { return m_fontSize; }
	const wxString& GetFaceName() const { return m_faceName; }
	bool IsBold() const { return m_bold; }
	const wxString& GetName() const { return m_name; }
};


/**
 * \ingroup LiteEditor
 * \brief EditorConfig a singleton class that manages the liteeditor.xml configuration file
 *
 * \version 1.0
 * first version
 *
 * \date 04-19-2007
 *
 * \author Eran
 */
class EditorConfig
{
	friend class Singleton<EditorConfig>;
	wxXmlDocument* m_doc;

public:
	bool Load(const wxFileName &fileName);
	void LoadStyle(const wxString& lexer, std::vector<AttributeStyle>& styles);
	void LoadWords(const wxString& lexer, wxString& words);
	bool IsOk() const { return m_doc->IsOk(); }

	/**
	 * Load notebook style from configuration
	 * \param nbName notebook name
	 * \return wxNOT_FOUND when notebook not found or style
	 */
	long LoadNotebookStyle(wxString &nbName);

	/**
	 * Save notebook style to configuration
	 * \param nbName notebook name
	 * \param style style value to save
	 */
	void SaveNotebookStyle(wxString &nbName, long style);

private:
	EditorConfig();
	virtual ~EditorConfig();

	wxXmlNode* GetLexerNode(const wxString& lexer);

};

typedef Singleton<EditorConfig> EditorConfigST;

#endif // LITEEDITOR_EDITOR_CONFIG_H
