#ifndef TARGETLEXER_H_INCLUDED
#define TARGETLEXER_H_INCLUDED

#include "LineTypes.h"
#include "Target.h"

class TargetLexer
{
	public:
		TargetLexer(TypedStrings lines);		
		
		void Lex();
		bool FindTarget(); // Is there another target in the makefile?
		bool FindDeps(); // Are there more dependencies for this target?
		bool FindActions(); // Are there more actions for this target?
			
		Targets getResult();
	
	private:
		TypedStrings m_lines;
		size_t m_current;
		
		Targets m_result;
};

#endif // TARGETLEXER_H_INCLUDED
