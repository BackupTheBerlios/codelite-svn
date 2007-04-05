#ifndef SEARCH_MANAGER_H
#define SEARCH_MANAGER_H

#include "singleton.h"

class SearchManager;

// The search manager class:
// The interface to the search thread.
class SearchManager : public Singleton<SearchManager>
{
	friend class Singleton<SearchManager>;
private:

};
#endif // SEARCH_MANAGER_H
