//---------------------------------------------------
/*
 	\file		tcp_server.h

 	\brief		TCP server module

	\date		29.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------


#ifndef INC_TCP_SERVER_H_
#define INC_TCP_SERVER_H_

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <map>
#include <memory>
#include <semaphore.h>

#include "signal_manager.h"

//---------------------------------------------------
//! Definitions of return values
#define TCP_SERVER_SUCCESS		((void*)0)
#define TCP_SERVER_ERROR		((void*)1)


//---------------------------------------------------
//! Class of tcp server
class TcpServer
{
public:
	using client_rq_routine = int (*)(int _clientDesc, void* _args);			//!< User defined routine
private:
	using client_thread_args_t = struct client_thread_args;
	using thread_pool_t = std::map<pthread_t, std::unique_ptr<client_thread_args_t>>;


public:
	static void* RunServer(void* _s_args);
	static void InitServerSignalHandlers(SignalHandlerManager* _sigMng);

	TcpServer(in_addr_t _addr=INADDR_LOOPBACK, in_port_t _port=5001, int _maxPending=100);
	~TcpServer();

	void SetSocketAddr(in_addr_t _addr);
	void SetSocketAddr(const char *_addr);
	void SetSocketPort(in_port_t _port);
	void SetSocketPort(const char* _port);

private:
	static void SigTermHandler(int _sig);
	static void SigIntHandler(int _sig);
	static void SigAlrmHandler(int _sig);
	static void* ProcessClientRequest(void* _args);

private:
	bool					Running;
	int						TcpSocket;
	sockaddr_in				SocketAddr;
	int						MaxPending;
	thread_pool_t			ThreadPool;
	pthread_mutex_t			PoolLock;
	sem_t					Semaphore;
	int						ServerID;
	static int				InstanceCnt;
//	static SignalHandlerManager* SigHandlerMngr;
};


//---------------------------------------------------
//! Server's init arguments passed to the thread (resp. daemon)
struct server_args
{
	TcpServer* 						ptrServer;
	TcpServer::client_rq_routine	rqRoutine;
	void*							rqRoutineArgs;
};

using server_args_t = struct server_args;




#endif /* INC_TCP_SERVER_H_ */
