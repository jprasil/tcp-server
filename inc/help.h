//---------------------------------------------------
/*
 	\file		help.h

 	\brief		Common functions

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------
#ifndef INC_HELP_H_
#define INC_HELP_H_

#include <cstdio>
#include <cstdarg>



//---------------------------------------------------
/*
	\brief	Error handlers, that print error message
			and terminate a process with code
			EXIT_FAILURE
*/
//--------------------------------------------------
#define HandleError(_msg) \
	do{ perror(_msg); exit(EXIT_FAILURE); }while(0)

#define HandleError_ErrNum(_errNum, _msg) \
	do{ errno=_errNum; perror(_msg); exit(EXIT_FAILURE); }while(0)

//---------------------------------------------------
/*
	\brief	Function prints message into the standard
			output	(stdout)

	\param	_str	Message (may included string
					arguments e.g. %s, %d, ..)
			...		String argument's values
*/
//---------------------------------------------------
void DebugMessage(const char* _str, ...);
//---------------------------------------------------
/*
	\brief	Function prints message into the error
			output (stderr)

	\param	_str	Message (may included string
					arguments e.g. %s, %d, ..)
			...		String argument's values
*/
//---------------------------------------------------
void ErrorMessage(const char* _str, ...);



#endif /* INC_HELP_H_ */
