///////////////////////////////////////////////////////////////////////////////
// Name:        wxAutoComboBox.cpp
// Purpose:     wxAutoComboBox class implementation.
// Author:      Guru Kathiresan
// Modified by:
// Created:     03/24/2006
// Copyright:   (c) Guru Kathiresan
// Licence:     BSD licence
///////////////////////////////////////////////////////////////////////////////

#include "autocombobox.h"

#if wxUSE_COMBOBOX

IMPLEMENT_DYNAMIC_CLASS(wxAutoComboBox, wxComboBox)

BEGIN_EVENT_TABLE(wxAutoComboBox, wxComboBox)
EVT_KEY_UP(wxAutoComboBox::OnKeyUp)
END_EVENT_TABLE()

wxAutoComboBox::wxAutoComboBox()
{

}

wxAutoComboBox::wxAutoComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name)
: wxComboBox(parent, id, value, pos, size, choices, style, validator, name)
{
}


wxAutoComboBox::~wxAutoComboBox(void)
{
}

void wxAutoComboBox::OnKeyUp(wxKeyEvent &event)
{
	int nKeyCode = event.GetKeyCode();
	if ( ((nKeyCode >= 32) && (nKeyCode <= 126)) || ((nKeyCode >= 128) && (nKeyCode <= 254)))
	{
		wxString txtValue = GetValue();

		long start=-1, end=-1;
		GetSelection(&start, &end);

		wxString EnteredValue = txtValue.Left(start);
		int Idx = -1;
		for( size_t i=0; i<this->GetCount(); i++ )
		{
			if (this->GetString((unsigned int)i).Left(EnteredValue.Length()).CmpNoCase(EnteredValue) == 0)
			{
				Idx = (int)i;
				break;
			}                     
		}
		if (Idx != -1)
		{
			SetSelection(Idx);
			SetSelection(start, (long)GetValue().Length());  
			event.Skip(true);
			return;
		}              

	}
	event.Skip(true);
}
#endif // wxUSE_COMBOBOX 
