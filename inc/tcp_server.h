//---------------------------------------------------
/*
 	\file		tcp_server.h

 	\brief		TCP server module
 				Calling of function RunServer() starts
 				listening of server on given port and
 				IP address.

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
	using client_rq_routine = int (*)(int _clientDesc, void* _args);					//!< User defined routine
private:
	using client_thread_args_t = struct client_thread_args;								//!< Type definition of argument for thread which processes client request
	using thread_pool_t = std::map<pthread_t, std::unique_ptr<client_thread_args_t>>;	//!< Type definition of client's thread pool


public:
	static void* RunServer(void* _s_args);												//!< Function starts server
	static void InitServerSignalHandlers(SignalHandlerManager* _sigMng);				//!< Function initialize signal handlers

	TcpServer(in_addr_t _addr=INADDR_LOOPBACK, in_port_t _port=5001, int _maxPending=100);	//!< Constructor
	~TcpServer();

	void SetSocketAddr(in_addr_t _addr);												//!< Function sets an ip address for socket
	void SetSocketAddr(const char *_addr);												//!< Overloaded function - sets an ip address for socket
	void SetSocketPort(in_port_t _port);												//!< Function sets a port number
	void SetSocketPort(const char* _port);												//!< Overloaded function - sets a port number

private:
	static void SigTermHandler(int _sig);												//!< SIGTERM handler
	static void SigIntHandler(int _sig);												//!< SIGINT handler
	static void SigAlrmHandler(int _sig);												//!< SIGALRM handler
	static void* ProcessClientRequest(void* _args);										//!< Function process client's request

private:
	bool					Running;			//!< Flag that server is running
	int						TcpSocket;			//!< Server's socket descriptor
	sockaddr_in				SocketAddr;			//!< Socket address
	int						MaxPending;			//!< Maximum pending connections
	thread_pool_t			ThreadPool;			//!< Pool of threads which processing client's requests
	pthread_mutex_t			PoolLock;			//!< Mutex protect an access to ThreadPool
	sem_t					Semaphore;			//!< Semaphore protect the server from overloading
	int						ServerID;			//!< server's ID
	static int				InstanceCnt;		//!< Counter of server's instances (simultaneously it used for generating ID)
};


//---------------------------------------------------
//! Server's arguments passed to the RunServer() function
struct server_args
{
	TcpServer* 						ptrServer;		//!< Pointer to instance of server
	TcpServer::client_rq_routine	rqRoutine;		//!< Pointer to function that process client's request
	void*							rqRoutineArgs;	//!< Pointer to argument passed to the rqRoutine
};
//! Type definition of the arguments
using server_args_t = struct server_args;




#endif /* INC_TCP_SERVER_H_ */
