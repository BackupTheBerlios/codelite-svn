#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <map>
#include <list>
#include "wx/string.h"
#include "singleton.h"
#include "builder.h"

class BuildManager;

/**
 * \ingroup SDK
 * The build manager class which contains all the builders currently
 * available to the editor
 *
 * Each and every call to this class is protected by a critical section to make this
 * class safe when used inside the compiler thread
 *
 * \version 1.0
 * first version
 *
 * \date 05-19-2007
 *
 * \author Eran
 *
 */
class BuildManager {

	friend class Singleton<BuildManager>;
	std::map<wxString, BuilderPtr> m_builders;
	wxCriticalSection m_cs;

public:
	typedef std::map<wxString, BuilderPtr>::const_iterator ConstIterator;

private:
	BuildManager();
	virtual ~BuildManager();

public:
	void AddBuilder(BuilderPtr builder);
	void RemoveBuilder(const wxString &name);
	void GetBuilders(std::list<wxString> &list);
};

typedef Singleton<BuildManager> BuildManagerST;

#endif // BUILDMANAGER_H

