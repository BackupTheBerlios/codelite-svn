#ifndef LITEEDITOR_EDITOR_H
#define LITEEDITOR_EDITOR_H

#include <wx/wxscintilla.h>
#include <stack>
#include <map>
#include "entry.h"
#include "calltip.h"
#include "wx/filename.h"
#include "findreplacedlg.h"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>
#include "context_base.h"

class wxFindReplaceDialog;

/**
 * \ingroup LiteEditor
 * LEditor Lite Editor editing component based on Scintilla
 * LEditor provides most of the C++/C editing capablities including:
 * -# Auto Completion
 * -# Find and replace
 * -# Bookmarks
 * -# Folding
 * -# Find definition of a symbol 
 * and many many more
 *
 * \version 1.0
 * first version
 *
 * \date 04-21-2007
 *
 * \author Eran
 *
 */
class LEditor : public wxScintilla
{
	wxFileName m_fileName;
	wxString m_project;
	wxStopWatch m_watch;
	ContextBasePtr m_context;

	// static cache among editors to keep track of jumping between editors
	static std::stack<TagEntry> m_history;
	static FindReplaceDialog *m_findReplaceDlg;
	static FindReplaceData m_findReplaceData;
	int m_lastMatchPos;
	static std::map<wxString, int> ms_bookmarkShapes;

public:
	/// Construct a LEditor object
	LEditor(wxWindow* parent, wxWindowID id, const wxSize& size, const wxString& fileName, const wxString& project);

	/// Default destructor
	virtual ~LEditor();
	
	// Save the editor data into file
	bool SaveFile();

	// Save content of the editor to a given file (Save As...)
	// this function prompts the user for selecting file name
	bool SaveFileAs();

	void CompleteWord();
	
	// Return the file name opened in this editor
	const wxFileName& GetFileName() const { return m_fileName; }

	// Return the project name
	const wxString &GetProject() const { return m_project; }
	// Set the project name
	void SetProject(const wxString &proj) { m_project = proj; }

	// Attempt to display a list of members 
	// after a '.' or '->' operator has been inserted into 
	// the code
	void CodeComplete();

	// User clicked Ctrl+.
	void GotoDefinition();

	// User clicked Ctrl+,
	void GotoPreviousDefintion();

	/**
	 * Return true if editor definition contains more
	 * on its stack
	 */
	bool CanGotoPreviousDefintion() { return m_history.empty() == false; }

	// Load a file 
	void OpenFile(const wxString& fileName, const wxString& project);

	// Setters / Getters
	const wxString &GetProjectName() const { return m_project; }

	// Callback function for UI events 
	void OnUpdateUI(wxUpdateUIEvent &event);

	// Callback function for menu command events
	void OnMenuCommand(wxCommandEvent &event);

	// Return the word under the caret
	wxString GetWordUnderCursor();

	// try to match a brace from the current caret pos and select the region
	void MatchBraceAndSelect(bool selRegion);

	// Popup a find/replace dialog
	void DoFindAndReplace();

	// set this page as active, this usually happened when user changed the notebook 
	// page to this one
	void SetActive();

	// Perform FindNext operation based on the data stored in the FindReplaceData class
	void FindNext(const FindReplaceData &data);

	/**
	 * Change the document's syntax highlight
	 * \param lexerName the syntax highlight's lexer name (as appear in the liteeditor.xml file)
	 */
	void SetSyntaxHighlight(const wxString &lexerName);

	/** 
	 * Return the document context object
	 */
	ContextBasePtr GetContext() const { return m_context; }

	// Bookmark API
	//-----------------------------------------

	// Toggle marker at the current line
	void ToggleMarker();
	// Delete all markers from the current document
	void DelAllMarkers();
	// Find next marker and move cursor to that line
	void FindNextMarker();
	// Find previous marker and move cursor to that line
	void FindPrevMarker();
	// Replace all 
	bool ReplaceAll();
	// mark all occurances
	bool MarkAll();

	/** 
	 * Position caret at given position
	 * \param pos zero based offset to place the caret
	 */
	void SetCaretAt(long pos);	

	static FindReplaceDialog* GetFindReplaceDialog() { return m_findReplaceDlg; }

	// Util function
	wxChar PreviousChar(const int& pos, int &foundPos);
	wxChar NextChar(const int& pos, int &foundPos);
	int  FindString (const wxString &str, int flags, const bool down, long pos);
	void SetDirty(bool dirty);
	
	bool FindAndSelect();
	bool FindAndSelect(const FindReplaceData &data);

	bool Replace();
	bool Replace(const FindReplaceData &data);

	/**
	 * Add marker to the current line
	 */
	void AddMarker();

	/**
	 * Delete a marker from the current line
	 */
	void DelMarker();

	/**
	 * Attempt to match brace backward
	 * \param chCloseBrace the closing brace character (can be one of: '}' ')' ']') 
	 * \param pos position to start the match
	 * \param matchedPos output, the position of the matched brace
	 * \return true on match false otherwise
	 */
	bool MatchBraceBack(const wxChar& chCloseBrace, const long &pos, long &matchedPos);

	/**
	 * Return the current LEditor history stack
	 */
	static std::stack<TagEntry> &GetHistory() { return m_history; }

private:

	void SetProperties();
	void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
	void SetLineNumberWidth();
	bool SaveToFile(const wxFileName &fileName);
	void BraceMatch(const bool& bSelRegion);

	// Conevert FindReplaceDialog flags to wxSCI flags
	int  GetSciSearchFlag(const FindReplaceData &data);

	DECLARE_EVENT_TABLE()
	void OnCharAdded(wxScintillaEvent& event);
	void OnMarginClick(wxScintillaEvent& event);
	void OnChange(wxScintillaEvent& event);
	void OnDwellStart(wxScintillaEvent& event);
	void OnDwellEnd(wxScintillaEvent& event);
	void OnCallTipClick(wxScintillaEvent& event);
	void OnModified(wxScintillaEvent& event);
	void OnSciUpdateUI(wxScintillaEvent &event);
	void OnFindDialog(wxCommandEvent &event);
	
	
};

#endif // LITEEDITOR_EDITOR_H

