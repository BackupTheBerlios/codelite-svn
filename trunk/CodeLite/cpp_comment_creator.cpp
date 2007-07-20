#include "cpp_comment_creator.h"
#include <wx/datetime.h>
#include <vector>
#include "language.h"

CppCommentCreator::CppCommentCreator(TagEntryPtr tag)
: m_tag(tag)
{
}

CppCommentCreator::~CppCommentCreator()
{
}

wxString CppCommentCreator::CreateComment()
{
	if(m_tag->GetKind() == wxT("class"))
		return ClassComment();
	else if(m_tag->GetKind() == wxT("function"))
		return FunctionComment();
	else if(m_tag->GetKind() == wxT("prototype"))
		return FunctionComment();
	return wxEmptyString;
}

wxString CppCommentCreator::ClassComment()
{
	wxDateTime now = wxDateTime::Now();
	wxString comment;
	comment << wxT("/**\n");
	comment << wxT(" * \\class ") << m_tag->GetName() << wxT("\n");
	comment << wxT(" * \\brief \n");
	comment << wxT(" * \\author ") << wxGetUserName() << wxT("\n");
	comment << wxT(" * \\date ") << now.FormatDate() << wxT("\n");
	comment << wxT(" */\n");
	return comment;
}

wxString CppCommentCreator::FunctionComment()
{
	wxDateTime now = wxDateTime::Now();
	wxString comment;

	//parse the function signature
	std::vector<TagEntryPtr> tags;
	LanguageST::Get()->GetLocalVariables(m_tag->GetSignature(), tags);
	comment << wxT("/**\n");
	comment << wxT(" * \\brief \n");
	for(size_t i=0; i<tags.size(); i++)
		comment << wxT(" * \\param ") << tags.at(i)->GetName() << wxT("\n");
	comment << wxT(" */\n");
	return comment;
}
