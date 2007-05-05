#include "context_base.h"
#include <vector>
#include "editor_config.h"
#include "editor.h"

ContextBase::ContextBase(LEditor *container, const wxString &name)
: m_container(container)
, m_name(name)
{
}

ContextBase::ContextBase(const wxString &name)
: m_name(name)
{
}

ContextBase::~ContextBase()
{
}
