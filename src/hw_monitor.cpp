//---------------------------------------------------
/*
 	\file		hw_monitor.cpp

 	\brief		HW monitor module
 				Calling of function StartHwMonitor()
 				starts periodic monitoring of cpu
 				utilization and	mem usage.
 				(It execution in separate thread)

	\date		26.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------


#include "hw_monitor.h"
#include "help.h"

#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <map>

#include <fstream>


//---------------------------------------------------
/*
	\brief	Function which initialize signal handlers
			and register them into the SignalHandlerManager

	\param	_sigMng	Pointer to SignalHandlerManager
*/
//---------------------------------------------------
void HwMonitor::InitHwMonitorSignalHandlers(SignalHandlerManager* _sigMng)
{
//	SigHandlerMngr = _sigMng;
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sa.sa_handler = SigTermHandler;
	_sigMng->AddSignalHandler(SIGTERM, sa);

	sa.sa_handler = SigTermHandler;
	_sigMng->AddSignalHandler(SIGINT, sa);
}

//---------------------------------------------------
/*
	\brief	Signal handler for signal SIGTERM

	\param	_sig	Signal number
*/
//---------------------------------------------------
void HwMonitor::SigTermHandler(int _sig)
{
	pthread_t tid = pthread_self();
	HwMonitor* monitor = GetInstance();

	DebugMessage("HwMonitor SigTerm() called from thread tid: %d", tid);

//	if(tid == monitor->GetTID())
	if(tid == monitor->TID)
	{
		monitor->StopHwMonitor();
	}
	else
	{
		pthread_mutex_lock(&monitor->Lock);
		monitor->StopHwMonitor();
		pthread_mutex_unlock(&monitor->Lock);
	}
}
//---------------------------------------------------
/*
	\brief	Constructor
*/
//---------------------------------------------------
HwMonitor::HwMonitor() :
	Running(false),
	CpuUtilization(0),
	MemUsage(0),
	TID(0)
{
	Lock = PTHREAD_MUTEX_INITIALIZER;
}
//---------------------------------------------------
/*
	\brief	Destructor
*/
//---------------------------------------------------
HwMonitor::~HwMonitor()
{
	if(TID)
		pthread_cancel(TID);
}
//---------------------------------------------------
/*
	\brief	Start RunMonitor() in separate
			thread
*/
//---------------------------------------------------
void HwMonitor::StartHwMonitor()
{
	pthread_mutex_lock(&Lock);

	if(!Running)
	{
		int res = pthread_create(&TID, nullptr, RunMonitor, nullptr);
		Running = (res != 0) ? false : true;
	}

	pthread_mutex_unlock(&Lock);
}
//---------------------------------------------------
/*
	\brief	Stop HW monitor and terminate thread
*/
//---------------------------------------------------
void HwMonitor::StopHwMonitor()
{
	Running = false;
}
//---------------------------------------------------
/*
	\brief	Finds out HW resources
*/
//---------------------------------------------------
void* HwMonitor::RunMonitor(void* _args)
{
	cpupct_t a[4], b[4];
	FILE *procstat;
	HwMonitor* monitor = GetInstance();
	bool running = true;

	std::string tmpStr;
	std::ifstream procmeminfo;
	std::map<const std::string, memusg_t> memValues = {{"MemTotal:", 0}, {"MemFree:", 0}, {"Buffers:", 0}, {"Cached:", 0}};
	const int parseFail = 0xFFFF;


	while(running)
	{
		//-------------------------------------------------------
		//! Measure processor utilization
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

		procstat = fopen("/proc/stat","r");
		fscanf(procstat, "%*s %Lf %Lf %Lf %Lf", &a[0],&a[1],&a[2],&a[3]);
		fclose(procstat);

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		sleep(1);

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

		procstat = fopen("/proc/stat","r");
		fscanf(procstat,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
		fclose(procstat);

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		monitor->CpuUtilization = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3])) * 100.0;

		//------------------------------------------------------
		//! Measure memory usage
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

		procmeminfo.open("/proc/meminfo", std::ifstream::in);

		int load = 0;
		while(load < 4)
		{
			procmeminfo >> tmpStr;
			auto it = memValues.find(tmpStr);
			if(it != memValues.end())
			{
				procmeminfo >> it->second;
				load++;
			}

			if(procmeminfo.eof())
			{
				load = parseFail;
			}
		}

		procmeminfo.close();

		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		if(load != parseFail)
		{
			monitor->MemUsage = memValues.at("MemTotal:") - memValues.at("MemFree:") - memValues.at("Buffers:") - memValues.at("Cached:");
		}

		pthread_mutex_lock(&monitor->Lock);
		running = monitor->Running;
		pthread_mutex_unlock(&monitor->Lock);

	}
	return EXIT_SUCCESS;
}

//---------------------------------------------------
/*
	\brief	Getters
*/
//---------------------------------------------------
//@{
auto HwMonitor::GetCpuUtilization() -> cpupct_t
{
	return CpuUtilization;
}

auto HwMonitor::GetMemUsage() -> memusg_t
{
	return MemUsage;
}

pthread_t HwMonitor::GetTID()
{
	return TID;
}
//@}
