#include "manager.h"
#include "ctags_manager.h"
#include "frame.h"
#include "editor.h"

Manager::Manager(void)
{
}

Manager::~Manager(void)
{
}

bool Manager::IsWorkspaceOpen() const 
{
	return TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().IsOk();
}

void Manager::OpenFile(const TagEntry &tag)
{
	Frame::Get()->OpenFile(tag);
}

const wxString &Manager::GetInstallPath() const
{
	return Frame::Get()->GetInstallPath();
}

void Manager::SetPageTitle(wxWindow *page, const wxString &name)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if( selection != -1 ){
		Frame::Get()->GetNotebook()->SetPageText(selection, name);
	}
}

const wxString Manager::GetPageTitle(wxWindow *page)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	int selection = nb->GetPageIndex(page);
	if( selection != -1 ) {
		return Frame::Get()->GetNotebook()->GetPageText(selection);
	} else {
		return wxEmptyString;
	}
}

void Manager::SaveAll()
{
	wxFlatNotebook *book = Frame::Get()->GetNotebook();
	size_t count = book->GetPageCount();

	for(size_t i=0; i<count; i++)
	{
		LEditor* editor = static_cast<LEditor*>(book->GetPage(i));
		if( !editor )
			continue;
		
		if( editor->GetModify() ) 
		{
			editor->SaveFile();
		}
	}
}
