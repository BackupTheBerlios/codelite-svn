#include "read_proc_input_request.h"


void ReadProcInputRequest::Process()
{
	wxString msg;
	if( m_in.CanRead() ){
		wxTextInputStream in(m_in);
		msg << in.ReadLine() << wxT("\n");
	}

	if(m_err.CanRead()){
		wxTextInputStream in(m_in);
		msg << in.ReadLine() << wxT("\n");
	}
}
