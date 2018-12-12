//---------------------------------------------------
/*
 	\file		daemon.h

 	\brief		Daemon module
 				Calling DaemonStart() function ensure
 				that program will run as daemon

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------


#ifndef INC_DAEMON_H_
#define INC_DAEMON_H_

#include "signal_manager.h"

//---------------------------------------------------
//! Daemon class which is implemented as Singleton
class Daemon
{
public:
	static void InitDaemonSignalHandlers(SignalHandlerManager* _sigMng);	//!< Function initialize signal handlers
	static Daemon* GetInstance()											//!< Function gets instance of Daemon
	{
		static Daemon instance;						//!< Instance of daemon
		return &instance;
	}

private:
	static void SigTermHandler(int _sig);			//!< SIGTERM handler
	static void SigHupHandler(int _sig);			//!< SIGHUB handler

	explicit Daemon(){}								//!< Constructor
	int RedirectStandardStreams();					//!< Function redirects standard I/O
	void PrintProcessInfo(const char* _procName);	//!< Function prints informations about process

public:
	Daemon(Daemon&)				= delete;			//!< Delete copy constructor
	void operator=(Daemon&)		= delete;			//!< Delete operator =
	void DaemonStart(const bool _redirectIO);		//!< Function starts program as daemon

};




#endif /* INC_DAEMON_H_ */
