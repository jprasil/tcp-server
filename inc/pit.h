//---------------------------------------------------
/*
 	\file		pit.h

 	\brief		Periodic Interval Timer module
 				This module ensures periodic interrupts
 				of process by SIGALRM signal.

	\date		16.11.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#ifndef INC_PIT_H_
#define INC_PIT_H_


#include <sys/time.h>

//---------------------------------------------------
//!	New name space - pit
namespace pit
{
	void SetPIT(__time_t _sec, __suseconds_t _usec);	//!< Function activates Periodic Interval Timer
}


#endif /* INC_PIT_H_ */
