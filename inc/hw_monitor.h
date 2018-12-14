//---------------------------------------------------
/*
 	\file		hw_monitor.h

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

#ifndef INC_HW_MONITOR_H_
#define INC_HW_MONITOR_H_

#include "signal_manager.h"
#include <pthread.h>

//---------------------------------------------------
//! HW Monitor class implemented as singleton
class HwMonitor
{
public:
	//! Type definitions
	using cpupct_t = long double;
	using memusg_t = unsigned int;

public:
	static void InitHwMonitorSignalHandlers(SignalHandlerManager* _sigMng);	//!< Function initialize signal handlers
	static HwMonitor* GetInstance()											//!< Function gets pointer to instance of HwMonitor
	{
		static HwMonitor instance;
		return &instance;
	}

	void StartHwMonitor();								//!<
	void StopHwMonitor();
	~HwMonitor();
	auto GetCpuUtilization() -> cpupct_t;
	auto GetMemUsage() -> memusg_t;
	pthread_t GetTID();

	HwMonitor(HwMonitor&)		= delete;
	void operator=(HwMonitor&)	= delete;


private:
	static void* RunMonitor(void* _args);
	static void SigTermHandler(int _sig);
	static void SigIntHandler(int _sig);
	HwMonitor();

private:
	bool			Running;
	cpupct_t		CpuUtilization;
	memusg_t		MemUsage;
	pthread_mutex_t	Lock;
	pthread_t		TID;
};

#endif /* INC_HW_MONITOR_H_ */
