#include "VariableLexer.h"
#include <vector>
#include <map>
#include <wx/msgdlg.h> // wxMessageBox
#include <wx/log.h>

void VariableLexer::DebugMessage(const wxString& msg)
{
	wxLogMessage( msg ); 
}

VariableLexer::VariableLexer(wxArrayString lines) :
m_lines(lines),
m_current(0)
{
	Control();
}

void VariableLexer::Control()
{
	bool done = false;
	bool good = true;
	while(m_current < m_lines.size() && good)
	{
		done = ReadVariables();
		if(done) break;
		good = ExpandVariables();
	}
}	

/**
 * Get result, constructor ensures that m_result is available when we get to this point.
 */ 
wxArrayString VariableLexer::getResult()
{
	return m_result;
}
		

/**
 * Read in only the LINE_DEFINITIONs and store them in a map.
 * Not sure what this could be replaced by.
 */ 
bool VariableLexer::ReadVariables()
{
	for(; m_current < m_lines.size(); m_current++)
	{
		wxString current = m_lines[m_current];
		//printf("Current line: '%s'\n", current.mb_str());
			
		size_t from = -1;
		size_t to = -1;
		size_t equals = current.Find('=');
		size_t colonequals = current.Find(wxT(":="));
		size_t space = current.Find(' ');
		size_t tab = current.Find('\t');
		size_t pos = current.Find(wxT("$("));
		
		if(current.size() == 0)
		{
			continue;
		}
		
		if(pos != (size_t)-1)
		{
			return false;
		}
		
		if(!isalpha(current.at(0))
		|| (colonequals == (size_t)-1 && equals == (size_t)-1))
		{
			m_result.push_back(current);
			continue;
		}
		
		if(space < to)
		{
			to = space;
		}
		
		if(tab < to)
		{
			to = tab;
		}
			
		if(colonequals < to)
		{
			to = colonequals;
		}
		
		if(equals < to)
		{
			to = equals;
		}
		
		if(colonequals < from)
		{
			from = colonequals + 2;
		}
		
		if(equals < from)
		{
			from = equals + 1;
		}
		
		wxString name = current.substr(0, to);
		wxString value = current.substr(from);
		m_tokens[name] = value;
	}
	
	return true;
}

/**
 * Read in only the LINE_EXPANSION and expand them.
 * Not sure what this could be replaced by either. 
 */ 
bool VariableLexer::ExpandVariables()
{
	wxString current = m_lines[m_current];
	/*if(current.find("$(") == -1) // FISME
	{
		printf("Invalid Line Expansion, no $( found!!\n");
		exit(-1);
	}*/
	
	for(size_t pos = m_lines[m_current].find(wxT("$(")); pos != (size_t)-1; pos = m_lines[m_current].find(wxT("$(")))
	{
		current = m_lines[m_current];
		size_t end = current.find(')');
		
		if(end == (size_t)-1 || end < pos)
		{
			wxString error;
			error << wxT("Pos is ") << pos << wxT(", End at ") << end << wxT(", line '") << current << wxT("'.\n");
			error << wxT("Invalid Line Expansion!\n");
			wxMessageBox(error, wxT("Lite Editor"), wxICON_HAND | wxOK);
			return false;
		}
		
		int skip = end - pos - 2;
		
		wxString name = current.substr(pos+2, skip);
		wxString value = m_tokens[name];
			
		if(value.size() == 0)
		{
			wxString error;
			error << wxT("We don't have token a token yet :(.\n");
			error << wxT("Line: ") << current << wxT("\n");
			error << wxT("Token: ") << name << wxT("\n");
			wxMessageBox(error, wxT("Lite Editor"), wxICON_HAND | wxOK);
			return false;
		}
		
		wxString before;
		wxString after;
		
		if(pos > 0)
		{
			before = current.substr(0, pos);
		}
		
		if(end+1 < current.size())
		{
			after = current.substr(end+1);
		}
		
		wxString result = before + value + after;
			
		m_lines[m_current].swap(result);
	}
	
	return true;
}
