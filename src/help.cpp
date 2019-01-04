//---------------------------------------------------
/*
 	\file		help.cpp

 	\brief		Common functions

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------



#include "help.h"


#include <iostream>



//---------------------------------------------------
/*
	\brief	Function prints message into the standard
			output	(stdout)

	\param	_str	Message (may included string
					arguments e.g. %s, %d, ..)
			...		String argument's values
*/
//---------------------------------------------------
void DebugMessage(const char* _str, ...)
{
#ifdef _DEBUG
	va_list args;
	char text[512];

	va_start(args, _str);
	vsnprintf(text, 512, _str, args);
	va_end(args);
	std::cout << text << std::endl;
#endif //_DEBUG
}
//---------------------------------------------------
/*
	\brief	Function prints message into the error
			output (stderr)

	\param	_str	Message (may included string
					arguments e.g. %s, %d, ..)
			...		String argument's values
*/
//---------------------------------------------------
void ErrorMessage(const char* _str, ...)
{
	va_list args;
	char text[512];

	va_start(args, _str);
	vsnprintf(text, 512, _str, args);
	va_end(args);
	std::cerr << text << std::endl;
}


