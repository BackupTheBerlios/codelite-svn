#ifndef LITEEDITOR_EDITOR_CONFIG_H
#define LITEEDITOR_EDITOR_CONFIG_H

#include "singleton.h"
#include <wx/filename.h>
#include <vector>
#include "wx/xml/xml.h"
#include "lexer_configuration.h"
#include "optionsconfig.h"
#include "compiler.h"

// Cookie class for the editor to provide reentrance operations
// on various methods (such as iteration)
class EditorConfigCookie {
public:
	wxXmlNode *child;	
	wxXmlNode *parent;

public:
	EditorConfigCookie() : child(NULL), parent(NULL) {}
	~EditorConfigCookie() {}
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
	wxFileName m_fileName; 

public:
	/**
	 * Load the configuration file
	 * \param fileName configuration file name
	 * \return true on success false otherwise
	 */
	bool Load(const wxFileName &fileName);

	/**
	 * Find lexer configuration and return a pointer to a LexerConf object
	 * \param lexer lexer name (e.g. Cpp, Java, Default etc..)
	 * \return LexerConfPtr 
	 */
	LexerConfPtr GetLexer(const wxString& lexer);

	/**
	 * Returns the first lexer found.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	LexerConfPtr GetFirstLexer(EditorConfigCookie &cookie);

	/**
	 * Returns the next lexer.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	LexerConfPtr GetNextLexer(EditorConfigCookie &cookie);

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

	/**
	 * Read the editor options from the configuration file
	 * and return them as an object
	 */
	OptionsConfigPtr GetOptions() const;

	/**
	 * Set options to the configuration file, override them if they does not exist
	 */
	void SetOptions(OptionsConfigPtr opts);

	//-------------------------------------------------------------
	// Compilers
	//-------------------------------------------------------------

	/**
	 * Set or update a given compiler using its name as the index
	 */
	void SetCompiler(CompilerPtr cmp);

	/**
	 * Find and return compiler by name
	 */
	CompilerPtr GetCompiler(const wxString &name) const;

	/**
	 * Returns the first compiler found.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	CompilerPtr GetFirstCompiler(EditorConfigCookie &cookie);

	/**
	 * Returns the next compiler.
	 * For this enumeration function you must pass in a 'cookie' parameter which is opaque for the application but is necessary for the library to make these functions reentrant 
	 * (i.e. allow more than one enumeration on one and the same object simultaneously).
	 */
	CompilerPtr GetNextCompiler(EditorConfigCookie &cookie);

private:
	EditorConfig();
	virtual ~EditorConfig();
	wxXmlNode *GetLexerNode(const wxString& lexer);
	wxXmlNode *GetCompilerNode(const wxString& name) const;
};

typedef Singleton<EditorConfig> EditorConfigST;
#endif // LITEEDITOR_EDITOR_CONFIG_H
