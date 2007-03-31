#ifndef MANAGER_H
#define MANAGER_H

#include "singleton.h"
#include "wx/string.h"
#include "readtags.h"
#include "entry.h"

class Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;
public:
	bool IsWorkspaceOpen() const;
	const wxString &GetInstallPath() const;
	void OpenFile(const TagEntry &tag);
	void SetActivePageTitle(const wxString &name);
	const wxString GetActivePageTitle();

protected:
	Manager(void);
	virtual ~Manager(void);
};

#endif // MANAGER_H