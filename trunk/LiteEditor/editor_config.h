#ifndef LITEEDITOR_EDITOR_CONFIG_H
#define LITEEDITOR_EDITOR_CONFIG_H

#include "singleton.h"
#include <wx/filename.h>
#include <vector>
#include "wx/xml/xml.h"
#include "lexer_configuration.h"

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
	wxFileName m_fileName; 

public:
	bool Load(const wxFileName &fileName);
	LexerConfPtr GetLexer(const wxString& lexer);
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

	/**
	 * Save the perspective to the configuration file
	 * \param name perspective name
	 * \param pers perspective to save
	 */
	void SavePerspective(const wxString &name, const wxString &pers);

	/**
	 * Load perspective from configuration file
	 * \param name perspective name
 	 * \return perspective
	 */
	wxString LoadPerspective(const wxString &name) const ;
private:
	EditorConfig();
	virtual ~EditorConfig();

	wxXmlNode* GetLexerNode(const wxString& lexer);

};

typedef Singleton<EditorConfig> EditorConfigST;

#endif // LITEEDITOR_EDITOR_CONFIG_H
