#include "context_manager.h"
#include "context_cpp.h"
#include "context_text.h"
#include "context_base.h"

ContextManager::ContextManager()
{
	// register available context
	m_contextPool[wxT("Cpp")] = ContextBasePtr( new ContextCpp() );
	m_contextPool[wxT("Text")] = ContextBasePtr( new ContextText() );
	m_contextPool[wxT("Default")] = ContextBasePtr( new ContextText() );
}

ContextManager::~ContextManager()
{
}

ContextBasePtr ContextManager::NewContext(wxWindow *parent, const wxString &lexerName)
{
	// this function is actually a big switch ....
	std::map<wxString, ContextBasePtr>::iterator iter = m_contextPool.find(lexerName);
	if( iter == m_contextPool.end()){
		return m_contextPool[wxT("Text")]->NewInstance((LEditor*)parent);
	}

	return iter->second->NewInstance((LEditor*)parent);
}
