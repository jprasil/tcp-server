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

	void StartHwMonitor();								//!< Start HW monitor in separate thread
	void StopHwMonitor();								//!< Stop HW monitor
	~HwMonitor();										//!< Desctructor
	//-------------------------------------
	//!	Getters
	//@{
	auto GetCpuUtilization() -> cpupct_t;
	auto GetMemUsage() -> memusg_t;
	pthread_t GetTID();
	//@}

	HwMonitor(HwMonitor&)		= delete;				//!< Delete constructor
	void operator=(HwMonitor&)	= delete;				//!< Delete operator =


private:
	static void* RunMonitor(void* _args);				//!< Function finds out HW resources
	static void SigTermHandler(int _sig);				//!< SIGTERM handler
	static void SigIntHandler(int _sig);				//!< SIGINT handler
	HwMonitor();										//!< Constructor

private:
	bool			Running;					//!< Running flag
	cpupct_t		CpuUtilization;				//!< CPU utilization [%]
	memusg_t		MemUsage;					//!< Memory usage [kb]
	pthread_mutex_t	Lock;						//!< Mutex
	pthread_t		TID;						//!< Thread ID
};

#endif /* INC_HW_MONITOR_H_ */
