#ifndef BUILD_SYSTEM_H
#define BUILD_SYSTEM_H

#include "wx/xml/xml.h"
#include "wx/string.h"
#include "smart_ptr.h"
#include "list"

class BuildSystem {
	wxString m_name;
	wxString m_toolPath;
	wxString m_toolOptions;

public:
	BuildSystem(wxXmlNode *node);
	virtual ~BuildSystem();
	wxXmlNode *ToXml() const;

	const wxString &GetName() const { return m_name; }
	const wxString &GetToolPath() const { return m_toolPath; }
	const wxString &GetToolOptions() const { return m_toolOptions; }
	void SetName(const wxString &name) { m_name = name; }
	void SetToolPath(const wxString &path) { m_toolPath = path; }
	void SetToolOptions(const wxString &options) { m_toolOptions = options; }
};

typedef SmartPtr<BuildSystem> BuildSystemPtr;
typedef std::list<BuildSystemPtr> BuildSystemList;

#endif //BUILD_SYSTEM_H
