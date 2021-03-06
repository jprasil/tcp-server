//---------------------------------------------------
/*
 	\file		main.cpp

 	\brief		Application of TCP server which
 				accepts two commands - mem, cpu

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


	// Initialize signal handlers for daemon
	Daemon::InitDaemonSignalHandlers(SignalHandlerManager::GetInstance());
	// Runs the program as daemon
#ifdef _DEBUG
	Daemon::GetInstance()->DaemonStart(false);
#else
	Daemon::GetInstance()->DaemonStart(true);
#endif //_DEBUG


	// Create instance of the TcpServer
	std::unique_ptr<TcpServer> server(new TcpServer);
	// Initialize signal handlers for server
	TcpServer::InitServerSignalHandlers(SignalHandlerManager::GetInstance());
	// Create server's arguments
	server_args_t serverArg = {server.get(), ServerComm, nullptr};


	// Initialize signal handlers for HW Monitor
	HwMonitor::InitHwMonitorSignalHandlers(SignalHandlerManager::GetInstance());


	// Run HW Monitor
	HwMonitor::GetInstance()->StartHwMonitor();
	// Run server
	TcpServer::RunServer(&serverArg);

	return 0;
}
