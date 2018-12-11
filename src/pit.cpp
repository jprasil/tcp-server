/*
 * pit.cpp
 *
 *  Created on: 16. 11. 2018
 *      Author: root
 */

#include "pit.h"
#include "help.h"
#include <cstring>
#include <cerrno>
#include <pthread.h>

static itimerval PIT = {{0,0},{0,0}};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;



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

