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
	wxKillError rc;
#ifdef __WXMSW__
	std::map<unsigned long, bool> tree;
	ProcUtils pu;
	pu.GetProcTree(tree, GetPid());

	std::map<unsigned long, bool>::iterator iter = tree.begin();
	for(; iter != tree.end(); iter++){
		wxKill(iter->first, wxSIGKILL, &rc);
	}
#else
	wxKill(GetPid(), wxSIGKILL, &rc, wxKILL_CHILDREN);
#endif 

	// Sleep for 20 ms to allow the process to be killed and 
	// the main frame to handle the event or else we can get 
	// memory leak
	wxMilliSleep( 150 );
}

long clProcess::Start(bool hide)
{
	Redirect();
	long flags = wxEXEC_ASYNC;
	if( !hide ){
		flags |= wxEXEC_NOHIDE;
	}

	m_pid = wxExecute(m_cmd, flags, this);
	return m_pid;
}

bool clProcess::HasInput(wxString &input, wxString &errors)
{
	bool hasInput = false;
	
	while ( IsInputAvailable() )
	{
		wxTextInputStream tis(*GetInputStream());

		// this assumes that the output is always line buffered
		input << tis.ReadLine();
		if(!input.EndsWith(wxT("\n"))){
			input << wxT("\n");
		}
		hasInput = true;
	}

	while ( IsErrorAvailable() )
	{
		wxTextInputStream tis(*GetErrorStream());

		errors << tis.ReadLine();
		if(!errors.EndsWith(wxT("\n"))){
			errors << wxT("\n");
		}
		hasInput = true;
	}

	return hasInput;
}
