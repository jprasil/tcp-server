//---------------------------------------------------
/*
 	\file		daemon.cpp

 	\brief		Daemon module which launch functor.
 				Functor is passed as argument of
 				member function DaemonStart().

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------


#include "daemon.h"
#include "help.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

using namespace std;


//SignalHandlerManager* Daemon::SigHandlerMngr = nullptr;


//---------------------------------------------------
//!	\note Definition of signal handler
//@{
void Daemon::SigHupHandler(int _sig)
{
	pthread_t tid = pthread_self();
	DebugMessage("Daemon SigHup() called from thread tid: %d", tid);
}

void Daemon::SigTermHandler(int _sig)
{
	pthread_t tid = pthread_self();
	DebugMessage("Daemon SigTerm() called from thread tid: %d", tid);
}
//@}

//---------------------------------------------------
/*
	\brief	Member function that redirect standard I/O
			(stdin, stdout and stderr) to /dev/null

	\return	Result of redirection
			0 - redirection successful
*/
//---------------------------------------------------
int Daemon::RedirectStandardStreams()
{
	if(freopen("/dev/null", "r+", stdin) == NULL)
	{
		ErrorMessage("Daemon redirect STDIN error: %s", strerror(errno));
		return -1;
	}

	if(freopen("/dev/null", "w+", stderr) == NULL)
	{
		ErrorMessage("Daemon redirect STDERR error: %s", strerror(errno));
		return -1;
	}

	if(freopen("/dev/null", "w+", stdout) == NULL)
	{
		ErrorMessage("Daemon redirect STDOUT error: %s", strerror(errno));
		return -1;
	}

	return 0;
}
//---------------------------------------------------
/*
	\brief	Member function which print informations
			about a process

	\param	_procName 	Label of process
*/
//---------------------------------------------------
void Daemon::PrintProcessInfo(const char* _procName)
{
#ifdef _DEBUG
	DebugMessage("%s: PID=%d, SID=%d, PPID= %d",_procName, getpid(), getsid(getpid()), getppid());
#endif //_DEBUG
}
//---------------------------------------------------
/*
	\brief	Function which initialize signal's handlers
			and register them into the SignalHandlerManager

	\param	_sigMng	Pointer to SignalHandlerManager
*/
//---------------------------------------------------
void Daemon::InitDaemonSignalHandlers(SignalHandlerManager* _sigMng)
{
//	SigHandlerMngr = _sigMng;
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sa.sa_handler = SigTermHandler;
	_sigMng->AddSignalHandler(SIGTERM, sa);

	sa.sa_handler = SigHupHandler;
	sa.sa_flags = SA_RESTART;
	_sigMng->AddSignalHandler(SIGHUP, sa);
}
//---------------------------------------------------
/*
	\brief	Metoda vytvori demona, ktery spusti
			predany funktor

	\param	_procName	Pojmenovani procesu
*/
//---------------------------------------------------
void Daemon::DaemonStart(const bool _redirectIO)
{
	pid_t pid, sid;

	PrintProcessInfo("Main process");

	// Create child process
	pid = fork();

	if(pid < 0)
	{
		// Create of child process failure
		HandleError("fork() error");
	}

	// Check which of processes is parent
	if(pid > 0)
	{
		// Terminate parent process
		exit(EXIT_SUCCESS);
	}

	// Only child process is running now
	PrintProcessInfo("Child process");

	// Set file mode mask (process doesn't take user rights)
	umask(0);

	// Change of current directory to root
	if(chdir("/") < 0)
	{
		// Change of directory failure
		HandleError("chdir() error");
	}

	// Set new session id
	sid = setsid();
	if(sid < 0)
	{
		// Setting of new session failure
		HandleError("setsid() error");
	}

	// Redirect of standard IO
	if(_redirectIO)
	{
		if(RedirectStandardStreams() < 0)
		{
			// Redirect of IO failure
			exit(EXIT_FAILURE);
		}
	}

	// Launch functor
//	void* ret = ((void*)0xFF);
//	ret = _func(_args);

	DebugMessage("Daemon is running now tid: %d", pthread_self());

//	exit(EXIT_SUCCESS);
}



