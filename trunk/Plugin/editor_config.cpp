#include "editor_config.h"
#include <wx/xml/xml.h>
#include "xmlutils.h"
#include "dirtraverser.h"

EditorConfig::EditorConfig()
{
	m_doc = new wxXmlDocument();
}

EditorConfig::~EditorConfig()
{
	delete m_doc;
}

bool EditorConfig::Load()
{
	m_fileName = wxFileName(wxT("config/liteeditor.xml"));
	m_fileName.MakeAbsolute();

	//load the main configuration file
	if(!m_doc->Load(m_fileName.GetFullPath())){
		return false;
	}
	//load all lexer configuration files
	DirTraverser traverser(wxT("*.xml"));
	wxDir dir(wxT("lexers/"));
	dir.Traverse(traverser);

	wxArrayString files = traverser.GetFiles();
	m_lexers.clear();
	for(size_t i=0; i<files.GetCount(); i++){
		LexerConfPtr lexer(new LexerConf(files.Item(i)));
		m_lexers[lexer->GetName()] = lexer;
	}
	return true;
}

void EditorConfig::SaveLexers()
{
	std::map<wxString, LexerConfPtr>::iterator iter = m_lexers.begin();
	for(; iter != m_lexers.end(); iter++)
	{
		iter->second->Save();
	}
}

wxXmlNode* EditorConfig::GetLexerNode(const wxString& lexerName)
{
	wxXmlNode *lexersNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Lexers"));
	if( lexersNode ){
		return XmlUtils::FindNodeByName(lexersNode, wxT("Lexer"), lexerName);
	}
	return NULL;
}

LexerConfPtr EditorConfig::GetLexer(const wxString &lexerName) {
	if(m_lexers.find(lexerName) == m_lexers.end()){
		return NULL;
	}

	return m_lexers.find(lexerName)->second;
}

wxString EditorConfig::LoadPerspective(const wxString &Name) const
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return wxEmptyString;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Perspective") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == Name){
				return child->GetPropVal(wxT("Value"), wxEmptyString);
			}
		}
		child = child->GetNext();
	}

	return wxEmptyString;
}

void EditorConfig::SavePerspective(const wxString &name, const wxString &pers)
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Perspective") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == name){
				XmlUtils::UpdateProperty(child, wxT("Value"), pers);
				m_doc->Save(m_fileName.GetFullPath());
				return;
			}
		}
		child = child->GetNext();
	}

	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Perspective"));
	newChild->AddProperty(wxT("Name"), name);
	newChild->AddProperty(wxT("Value"), pers);
	layoutNode->AddChild(newChild);
	m_doc->Save(m_fileName.GetFullPath());
}

long EditorConfig::LoadNotebookStyle(const wxString &nbName)
{
	long style = wxNOT_FOUND;
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if(!layoutNode){
		return style;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Notebook") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == nbName){
				wxString strStyle = child->GetPropVal(wxT("Style"), wxEmptyString);
				strStyle.ToLong(&style);
				break;
			}
		}
		child = child->GetNext();
	}
	return style;
}

void EditorConfig::SaveNotebookStyle(const wxString &nbName, long style)
{
	wxXmlNode *layoutNode = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Layout"));
	if( !layoutNode ){
		return;
	}

	wxXmlNode *child = layoutNode->GetChildren();
	while( child ){
		if( child->GetName() == wxT("Notebook") ){
			if(child->GetPropVal(wxT("Name"), wxEmptyString) == nbName){
				wxString strStyle;
				strStyle << style;
				XmlUtils::UpdateProperty(child, wxT("Style"), strStyle);
				m_doc->Save(m_fileName.GetFullPath());
				return;
			}
		}
		child = child->GetNext();
	}

	wxXmlNode *newChild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Notebook"));
	newChild->AddProperty(wxT("Name"), nbName);

	wxString strStyle;
	strStyle << style;
	newChild->AddProperty(wxT("Style"), strStyle);
	layoutNode->AddChild(newChild);
	m_doc->Save(m_fileName.GetFullPath());
}

EditorConfig::ConstIterator EditorConfig::LexerEnd()
{
	return m_lexers.end();
}

EditorConfig::ConstIterator EditorConfig::LexerBegin()
{
	return m_lexers.begin();
}

void EditorConfig::SetLexer(LexerConfPtr lexer)
{
	m_lexers[lexer->GetName()] = lexer;
	lexer->Save();
}

OptionsConfigPtr EditorConfig::GetOptions() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	// node can be null ...
	return new OptionsConfig(node);
}

void EditorConfig::SetOptions(OptionsConfigPtr opts)
{
	// locate the current node
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("Options"));
	if( node ){
		m_doc->GetRoot()->RemoveChild(node);
		delete node;
	}

	m_doc->GetRoot()->AddChild(opts->ToXml());
	m_doc->Save(m_fileName.GetFullPath());
}

void EditorConfig::SetTagsDatabase(const wxString &dbName)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
	if( node ){
		XmlUtils::UpdateProperty(node, wxT("Path"), dbName);
	} else {
		//create new node
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("TagsDatabase"));
		node->AddProperty(wxT("Path"), dbName);
		m_doc->GetRoot()->AddChild(node);
	}
	m_doc->Save(m_fileName.GetFullPath());
}

wxString EditorConfig::GetTagsDatabase() const
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("TagsDatabase"));
	if(node){
		return XmlUtils::ReadString(node, wxT("Path"));
	}else{
		return wxEmptyString;
	}
}

void EditorConfig::GetRecentlyOpenedFies(wxArrayString &files)
{
	//find the root node of the recent files
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentFiles"));
	if(node)
	{
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("File")){
				wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
				files.Add(fileName);
			}
			child = child->GetNext();
		}
	}
}

void EditorConfig::SetRecentlyOpenedFies(const wxArrayString &files)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentFiles"));
	if(node == NULL){
		//create new entry in the configuration file
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("RecentFiles"));
		m_doc->GetRoot()->AddChild(node);
	}else{
		//remove old children
		XmlUtils::RemoveChildren(node);
	}
	
	for(size_t i=0; i<files.GetCount(); i++)
	{
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		child->AddProperty(wxT("Name"), files.Item(i));
		node->AddChild(child);
	}
	
	//save the data to disk
	m_doc->Save(m_fileName.GetFullPath());
}


void EditorConfig::GetRecentlyOpenedWorkspaces(wxArrayString &files)
{
	//find the root node of the recent files
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentWorkspaces"));
	if(node)
	{
		wxXmlNode *child = node->GetChildren();
		while(child){
			if(child->GetName() == wxT("File")){
				wxString fileName = XmlUtils::ReadString(child, wxT("Name"));
				files.Add(fileName);
			}
			child = child->GetNext();
		}
	}
}

void EditorConfig::SetRecentlyOpenedWorkspaces(const wxArrayString &files)
{
	wxXmlNode *node = XmlUtils::FindFirstByTagName(m_doc->GetRoot(), wxT("RecentWorkspaces"));
	if(node == NULL){
		//create new entry in the configuration file
		node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("RecentWorkspaces"));
		m_doc->GetRoot()->AddChild(node);
	}else{
		//remove old children
		XmlUtils::RemoveChildren(node);
	}
	
	for(size_t i=0; i<files.GetCount(); i++)
	{
		wxXmlNode *child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("File"));
		child->AddProperty(wxT("Name"), files.Item(i));
		node->AddChild(child);
	}
	
	//save the data to disk
	m_doc->Save(m_fileName.GetFullPath());
}