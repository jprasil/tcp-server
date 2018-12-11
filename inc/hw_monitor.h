/*
 * hw_monitor.h
 *
 *  Created on: Oct 26, 2018
 *      Author: root
 */

#ifndef INC_HW_MONITOR_H_
#define INC_HW_MONITOR_H_

#include "signal_manager.h"
#include <pthread.h>


class HwMonitor
{
public:
	using cpupct_t = long double;
	using memusg_t = unsigned int;

public:
	static void InitHwMonitorSignalHandlers(SignalHandlerManager* _sigMng);
	static HwMonitor* GetInstance()
	{
		static HwMonitor instance;
		return &instance;
	}

	void StartHwMonitor();
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
