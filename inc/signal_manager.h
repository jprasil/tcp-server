/*
 * signal_manager.h
 *
 *  Created on: 5. 11. 2018
 *      Author: root
 */

#ifndef INC_SIGNAL_MANAGER_H_
#define INC_SIGNAL_MANAGER_H_

#include <signal.h>
#include <set>
#include <map>
#include <memory>

class SignalHandlerManager
{
public:
	//static void SignalHandler(int _sig);
	static SignalHandlerManager* GetInstance()
	{
		static SignalHandlerManager instance;
		return &instance;
	}

	void AddSignalHandler(int _sig,  /*__sighandler_t _handler*/struct sigaction& _sa);
	void BlockSignals(/*const int _sig*/);
	void UnblockSignals(/*const int _sig*/);


	SignalHandlerManager(SignalHandlerManager&) =	delete;
	void operator=(SignalHandlerManager&)		=	delete;

private:
	static void SignalHandler(int _sig);
	SignalHandlerManager(){sigemptyset(&SigSet);}


private:
	std::map<int, std::unique_ptr<std::set<__sighandler_t>>>	Handlers;
	sigset_t 													SigSet;		//!< Set of registered signals
};





#endif /* INC_SIGNAL_MANAGER_H_ */
