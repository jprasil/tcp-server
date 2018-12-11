/*
 * main.cpp
 *
 *  Created on: 10. 10. 2018
 *      Author: root
 */


#include "tcp_server.h"
#include <pthread.h>
#include <cstdlib>
#include <fstream>

#include "daemon.h"


#include "hw_monitor.h"

#include "help.h"
#include "signal_manager.h"
#include "comm.h"

#include <memory>
#include <cerrno>
#include <vector>

#include <unistd.h>
/*void SetSignalHandlers()
{
	signal(SIGTERM, );
}*/

/*int RunLoop(void* _arg)
{
	sleep(30);
	return 0;
}
*/

/*class TestClass
{
public:
	TestClass(int _val = 1);
	~TestClass();

private:
	int		Val;
};

TestClass::TestClass(int _val) : Val(_val)
{

}

TestClass::~TestClass()
{
	DebugMessage("Called destructor - val: %d", Val);
}
*/

//std::vector<std::unique_ptr<TestClass>> test;





void* TestHwMonitor(void*)
{
	int runningTime = 360;
	HwMonitor* mon = HwMonitor::GetInstance();
	mon->StartHwMonitor();

	while(runningTime)
	{
		sleep(1);
		runningTime--;
//		DebugMessage("CPU utilization: %Lf [%]", mon->GetCpuUtilization());
//		DebugMessage("MEM usage: %u [kB]", mon->GetMemUsage());
	}
	return 0;
}

std::unique_ptr<TcpServer> serverX;
std::unique_ptr<TcpServer> serverY;

void ProcessTerminated(void)
{
	DebugMessage("Process terminated successful");
}


int main() {


	pthread_attr_t atr;
	pthread_attr_init(&atr);
	pthread_t tID[2] = {0,0};


	if(atexit(ProcessTerminated) != 0)
	{
		DebugMessage("atexit() error");
	}

	DebugMessage("Main thread tid: %d", pthread_self());

	Daemon		*daemon = Daemon::GetInstance();
	Daemon::InitDaemonSignalHandlers(SignalHandlerManager::GetInstance());
	daemon->DaemonStart();

//	std::unique_ptr<TcpServer> serverX(new TcpServer);
//	std::unique_ptr<TcpServer> serverY(new TcpServer);
	serverX = std::unique_ptr<TcpServer>(new TcpServer);
	serverY = std::unique_ptr<TcpServer>(new TcpServer);

	serverY->SetSocketPort("5003");
	// Init signal handlers for TcpServer
	TcpServer::InitServerSignalHandlers(SignalHandlerManager::GetInstance());

	// Init communication
//	Comm commX{(commfunc_t)&send, (commfunc_t)&recv};
//Comm commY(&send, &recv);

	// Create server arguments
	server_args_t serverArg[2] = {{serverX.get(), ServerComm, nullptr}, {serverY.get(), ServerComm, nullptr}};


	HwMonitor::InitHwMonitorSignalHandlers(SignalHandlerManager::GetInstance());
	HwMonitor::GetInstance()->StartHwMonitor();


	TcpServer::RunServer(&serverArg[0]);

	// Run as daemon
//	Daemon		*daemon = Daemon::GetInstance();
//	Daemon::InitDaemonSignalHandlers(SignalHandlerManager::GetInstance());
//	daemon->DaemonStart(TcpServer::RunServer, &serverArg[0]);
//	daemon->DaemonStart(TestHwMonitor, nullptr);

/*	HwMonitor* monitor = HwMonitor::GetInstance();

	monitor->StartHwMonitor();

	sleep(120);
*/
	// Run thread
/*	int idx = 0;
	for(pthread_t &i : tID)
	{
		pthread_create(&i, &atr, TcpServer::RunServer, &serverArg[idx]);
		DebugMessage("Server thread created id: %d tid: %d", idx, i);
		idx++;
	}

	void* result;
	for(pthread_t i : tID)
	{
		DebugMessage("Main thread waiting for thread: %d", i);
		pthread_join(i, &result);
		DebugMessage("Thread tid: %d exited with code: %d", i, result);
	}

	DebugMessage("Main thread finishing tid: %d", pthread_self());
	pthread_attr_destroy(&atr);
*/
/*	ErrorMessage("Starting server");

	TcpServer::RunServer(nullptr);

	DebugMessage("ServerTerminated with: %d", errno);
*/


/*	test.push_back(std::unique_ptr<TestClass>(new TestClass(5)));
	test.push_back(std::unique_ptr<TestClass>(new TestClass(6)));

	perror("Working with error code");

	exit(EXIT_FAILURE);*/
//	_Exit(EXIT_FAILURE);
//	_Exit(EXIT_SUCCESS);

//	HwMonitor monitor;
//	monitor.GetCpuUtilization();

//	Daemon *daemon = Daemon::GetInstance();
//	daemon->DaemonStart(RunLoop);

/*	pid_t pid, sid;

	PrintProcessInfo("Main process");

	// Create child process
	pid = fork();

	if(pid < 0)
	{
		// Create child process failure
		cerr << "fork() error" << endl;
		_Exit(EXIT_FAILURE);
	}

	// Print child process info
	if(pid == 0)
	{
		PrintProcessInfo("Child process");
	}

	if(pid > 0)
	{
		PrintProcessInfo("Main process terminated");
		// Terminate parent process
		_Exit(EXIT_SUCCESS);
	}

	// Only child process is running now
	PrintProcessInfo("Child process");


	// Set file mode mask (process doesn't take user rights)
	umask(0);

	// Set new session id
	sid = setsid();

	if(sid < 0)
	{
		// Setting new session failure
		cerr << "setsid() error" << endl;
		_Exit(EXIT_FAILURE);
	}

	// Change the current directory to root
	chdir("/");

	PrintProcessInfo("Daemon process");
*/
//	TcpServer server;
//	int retVal = server.Run();

	pthread_attr_destroy(&atr);

	return 0;
}
