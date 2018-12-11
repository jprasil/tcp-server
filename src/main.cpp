//---------------------------------------------------
/*
 	\file		main.cpp

 	\brief		Start of the program

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#include "tcp_server.h"

#include <cstdlib>


#include "daemon.h"


#include "hw_monitor.h"

#include "help.h"
#include "signal_manager.h"
#include "comm.h"

#include <memory>
#include <cerrno>


//#include <unistd.h>



//std::unique_ptr<TcpServer> server;

//--------------------------------------------------

//--------------------------------------------------
void ProcessTerminated(void)
{
	DebugMessage("Process terminated successful");
}

//--------------------------------------------------

//--------------------------------------------------
int main() {

	// Registered function will be called at normal program termination
	if(atexit(ProcessTerminated) != 0)
	{
		DebugMessage("atexit() error");
	}

	DebugMessage("Main thread tid: %d", pthread_self());


	// Initialize of daemon
	Daemon::InitDaemonSignalHandlers(SignalHandlerManager::GetInstance());
	// Runs the program as daemon
	Daemon::GetInstance()->DaemonStart(false);


	// Create instance of the TcpServer
	std::unique_ptr<TcpServer> server(new TcpServer);
	//server = std::unique_ptr<TcpServer>(new TcpServer);
	// Initialize server signal handlers
	TcpServer::InitServerSignalHandlers(SignalHandlerManager::GetInstance());
	// Create server arguments
	server_args_t serverArg = {server.get(), ServerComm, nullptr};


	// Initialize of HW Monitor
	HwMonitor::InitHwMonitorSignalHandlers(SignalHandlerManager::GetInstance());



	// Runs HW Monitor
	HwMonitor::GetInstance()->StartHwMonitor();
	// Runs server
	TcpServer::RunServer(&serverArg);



	return 0;
}
