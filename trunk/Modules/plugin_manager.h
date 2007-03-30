#include "../CodeLite/singleton.h"
#include <map>
#include <wx/string.h>

class Pluggable;

/**
 * The plugin manager class
 */
class PluginManager 
{
	friend class Singleton<PluginManager>;
	std::map<wxString, Pluggable*> m_classes;
public:
	/**
	 * Add class to the pluing manager
	 */
	void AddClass(Pluggable* pcls);

	/**
	 * Find class by name
	 */
	Pluggable *FindClass(const wxString &clasName);

private:
	PluginManager();
	virtual ~PluginManager();
};

typedef Singleton<PluginManager> PluginManagerST;

