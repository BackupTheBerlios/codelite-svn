#ifndef MACROS_H
#define MACROS_H

//-------------------------------------------------------
// wxWidgets Connect macros
//-------------------------------------------------------

#define ConnectChoice(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(fn), NULL, this);

#define ConnectButton(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListBoxDClick(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectCheckBox(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(fn), NULL, this);

#define ConnectListCtrlItemSelected(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(fn), NULL, this);

#define ConnectListCtrlItemActivated(ctrl, fn)\
	ctrl->Connect(ctrl->GetId(), wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler(fn), NULL, this);

#define TrimString(str){\
		str = str.Trim();\
		str = str.Trim(false);\
	}

#define BoolToString(b) b ? wxT("yes") : wxT("no")

//-----------------------------------------------------
// Constants
//-----------------------------------------------------

const wxString clCMD_NEW = wxT("<New...>");
const wxString clCMD_EDIT = wxT("<Edit...>");

#endif // MACROS_H
