#pragma once
#include <wx/string.h>

class Pluggable
{
private:
	wxString m_className;

public:
	// Ctor-Dtor
	Pluggable(void);

	virtual ~Pluggable(void){};

	// Abstract methods 
	virtual wxString GetClassName() const = 0;
	virtual Pluggable* Clone() = 0;
};
