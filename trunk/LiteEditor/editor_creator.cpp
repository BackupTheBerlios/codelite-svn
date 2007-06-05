#include "editor_creator.h"
#include "editor.h"
#include "wx/window.h"

EditorCreator::EditorCreator()
: m_parent(NULL)
{
}


EditorCreator::~EditorCreator()
{
}

void EditorCreator::SetParent(wxWindow *parent)
{
	m_parent = parent;
}

LEditor *EditorCreator::NewInstance()
{
	// On GTK try to get instance from the queue first
	LEditor *editor = m_queue.PopFront();
	if( !editor ){
		//no elements on the queue, create new instance
		editor = new LEditor(m_parent, wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, true);
	}
	editor->Show();
	return editor;
}

void EditorCreator::Add(LEditor *editor)
{
	m_queue.PushBack(editor);
}

size_t EditorCreator::GetQueueCount()
{
	return m_queue.GetCount();
}
