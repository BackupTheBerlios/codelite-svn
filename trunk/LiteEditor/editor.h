#ifndef LITEEDITOR_EDITOR_H
#define LITEEDITOR_EDITOR_H

#include <wx/wxscintilla.h>
#include <stack>
#include "entry.h"
#include "calltip.h"
#include "wx/filename.h"

//------------------------------------------------------------------------------
// The editor:
// I tried to provide a real life example for using the AutoCompletion
// mechanism, so i derived a class from wxScintilla
//------------------------------------------------------------------------------

class LEditor : public wxScintilla
{
	wxFileName m_fileName;
	wxString m_project;
	wxStopWatch m_watch;
	std::map<wxString, int> m_propertyInt;

	// static cache among editors to keep track of jumping between editors
	static std::stack<TagEntry> m_history;
	CallTipPtr m_ct;
	
	enum TipKind
	{
		TipNone = -1,
		TipHover,
		TipFuncProto
	};

	enum CalltipClickPos
	{
		Elsewhere = 0,
		ArrowUp ,
		ArrowDown
	};

	TipKind m_tipKind;

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

	// Attempt to display a list of members 
	// after a '.' or '->' operator has been inserted into 
	// the code
	void CodeComplete();

	// User clicked Ctrl+.
	void GotoDefinition();

	// User clicked Ctrl+,
	void GotoPreviousDefintion();

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
protected:

	// Util function
	wxChar PreviousChar(const int& pos, int &foundPos);
	wxChar NextChar(const int& pos, int &foundPos);
	int  FindString (const wxString &str, int flags, const bool down, long pos);
	bool IsCommentOrString(long pos);
	void SetDirty(bool dirty);

private:
	void SetProperties();
	void DefineMarker(int marker, int markerType, wxColor fore, wxColor back);
	void GetWordAndScope(wxString& word, wxString &scope, wxString &scopeName);
	void SetLineNumberWidth();
	bool SaveToFile(const wxFileName &fileName);
	void AutoIndent(const wxChar& nChar);
	bool MatchBraceBack(const wxChar& chCloseBrace, const long &pos, long &matchedPos);
	void BraceMatch(const bool& bSelRegion);
	

	DECLARE_EVENT_TABLE()
	void OnCharAdded(wxScintillaEvent& event);
	void OnMarginClick(wxScintillaEvent& event);
	void OnChange(wxScintillaEvent& event);
	void OnDwellStart(wxScintillaEvent& event);
	void OnDwellEnd(wxScintillaEvent& event);
	void OnCallTipClick(wxScintillaEvent& event);
	void OnModified(wxScintillaEvent& event);
	void OnSciUpdateUI(wxScintillaEvent &event);
	
};

#endif // LITEEDITOR_EDITOR_H
