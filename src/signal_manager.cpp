//---------------------------------------------------
/*
 	\file		signal_manager.cpp

 	\brief		Signal handler manager module
 				It distribute received signal into the
 				registered signal handlers

	\date		5.11.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#include "signal_manager.h"
#include "help.h"

#include <utility>


//---------------------------------------------------
/*
	\brief	Signal handler which is registered
			to the system for given signal _sig.
			It call all registered handlers of given
			signal

	\param	_sig	Signal has invoked this handler

*/
//---------------------------------------------------
void SignalHandlerManager::SignalHandler(int _sig)
{
	DebugMessage("Signal num: %d raised, catched in thread tid: %d", _sig, pthread_self());
	SignalHandlerManager* sigMng = SignalHandlerManager::GetInstance();

	// Block receiving of all registered signals
	sigMng->BlockSignals();

	// Attempt to find passed signal
	auto it = sigMng->Handlers.find(_sig);
	// Determine whether signal is registered
	if(it != sigMng->Handlers.end())
	{
		// Signal is registered, call all sig. handlers for this one
		std::set<__sighandler_t> *handlers = it->second.get();
		for(__sighandler_t h: *handlers)
			h(_sig);
	}

	// Unblock receiving of all registered signals
	sigMng->UnblockSignals();
}
//---------------------------------------------------
/*
	\brief	Register signal handler for passed signal

	\param	_sig	Signal
			_sa		Sigaction struct which contains
					signal handler for _sig
*/
//---------------------------------------------------
void SignalHandlerManager::AddSignalHandler(int _sig, /*__sighandler_t _handler*/struct sigaction& _sa)
{
	// Attempt to find passed signal
	auto it = Handlers.find(_sig);
	// Check whether this signal is registered
	if(it == Handlers.end())
	{
		// Given signal isn't registered
		DebugMessage("No handlers are registered for signal: %d", _sig);
		// Add the signal into the SigSet
		sigaddset(&SigSet, _sig);
		// Create a set of signal handlers for given signal
		std::set<__sighandler_t> *newSet = new std::set<__sighandler_t>;
		// Register signal handler into the set
		newSet->insert(_sa.sa_handler);
		// Register signal and associated set containing signal handlers
		Handlers.insert(std::pair<int, std::unique_ptr<std::set<__sighandler_t>>>(_sig, std::move(newSet)));
		// Register SignalHandlerManager::SignalHandler into the system
		_sa.sa_handler = SignalHandler;
		sigaction(_sig, &_sa, nullptr);

	}
	else
	{
		// Given signal is registered - there are registered other signal handlers for this one
		// Get set of signal handlers for passed signal
		std::set<__sighandler_t> *set = it->second.get();
		// Register signal handler into the set for this one
		set->insert(_sa.sa_handler);
		DebugMessage("Adding new handler for signal: %d", _sig);
	}

	// Print information about registered signals and signal handlers
	it = Handlers.find(_sig);
	DebugMessage("Handlers <map> size: %d", Handlers.size());
	DebugMessage("Number of handlers for signal %d: %d", _sig, it->second.get()->size());

}
//---------------------------------------------------
/*
	\brief	Disable receiving all registered signals
*/
//---------------------------------------------------
void SignalHandlerManager::BlockSignals()
{
	pthread_sigmask(SIG_BLOCK, &SigSet, nullptr);
}
//---------------------------------------------------
/*
	\brief	Enable receiving all registered signals
*/
//---------------------------------------------------
void SignalHandlerManager::UnblockSignals()
{
	pthread_sigmask(SIG_UNBLOCK, &SigSet, nullptr);
}

