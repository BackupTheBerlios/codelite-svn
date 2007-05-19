#ifndef BUILDER_GNUMAKE_H
#define BUILDER_GNUMAKE_H

#include "builder.h"

class BuilderGnuMake : public Builder 
{
public:
	BuilderGnuMake();
	virtual ~BuilderGnuMake();

	virtual bool BuildProject(const wxString &project, const wxString &target);
	virtual bool BuildWorkspace(const wxString &target);
	virtual bool Export(const wxString &project);
};
#endif // BUILDER_GNUMAKE_H
