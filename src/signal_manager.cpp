/*
 * signal_manager.cpp
 *
 *  Created on: 5. 11. 2018
 *      Author: root
 */

#include "signal_manager.h"
#include "help.h"

#include <utility>



void SignalHandlerManager::SignalHandler(int _sig)
{
	DebugMessage("Signal num: %d raised, catched in thread tid: %d", _sig, pthread_self());
	SignalHandlerManager* sigMng = SignalHandlerManager::GetInstance();

	sigMng->BlockSignals();

	auto it = sigMng->Handlers.find(_sig);
	if(it != sigMng->Handlers.end())
	{
		std::set<__sighandler_t> *handlers = it->second.get();
		for(__sighandler_t h: *handlers)
			h(_sig);
	}

	sigMng->UnblockSignals();
}

void SignalHandlerManager::AddSignalHandler(int _sig, /*__sighandler_t _handler*/struct sigaction& _sa)
{
//	std::map<int, std::unique_ptr<std::set<__sighandler_t>>>::iterator it;

	auto it = Handlers.find(_sig);
	if(it == Handlers.end())
	{
		// No handler for signal _sig
		DebugMessage("No handlers are registered for signal: %d", _sig);
		sigaddset(&SigSet, _sig);
		std::set<__sighandler_t> *newSet = new std::set<__sighandler_t>;
		newSet->insert(_sa.sa_handler);
		Handlers.insert(std::pair<int, std::unique_ptr<std::set<__sighandler_t>>>(_sig, std::move(newSet)));
		_sa.sa_handler = SignalHandler;
		sigaction(_sig, &_sa, nullptr);

	}
	else
	{
		// Handler exist for signal _sig
		std::set<__sighandler_t> *set = it->second.get();
		set->insert(_sa.sa_handler);
		DebugMessage("Adding new handler for signal: %d", _sig);
	}

	it = Handlers.find(_sig);
	DebugMessage("Handlers <map> size: %d", Handlers.size());
	DebugMessage("Number of handlers for sig-%d: %d", _sig, it->second.get()->size());

}

void SignalHandlerManager::BlockSignals(/*const int _sig*/)
{
	pthread_sigmask(SIG_BLOCK, &SigSet, nullptr);
}

void SignalHandlerManager::UnblockSignals()
{
	pthread_sigmask(SIG_UNBLOCK, &SigSet, nullptr);
}

