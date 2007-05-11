#ifndef BUILDER_H
#define BUILDER_H

#include "wx/string.h"

class Builder {
protected:
	wxString m_name;

public:
	Builder(const wxString &name);
	virtual ~Builder();

	const wxString &GetName() const { return m_name; }
};

#endif // BUILDER_H
