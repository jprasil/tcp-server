//---------------------------------------------------
/*
 	\file		signal_manager.h

 	\brief		Signal handler manager module
 				It distribute received signal into the
 				registered signal handlers

	\date		5.11.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#ifndef INC_SIGNAL_MANAGER_H_
#define INC_SIGNAL_MANAGER_H_

#include <signal.h>
#include <set>
#include <map>
#include <memory>

//---------------------------------------------------
//! Signal handler manager class implemented as singleton
class SignalHandlerManager
{
public:
	static SignalHandlerManager* GetInstance()					//!< Returns pointer to instance of SignalHandlerManager
	{
		static SignalHandlerManager instance;
		return &instance;
	}

	void AddSignalHandler(int _sig, struct sigaction& _sa);		//!< Function add a signal handler for particular signal
	void BlockSignals(/*const int _sig*/);						//!< Function block receiving of all registered signals
	void UnblockSignals(/*const int _sig*/);					//!< Function unblock receiving of all registered signals


	SignalHandlerManager(SignalHandlerManager&) =	delete;		//!< Delete copy constructor
	void operator=(SignalHandlerManager&)		=	delete;		//!< Delete operator =

private:
	static void SignalHandler(int _sig);						//!< Signal handler for every registered signal (it call registered handlers)
	SignalHandlerManager(){sigemptyset(&SigSet);}				//!< Constructor


private:
	std::map<int, std::unique_ptr<std::set<__sighandler_t>>>	Handlers;	//!< Map of registered signals and handlers (every registered signal has at least one registered handler)
	sigset_t 													SigSet;		//!< Set of registered signals
};





#endif /* INC_SIGNAL_MANAGER_H_ */
