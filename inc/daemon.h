//---------------------------------------------------
/*
 	\file		daemon.h

 	\brief		Daemon module which launch functor.
 				Functor is passed as argument of
 				member function DaemonStart().

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
//	using run_t = void* (*)(void* _arg);			//!< Type declaration of functor

	static void InitDaemonSignalHandlers(SignalHandlerManager* _sigMng);
	static Daemon* GetInstance()				//!< Function gets instance of Daemon
	{
		static Daemon instance;
		return &instance;
	}

private:
	static void SigTermHandler(int _sig);
	static void SigHupHandler(int _sig);

//	static int DummyRun(void* _arg){return 0;}		//!< Definice defaultniho funktoru

	explicit Daemon(){}								//!< Konstruktor
	int RedirectStandardStreams();					//!< Funkce presmeruje standardni I/O
//	void SetSignalHendlers();						//!< Setting signal's handlers
	void PrintProcessInfo(const char* _procName);	//!< Vypise info o procesu

public:
	Daemon(Daemon&)				= delete;				//!< Odstraneni kopirovaciho konstruktoru
	void operator=(Daemon&)		= delete;				//!< Odstraneni operatoru =
	void DaemonStart(/*run_t _func, void*_args=nullptr*/);	//!< Start demona

private:
//	static SignalHandlerManager*	SigHandlerMngr;

};




#endif /* INC_DAEMON_H_ */
