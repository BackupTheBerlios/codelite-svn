#ifndef FIND_REPLACE_DLG_H
#define FIND_REPLACE_DLG_H

#include "wx/dialog.h"

class wxTextCtrl;
class wxCheckBox;
class wxButton;
class wxStaticText;

#define wxFRD_MATCHCASE						0x00000001
#define wxFRD_MATCHWHOLEWORD				0x00000002
#define wxFRD_REGULAREXPRESSION				0x00000004
#define wxFRD_SEARCHUP						0x00000008
#define wxFRD_WRAPSEARCH					0x00000010

#ifndef WXDLLIMPEXP_LE
	#ifdef WXMAKINGDLL
	#    define WXDLLIMPEXP_LE WXIMPORT
	#elif defined(WXUSINGDLL)
	#    define WXDLLIMPEXP_LE WXEXPORT
	#else 
	#    define WXDLLIMPEXP_LE
	#endif // WXDLLIMPEXP_LE
#endif

DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE, wxEVT_FRD_FIND_NEXT, -1)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE, wxEVT_FRD_CLOSE, -1)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACE, -1)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE, wxEVT_FRD_REPLACEALL, -1)
DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_LE, wxEVT_FRD_BOOKMARKALL, -1)

class FindReplaceData 
{
	wxArrayString m_replaceString;
	wxArrayString m_findString;
	size_t	m_flags;
	
public:
	FindReplaceData() 
		: m_flags(0)
	{}

	// Copy ctor
	FindReplaceData(const FindReplaceData& src)
	{
		*this = src;
	}

	// assignment operator
	FindReplaceData& operator=(const FindReplaceData& src)
	{
		if(this == &src)
			return *this;

		// Copy fields
		m_replaceString = src.m_replaceString;
		m_findString = src.m_findString;
		m_flags = src.m_flags;
		return *this;
	}

	virtual ~FindReplaceData()
	{}

	// Setters/Getters
	const size_t GetFlags() const { return m_flags; }
	void SetFlags(size_t flags) { m_flags = flags; }
	
	wxArrayString &GetFindStringArr() { return m_findString; }
	wxArrayString &GetReplaceStringArr() { return m_replaceString; }
	
	/**
	 * \brief return the first find string on the array 
	 */
	wxString GetFindString() const {
		if(m_findString.IsEmpty()){
			return wxEmptyString;
		}else{
			return m_findString.Item(0);
		}
	}
	
	/**
	 * \brief return the first replace string from the array
	 */
	wxString GetReplaceString() const{
		if(m_replaceString.IsEmpty()){
			return wxEmptyString;
		}else{
			return m_replaceString.Item(0);
		}
	}
	
	/**
	 * \brief add find string to the array and make it first item on the array as well
	 * \param str find string to add
	 */
	void SetFindString(const wxString &str){
		int where = m_findString.Index(str);
		if(where != wxNOT_FOUND){
			m_findString.RemoveAt(where);
		}			
		m_findString.Insert(str, 0);
	}

	/**
	 * \brief add replace string to the array and make it first item on the array as well
	 * \param str replace string to add
	 */
	void SetReplaceString(const wxString &str){
		int where = m_replaceString.Index(str);
		if(where != wxNOT_FOUND){
			m_replaceString.RemoveAt(where);
		}			
		m_replaceString.Insert(str, 0);
	}
	
};

class wxStaticText;

class FindReplaceDialog : public wxDialog
{
	wxEvtHandler *m_owner;

	FindReplaceData m_data;

	// Options
	wxTextCtrl *m_findString;
	wxTextCtrl *m_replaceString;
	wxCheckBox *m_matchCase;
	wxCheckBox *m_matchWholeWord;
	wxCheckBox *m_regualrExpression;
	wxCheckBox *m_searchUp;
	wxCheckBox *m_wrapsearch;
	
	// Buttons
	wxButton *m_find;
	wxButton *m_replace;
	wxButton *m_replaceAll;
	wxButton *m_markAll;
	wxButton *m_cancel;
	wxStaticText *m_replacementsMsg;

public:
	virtual ~FindReplaceDialog( );
	FindReplaceDialog();
	FindReplaceDialog(	wxWindow* parent, 
						const FindReplaceData& data, 
						wxWindowID id = wxID_ANY, 
						const wxString& caption = wxT("Find / Replace"), 
						const wxPoint& pos = wxDefaultPosition, 
						const wxSize& size = wxSize(400, 200), 
						long style = wxDEFAULT_DIALOG_STYLE);

	// Creation
	bool Create(wxWindow* parent, 
				const FindReplaceData& data, 
				wxWindowID id = wxID_ANY, 
				const wxString& caption = wxT("Find / Replace"), 
				const wxPoint& pos = wxDefaultPosition, 
				const wxSize& size = wxSize(400, 200), 
				long style = wxDEFAULT_DIALOG_STYLE
				);
 
	// Return the data 
	 FindReplaceData& GetData() { return m_data; }

	virtual bool Show();

	void SetEventOwner(wxEvtHandler *owner) { m_owner = owner; }
	wxEvtHandler *GetEventOwner() const { return m_owner; }

	// Set the replacements message
	void SetReplacementsMessage(const wxString &msg);
	void SetFindReplaceData(const FindReplaceData& data);

protected:
	void CreateGUIControls();
	void ConnectEvents();
	void OnClick(wxCommandEvent &event);
	void SendEvent(wxEventType type);
	
	void OnKeyDown(wxKeyEvent &event);
	DECLARE_EVENT_TABLE()
	void OnClose(wxCloseEvent &event);
	
};

#endif // FIND_REPLACE_DLG_H
