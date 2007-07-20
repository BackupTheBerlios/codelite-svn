#ifndef LITEEDITOR_EDITOR_CONFIG_H
#define LITEEDITOR_EDITOR_CONFIG_H

#include "singleton.h"
#include <wx/filename.h>
#include <vector>
#include "wx/xml/xml.h"
#include "lexer_configuration.h"
#include "optionsconfig.h"
#include "map"
#include "serialized_object.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

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
class WXDLLIMPEXP_LE_SDK EditorConfig
{
	friend class Singleton<EditorConfig>;
	wxXmlDocument* m_doc;
	wxFileName m_fileName; 
	std::map<wxString, LexerConfPtr> m_lexers;
	
public:
	typedef std::map<wxString, LexerConfPtr>::const_iterator ConstIterator;

	/**
	 * Load the configuration file
	 * \param fileName configuration file name
	 * \return true on success false otherwise
	 */
	bool Load();

	/**
	 * Find lexer configuration and return a pointer to a LexerConf object
	 * \param lexer lexer name (e.g. Cpp, Java, Default etc..)
	 * \return LexerConfPtr 
	 */
	LexerConfPtr GetLexer(const wxString& lexer);

	/**
	 * Return iterator to the begin of the undelying lexer mapping	 
 	 */
	ConstIterator LexerBegin();

	/**
	 * Return iterator to the end of the undelying lexer mapping
	 */
	ConstIterator LexerEnd();

	/**
	 * Set a lexer to the configuration file
	 */
	void SetLexer(LexerConfPtr lexer);

	/**
	 * Test if this configuration is loaded properly
	 * \return true of a file is loaded into the configuration manager false otherwise
	 */
	bool IsOk() const { return m_doc->IsOk(); }

	/**
	 * Load notebook style from configuration
	 * \param nbName notebook name
	 * \return wxNOT_FOUND when notebook not found or style
	 */
	long LoadNotebookStyle(const wxString &nbName);

	/**
	 * Save notebook style to configuration
	 * \param nbName notebook name
	 * \param style style value to save
	 */
	void SaveNotebookStyle(const wxString &nbName, long style);

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

	/**
	 * Read the editor options from the configuration file
	 * and return them as an object
	 */
	OptionsConfigPtr GetOptions() const;

	/**
	 * Set options to the configuration file, override them if they does not exist
	 */
	void SetOptions(OptionsConfigPtr opts);

	/**
	 * Return the database that should be used by the editor
	 * \return 
	 */
	wxString GetTagsDatabase() const;

	/**
	 * Set tags database to be use by editor (in addition to the workspace one)
	 * \param &dbName 
	 */
	void SetTagsDatabase(const wxString &dbName);

	/**
	 * save lexers settings
	 */
	void SaveLexers();

	/**
	 * get an array of recently opened files
	 * \param files  [output] a place holder for the output
	 */
	void GetRecentlyOpenedFies(wxArrayString &files);

	/**
	 * get an array of recently opened files
	 * \param files  list of files to save
	 */
	void SetRecentlyOpenedFies(const wxArrayString &files);

	/**
	 * get an array of recently opened workspaces
	 * \param files  [output] a place holder for the output
	 */
	void GetRecentlyOpenedWorkspaces(wxArrayString &files);

	/**
	 * get an array of recently opened workspaces
	 * \param files  list of files to save
	 */
	void SetRecentlyOpenedWorkspaces(const wxArrayString &files);

	/**
	 * \brief write an archived object to the xml configuration
	 * \param name object name
	 * \param arch the archived object container
	 */
	bool WriteObject(const wxString &name, SerializedObject *obj);
	
	/**
	 * \brief read an archived object from the configuration
	 * \param name object to read
	 * \param arch [output]
	 */
	bool ReadObject(const wxString &name, SerializedObject *obj);
	
private:
	EditorConfig();
	virtual ~EditorConfig();
	wxXmlNode *GetLexerNode(const wxString& lexer);
};

typedef Singleton<EditorConfig> EditorConfigST;
#endif // LITEEDITOR_EDITOR_CONFIG_H
