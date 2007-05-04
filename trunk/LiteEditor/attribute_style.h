#ifndef ATTRIBUTE_STYLE_H
#define ATTRIBUTE_STYLE_H

#include "wx/string.h"

class StyleProperty
{
	wxString m_fgColour;
	long m_fontSize;
	wxString m_name;
	wxString m_faceName;
	bool m_bold;

public:
	StyleProperty(const wxString& fgColour, const int fontSize, const wxString& name, const wxString& face, bool bold)
		: m_fgColour(fgColour)
		, m_fontSize(fontSize)
		, m_name(name)
		, m_faceName(face)
		, m_bold(bold)
	{
	};

	StyleProperty()
		: m_fgColour(_T("BLACK"))
		, m_fontSize(10)
		, m_name(wxEmptyString)
		, m_faceName(_T("Courier"))
		, m_bold(false)
	{
	};

	StyleProperty(const StyleProperty& rhs)
	{
		*this = rhs;
	};

	StyleProperty& operator=(const StyleProperty& rhs)
	{
		m_fgColour = rhs.m_fgColour;
		m_faceName = rhs.m_faceName;
		m_bold = rhs.m_bold;
		m_fontSize = rhs.m_fontSize;
		m_name = rhs.m_name;
		return *this;
	};

	virtual ~StyleProperty()
	{
	};

	const wxString& GetFgColour() const { return m_fgColour; }
	const long GetFontSize() const { return m_fontSize; }
	const wxString& GetFaceName() const { return m_faceName; }
	bool IsBold() const { return m_bold; }
	const wxString& GetName() const { return m_name; }
};

#endif 
