#include "output_pane.h"

const wxString OutputPane::FIND_IN_FILES_WIN = wxT("Find Results");
const wxString OutputPane::BUILD_WIN = wxT("Build");

OutputPane::OutputPane(wxWindow *parent, const wxString &caption)
: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 300))
, m_caption(caption)
{
	CreateGUIControls();	
}


OutputPane::~OutputPane()
{

}

void OutputPane::CreateGUIControls()
{
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(mainSizer);

	long style = wxFNB_NO_X_BUTTON | wxFNB_NO_NAV_BUTTONS | wxFNB_DROPDOWN_TABS_LIST | wxFNB_BOTTOM; 
	m_book = new wxFlatNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
	mainSizer->Add(m_book, 1, wxEXPAND | wxALL, 1);

	// Create the 'Find In Files Window'
	wxTextCtrl *findInFilesWin = new wxTextCtrl(m_book, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(600, 200), wxTE_MULTILINE);
	m_book->AddPage(findInFilesWin, FIND_IN_FILES_WIN, true);

	wxFont font(8, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	findInFilesWin->SetFont(font);
}

void OutputPane::AppendText(const wxString &winName, const wxString &text)
{
	int index = CaptionToIndex(winName);
	if( index == wxNOT_FOUND )
		return;
	
	m_book->SetSelection((size_t)index);
	switch( index )
	{
	case 0:	// Find In Files
		{
			wxTextCtrl *win = static_cast<wxTextCtrl*>(m_book->GetPage((size_t)index));
			win->AppendText( text );
			break;
		}
	default:
		break;
	}
}

int OutputPane::CaptionToIndex(const wxString &caption)
{
	int i = 0;
	for(; i<m_book->GetPageCount(); i++)
	{
		if(m_book->GetPageText((size_t)i) == caption)
			return i;
	}
	return wxNOT_FOUND;
}