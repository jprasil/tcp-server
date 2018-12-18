//---------------------------------------------------
/*
 	\file		pit.cpp

 	\brief		Periodic Interval Timer module
 				This module ensures periodic interrupts
 				of process by SIGALRM signal.

	\date		16.11.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#include "pit.h"
#include "help.h"
#include <cstring>
#include <cerrno>
#include <pthread.h>

static itimerval PIT = {{0,0},{0,0}};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

//---------------------------------------------------
/*
 	\brief	Function activates PIT on given period

 	\param	_sec	Period [s]
 			_usec	Period [us]
*/
//---------------------------------------------------
void pit::SetPIT(__time_t _sec, __suseconds_t _usec)
{
	itimerval IntTimer;
	pthread_t tid = pthread_self();

	pthread_mutex_lock(&mutex);

	if(getitimer(ITIMER_REAL, &IntTimer) == 0)
	{
		if(IntTimer.it_interval.tv_sec == 0 && IntTimer.it_interval.tv_usec == 0)
		{
			DebugMessage("Timer is disarmed, thread set the timer tid: %d", tid);
			PIT.it_interval.tv_sec = _sec;
			PIT.it_interval.tv_usec = _usec;
			PIT.it_value.tv_sec = PIT.it_interval.tv_sec;
			PIT.it_value.tv_usec = PIT.it_interval.tv_usec;
			setitimer(ITIMER_REAL, &PIT, nullptr);
		}
		else
		{
			DebugMessage("Timer is armed, time until next expiration sec:%d usec:%d tid:%d", IntTimer.it_value.tv_sec, IntTimer.it_value.tv_usec, tid);
		}
	}
	else
	{
		ErrorMessage("getitimer() error: %s", strerror(errno));
	}

	pthread_mutex_unlock(&mutex);
}

