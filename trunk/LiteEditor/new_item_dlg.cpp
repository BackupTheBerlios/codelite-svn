///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "new_item_dlg.h"
#include "wx/xrc/xmlres.h"
#include "wx/textctrl.h"

static const wxString FileTypeCpp = wxT("C++ Source File (.cpp)");
static const wxString FileTypeC = wxT("C Source File (.c)");
static const wxString FileTypeHeader = wxT("Header File (.h)");
static const wxString FileTypeAny = wxT("Any File");

///////////////////////////////////////////////////////////////////////////

NewItemDlg::NewItemDlg( wxWindow* parent, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* m_mainSizer;
	m_mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_fileType = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL  );
	m_mainSizer->Add( m_fileType, 1, wxALL|wxEXPAND, 5 );
	
	// Initialise images map
	wxImageList *images = new wxImageList(16, 16, true);
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_c")));			//0
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_cplusplus")));	//1
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_h")));			//2
	images->Add(wxXmlResource::Get()->LoadBitmap(_T("page_white_text")));		//3

	m_fileType->AssignImageList( images,  wxIMAGE_LIST_SMALL );
	m_fileType->InsertColumn(0, wxT("File Type"));

	//-----------------------------------
	// Populate the list:
	wxListItem item;

	item.SetText(FileTypeCpp);
	item.SetImage(1);
	m_fileType->InsertItem(item);
	m_fileTypeValue = FileTypeCpp;

	item.SetText(FileTypeC);
	item.SetImage(0);
	m_fileType->InsertItem(item);

	item.SetText(FileTypeHeader);
	item.SetImage(2);
	m_fileType->InsertItem(item);

	item.SetText(FileTypeAny);
	item.SetImage(3);
	item.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	m_fileType->InsertItem(item);
	m_fileType->SetColumnWidth(0, wxLIST_AUTOSIZE);

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 3, 2, 5 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_location = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_location, 1, wxALL|wxEXPAND, 5 );
	m_location->SetValue(wxGetCwd());
	
	m_browseBtn = new wxButton( this, wxID_ANY, wxT("&Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_browseBtn, 0, wxALL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_fileName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_fileName, 1, wxALL|wxEXPAND, 5 );
	
	m_mainSizer->Add( fgSizer2, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_mainSizer->Add( m_staticline1, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_okButton = new wxButton( this, wxID_ANY, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_okButton, 0, wxALL, 5 );
	
	m_cancel = new wxButton( this, wxID_ANY, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_cancel, 0, wxALL, 5 );
	
	m_mainSizer->Add( bSizer6, 0, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( m_mainSizer );
	this->Layout();
	
	m_fileName->SetFocus();
	// Attach events 
	ConnectEvents();
}


void NewItemDlg::ConnectEvents()
{
	m_cancel->Connect(m_cancel->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
	m_okButton->Connect(m_okButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
	m_browseBtn->Connect(m_browseBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(NewItemDlg::OnClick), NULL, this);
	m_fileType->Connect(m_fileType->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(NewItemDlg::OnListItemSelected), NULL, this);
}


void NewItemDlg::OnClick(wxCommandEvent &event)
{
	int id = event.GetId();
	if( id == m_okButton->GetId() ){
		DoCreateFile();
	} else if( id == m_cancel->GetId() ){
		EndModal(wxID_CANCEL);
	} else if( id == m_browseBtn->GetId() ){
		wxDirDialog *dlg = new wxDirDialog(this, wxT("Location:"), wxGetCwd());
		if(dlg->ShowModal() == wxID_OK)
		{
			m_location->SetValue(dlg->GetPath());
		}
		dlg->Destroy();
	}
}

void NewItemDlg::DoCreateFile()
{
	wxString errMsg;
	if( !Validate(errMsg) ){
		wxMessageBox(errMsg, wxT("Lite Editor"), wxICON_INFORMATION | wxOK);
		return;
	}

	// Construct the file name
	if(m_fileTypeValue == FileTypeAny){
		m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue());
	} else if( m_fileTypeValue == FileTypeC ){
		// If user already provided suffix, dont add another one on top of it
		if( m_fileName->GetValue().Find(wxT(".")) == wxNOT_FOUND ){
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue(), wxT("c"));
		} else {
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue());
		}
	} else if( m_fileTypeValue == FileTypeCpp ){
		// If user already provided suffix, dont add another one on top of it
		if( m_fileName->GetValue().Find(wxT(".")) == wxNOT_FOUND ){
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue(), wxT("cpp"));
		} else {
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue());
		}
	} else if( m_fileTypeValue == FileTypeHeader ){
		// If user already provided suffix, dont add another one on top of it
		if( m_fileName->GetValue().Find(wxT(".")) == wxNOT_FOUND ){
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue(), wxT("h"));
		} else {
			m_newFileName = wxFileName(m_location->GetValue(), m_fileName->GetValue());
		}
	}
	EndModal(wxID_OK);
}

bool NewItemDlg::Validate(wxString &errMsg)
{
	// make sure we have file name & path set up correctly
	wxFileName fn(m_location->GetValue() + wxT("/"));

	if( m_location->GetValue().Trim().IsEmpty() ){
		errMsg = wxT("Missing location");
		return false;
	}

	if( !fn.DirExists() ){
		errMsg = wxT("Directory: ");
		errMsg << fn.GetPath() << wxT(" does not exist");
		return false;
	}

	fn = wxFileName(m_location->GetValue(), m_fileName->GetValue());
	if( fn.FileExists() ){
		errMsg = wxT("A file with that name already exist, please choose different name");
		return false;
	}

	if( m_fileName->GetValue().Trim().IsEmpty() ){
		errMsg = wxT("Missing file name");
		return false;
	}

	return true;
}

void NewItemDlg::OnListItemSelected(wxListEvent &event)
{
	m_fileTypeValue = event.GetText();
}


