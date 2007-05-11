#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <map>
#include "wx/string.h"

class Builder;

class BuildManager {
	
	std::map<wxString, Builder*> m_builders;

private:
	BuildManager();
	virtual ~BuildManager();

public:
	static BuildManager *Get();
	void AddBuilder(Builder *builder);
	void RemoveBuilder(const wxString &name);
};

#endif // BUILDMANAGER_H