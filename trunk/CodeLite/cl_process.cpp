#include "cl_process.h"
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include "procutils.h"

clProcess::clProcess(int id, const wxString &cmdLine)
: wxProcess(NULL, id)
, m_pid(-1)
, m_uid(id)
, m_cmd(cmdLine)
{
}

clProcess::~clProcess()
{
}

long clProcess::GetPid()
{
	return m_pid;
}

void clProcess::SetPid(long pid)
{
	m_pid = pid;
}

void clProcess::Terminate()
{
	std::map<unsigned long, bool> tree;
	ProcUtils pu;
	pu.GetProcTree(tree, GetPid());

	wxKillError rc;
	std::map<unsigned long, bool>::iterator iter = tree.begin();
	for(; iter != tree.end(); iter++){
		wxKill(iter->first, wxSIGKILL, &rc);
	}

	// Sleep for 20 ms to allow the process to be killed and 
	// the main frame to handle the event or else we can get 
	// memory leak
	wxMilliSleep( 20 );
}

long clProcess::Start()
{
	Redirect();
	m_pid = wxExecute(m_cmd, wxEXEC_ASYNC, this);
	return m_pid;
}

bool clProcess::HasInput(wxString &input)
{
	bool hasInput = false;
	
	if ( IsInputAvailable() )
	{
		wxTextInputStream tis(*GetInputStream());

		// this assumes that the output is always line buffered
		input << tis.ReadLine();
		hasInput = true;
	}

	if ( IsErrorAvailable() )
	{
		wxTextInputStream tis(*GetErrorStream());

		input << tis.ReadLine();
		hasInput = true;
	}

	return hasInput;
}
