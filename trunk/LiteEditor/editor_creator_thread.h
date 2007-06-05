#ifndef EDITOR_CREATOR_THREAD_H
#define EDITOR_CREATOR_THREAD_H

#include "worker_thread.h"
#include "singleton.h"

class EditorCreatorThread : public WorkerThread {
	wxWindow *m_parent;

public:
	EditorCreatorThread();
	virtual ~EditorCreatorThread();

	// The thread entry point
	void ProcessRequest(ThreadRequest *request);
	void SetEditorParent(wxWindow *parent) { m_parent = parent; }
};

typedef Singleton<EditorCreatorThread> EditorCreatorThreadST;

#endif // EDITOR_CREATOR_THREAD_H

