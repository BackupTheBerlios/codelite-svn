#ifndef __setters_getters_dlg__
#define __setters_getters_dlg__

/**
@file
Subclass of SettersGettersBaseDlg, which is generated by wxFormBuilder.
@todo Add your event handlers directly to this file.
*/

#include "setters_getters_base_dlg.h"
#include "manager.h"

/** Implementing SettersGettersBaseDlg */
class SettersGettersDlg : public SettersGettersBaseDlg
{
	std::vector<TagEntryPtr> m_members;
	
public:
	/** Constructor */
	SettersGettersDlg( wxWindow* parent, const std::vector<TagEntryPtr> &tags);
};

#endif // __setters_getters_dlg__
