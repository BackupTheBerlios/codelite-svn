#include "manager.h"
#include "ctags_manager.h"
#include "frame.h"

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

void Manager::SetActivePageTitle(const wxString &name)
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	Frame::Get()->GetNotebook()->SetPageText(nb->GetSelection(), name);
}

const wxString Manager::GetActivePageTitle()
{
	wxFlatNotebook *nb = Frame::Get()->GetNotebook();
	return Frame::Get()->GetNotebook()->GetPageText(nb->GetSelection());
}
