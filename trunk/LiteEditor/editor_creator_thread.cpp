#include "editor_creator_thread.h"
#include "editor_creator.h"
#include "editor.h"

EditorCreatorThread::EditorCreatorThread()
{
}

EditorCreatorThread::~EditorCreatorThread()
{
}

void EditorCreatorThread::ProcessRequest(ThreadRequest *request)
{
	wxUnusedVar(request);
	//check the current number of objects on the queue
	size_t count = EditorCreatorST::Get()->GetQueueCount();
	if(count  < 5){
		//create enough objects to fill the queue with 5 editors
		size_t amountToCreate = 5 - count;
		for(size_t i=0; i<amountToCreate; i++){
			EditorCreatorST::Get()->Add(new LEditor(m_parent, wxID_ANY, wxSize(1, 1), wxEmptyString, wxEmptyString, true));
		}
	}

	wxThread::Sleep(1000);

	//put another request on the thread queue to keep him awake
	Add(new ThreadRequest());
}
