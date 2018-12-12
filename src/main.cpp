//---------------------------------------------------
/*
 	\file		main.cpp

 	\brief		Application of TCP server which
 				react on two commands

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#include "tcp_server.h"
#include "daemon.h"
#include "hw_monitor.h"
#include "help.h"
#include "signal_manager.h"
#include "comm.h"

#include <cstdlib>
#include <memory>
#include <cerrno>



//--------------------------------------------------
/*
	\brief	Callback called when process is terminated
			successful
*/
//--------------------------------------------------
void cbTerminatedSuccessful(void)
{
	DebugMessage("Process terminated successful");
}

//--------------------------------------------------
/*
	\brief	Application entry point
*/
//--------------------------------------------------
int main()
{
	// Registered function will be called at normal program termination
	if(atexit(cbTerminatedSuccessful) != 0)
	{
		DebugMessage("atexit() error");
	}

	DebugMessage("Main thread tid: %d", pthread_self());


	// Initialize daemon signal handlers
	Daemon::InitDaemonSignalHandlers(SignalHandlerManager::GetInstance());
	// Runs the program as daemon
	Daemon::GetInstance()->DaemonStart(false);


	// Create instance of the TcpServer
	std::unique_ptr<TcpServer> server(new TcpServer);
	// Initialize server signal handlers
	TcpServer::InitServerSignalHandlers(SignalHandlerManager::GetInstance());
	// Create server arguments
	server_args_t serverArg = {server.get(), ServerComm, nullptr};


	// Initialize HW Monitor signal handlers
	HwMonitor::InitHwMonitorSignalHandlers(SignalHandlerManager::GetInstance());


	// Runs HW Monitor
	HwMonitor::GetInstance()->StartHwMonitor();
	// Runs server
	TcpServer::RunServer(&serverArg);



	return 0;
}
