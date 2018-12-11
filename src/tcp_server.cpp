//---------------------------------------------------
/*
 	\file		tcp_server.cpp

 	\brief		TCP server module -

	\date		29.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------

#include "tcp_server.h"
#include "help.h"
#include "pit.h"

#include <cstdio>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <fcntl.h>

using namespace std;



//---------------------------------------------------
//! Definition of private variables
//@{
int TcpServer::InstanceCnt = 0; 					//!< ID generator
//SignalHandlerManager* TcpServer::SigHandlerMngr = nullptr;
static map<pthread_t, TcpServer*> RunningServers;	//!< Register of running threads

//@}


//---------------------------------------------------
//! Server mutex
static pthread_mutex_t ServerMutex = PTHREAD_MUTEX_INITIALIZER;
//---------------------------------------------------
/*
	\brief	Function for lock/unlock server mutex
*/
//---------------------------------------------------
#define ServerLock() \
	do{ \
		int ret = pthread_mutex_lock(&ServerMutex); \
		if(ret != 0) \
			ErrorMessage("mutex_lock() ServerMutex error: %s line: %d", strerror(ret), __LINE__); \
	}while(0)



#define ServerUnlock() \
	do{ \
		int ret = pthread_mutex_unlock(&ServerMutex); \
		if(ret != 0) \
			ErrorMessage("mutex_unlock() ServerMutex error: %s line: %d", strerror(ret), __LINE__); \
	}while(0)

//---------------------------------------------------


//---------------------------------------------------
//! Definition of interval timer period [s]
#define INTERVAL_TIMER_PERIOD		20 //[s]
//---------------------------------------------------
//! Definition of own signal
//#define SIGFREE SIGRTMIN

//---------------------------------------------------
//! Private arguments
struct client_thread_args
{
	TcpServer						*ptrServer;
	int								clientDesc;		//!< Client socket descriptor
	TcpServer::client_rq_routine	rqRoutine;
	void* 							args;			//!< Arguments for rqRoutine
};



//---------------------------------------------------
/*
	\brief	Constructor

	\param	_addr		Server's IP address
			_port		Server's port
			_maxPendig	Maximum pending connections

*/
//---------------------------------------------------
TcpServer::TcpServer(in_addr_t _addr, in_port_t _port, int _maxPending) :
		TcpSocket(-1),
		MaxPending(_maxPending),
		Running(true),
		ServerID(InstanceCnt)
{
	InstanceCnt++;
	SocketAddr.sin_addr.s_addr = htonl(_addr);
	SocketAddr.sin_port = htons(_port);
	SocketAddr.sin_family = AF_INET;

	PoolLock = PTHREAD_MUTEX_INITIALIZER;
}

TcpServer::~TcpServer()
{
	DebugMessage("Server id: %d destroyed", ServerID);
	if(fcntl(TcpSocket, F_GETFD) != -1 || errno != EBADF)
	{
		if(close(TcpSocket) == -1)
			HandleError("Error during socket closing");
	}
}

//---------------------------------------------------
/*
	\brief	Member function for setting IPv4 address
			as text

	\param	_addr	Text representation of address
					(e.g. "172.0.0.1")
*/
//---------------------------------------------------
void TcpServer::SetSocketAddr(const char* _addr)
{
	inet_pton(AF_INET, _addr, &SocketAddr.sin_addr);
}
//---------------------------------------------------
/*
	\brief	Member function for setting IPv4 address
			as number

	\param	_addr	Number format of address
					(e.g. 0x7f000001 -> 127.0.0.1)
*/
//---------------------------------------------------
void TcpServer::SetSocketAddr(in_addr_t _addr)
{
	SocketAddr.sin_addr.s_addr = htonl(_addr);;
}

//---------------------------------------------------
/*
	\brief	Member function for setting port as number

	\param	_port	Port number
*/
//---------------------------------------------------
void TcpServer::SetSocketPort(in_port_t _port)
{
	SocketAddr.sin_port = htons(_port);
}
//---------------------------------------------------
/*
	\brief	Member function for setting port as text

	\param	_port	Port number as text
*/
//---------------------------------------------------
void TcpServer::SetSocketPort(const char* _port)
{
	SocketAddr.sin_port = htons(atoi(_port));
}

void TcpServer::InitServerSignalHandlers(SignalHandlerManager* _sigMng)
{
//	SigHandlerMngr = _sigMng;
	pthread_t tid = pthread_self();
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SigTermHandler;
	sa.sa_flags = 0;
	_sigMng->AddSignalHandler(SIGTERM, sa);

	sa.sa_handler = SigTermHandler;
	_sigMng->AddSignalHandler(SIGINT, sa);

//	sigaddset(&sa.sa_mask, SIGALRM);
//	pthread_sigmask(SIG_SETMASK, &sa.sa_mask, nullptr);
	sa.sa_handler = SigAlrmHandler;
	_sigMng->AddSignalHandler(SIGALRM, sa);
}

void TcpServer::SigAlrmHandler(int _sig)
{
	pthread_t tid = pthread_self();
	DebugMessage("TcpServer SigAlrm() called from thread tid: %d", tid);

	auto it = RunningServers.find(tid);
	if(it == RunningServers.end())
	{
		for(auto it = RunningServers.begin(); it != RunningServers.end(); ++it)
		{
			DebugMessage("Send SIGALRM to thread tid: %d", it->first);
			pthread_kill(it->first, SIGALRM);
		}
	}

}

void TcpServer::SigIntHandler(int _sig)
{
	pthread_t tid = pthread_self();
	DebugMessage("TcpServer SigInt() called from thread tid: %d", tid);

	auto it = RunningServers.find(tid);
	if(it != RunningServers.end())
	{
		if(it->second->Running)
		{
			DebugMessage("Terminate thread tid: %d", it->first);
			it->second->Running = false;
			pthread_kill(it->first, _sig);
		}
		else
		{
			RunningServers.erase(it->first);
			DebugMessage("Unregister server ID: %d, tid: %d from thread: %d", it->second->ServerID, it->first, tid);
		}
	}

}

void TcpServer::SigTermHandler(int _sig)
{
	pthread_t tid = pthread_self();
	DebugMessage("TcpServer SigTerm() called from thread tid: %d", tid);

	for(auto& it : RunningServers)
	{
		if(it.second->Running)
		{
			DebugMessage("Terminate server ID: %d, tid: %d from thread: %d", it.second->ServerID, it.first, tid);
			it.second->Running = false;
			RunningServers.erase(it.first);
			pthread_kill(it.first, _sig);
		}
		else
		{
			DebugMessage("TOTO BY NEMELO NASTAT: %d", tid);
		}
	}

}
//---------------------------------------------------
/*
	\brief	Function runs tcp server

	\param	_s_args	Pointer to TcpServer attributes

	\return
*/
//---------------------------------------------------
void* TcpServer::RunServer(void* _s_args)
{
	pthread_t tid = pthread_self();
	server_args_t *serverAttr;
	bool running = true;
	int yes = 1;

	if(_s_args == nullptr)
	{
		// No arguments passed to the server
		ErrorMessage("No arguments for server tid: %d", tid);
		pthread_exit(TCP_SERVER_ERROR);
	}

	// Get server's arguments
	serverAttr = reinterpret_cast<server_args_t*>(_s_args);
	TcpServer* server = serverAttr->ptrServer;

	// Register server thread
	ServerLock();

	auto ret = RunningServers.insert(pair<pthread_t, TcpServer*>(tid, server));
	if(ret.second == false)
	{
		DebugMessage("Server can't be registered tid: %d, id: %d", tid, server->ServerID);
		ServerUnlock();
		pthread_exit(TCP_SERVER_ERROR);
	}

	ServerUnlock();

	// Creating of socket based on TCP/IP protocol
	server->TcpSocket = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/, IPPROTO_TCP);
	if(server->TcpSocket == -1)
	{
		HandleError("socket() error");
	}

	// Ensure that restart of server will be successful
	if(setsockopt(server->TcpSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		HandleError("setsockopt() error");
	}

	// Assigning of address and port to the socket
	if(bind(server->TcpSocket, reinterpret_cast<sockaddr*>(&server->SocketAddr), sizeof(SocketAddr)) == -1)
	{
		HandleError("bind() error");
	}

	// Accepting incoming connections
	if(listen(server->TcpSocket, server->MaxPending) == -1)
	{
		HandleError("listen() error");
	}

	// Set interval timer which generated interrupt system call
	pit::SetPIT(INTERVAL_TIMER_PERIOD, 0);

	int client, result;
	sockaddr_in clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);
	pthread_attr_t attr;

	// Initialize thread's attributes
	result = pthread_attr_init(&attr);
	if(result != 0)
	{
		HandleError_ErrNum(result, "pthread_attr_init() error: %d");
	}

	// Semaphore initializing
	if(sem_init(&server->Semaphore, 0 /*thread-shared*/, server->MaxPending) == -1)
	{
		HandleError("sem_init() error");
	}

	while(server->Running)
	{
		client = accept(server->TcpSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);

		// Test client descriptor
		if(client == -1)
		{
			ErrorMessage("accept() error: %s", strerror(errno));
		}
		else
		{
			// Try to acquire semaphore
			if(sem_wait(&server->Semaphore) == -1)
			{
				ErrorMessage("sem_wait() error: %s", strerror(errno));
			}

			if(serverAttr->rqRoutine != nullptr)
			{
				pthread_t ID;
				auto tArgs = unique_ptr<client_thread_args_t>(new client_thread_args_t({server, client, serverAttr->rqRoutine, serverAttr->rqRoutineArgs}));

				result = pthread_mutex_lock(&server->PoolLock);
				if(result != 0)
					ErrorMessage("mutex_lock() PoolLock error: %s line: %d", strerror(result), __LINE__);

				result = pthread_create(&ID, &attr, ProcessClientRequest, reinterpret_cast<void*>(tArgs.get()));
				if(result != 0)
				{
					ErrorMessage("pthread_create() error: %s", strerror(result));
				}
				else
				{
					// Register thread to pool
					server->ThreadPool.insert(pair<pthread_t, unique_ptr<client_thread_args_t>>(ID, move(tArgs)));
				}

				result = pthread_mutex_unlock(&server->PoolLock);
				if(result != 0)
					ErrorMessage("mutex_unlock() PoolLock error: %s line: %d", strerror(result), __LINE__);
			}
		}
	}

	// Finish all child threads

	result = pthread_mutex_lock(&server->PoolLock);
	if(result != 0)
		ErrorMessage("mutex_lock() PoolLock error: %s line: %d", strerror(result), __LINE__);
	// All threads in the ThreadPool are running or blocked now
	// Vector of registered threads's ID
	vector<pthread_t> threadsID;
	// Send cancel request all of them
	for(auto &i: server->ThreadPool)
	{
		threadsID.push_back(i.first);
		pthread_cancel(i.first);
	}

	result = pthread_mutex_unlock(&server->PoolLock);
	if(result != 0)
		ErrorMessage("mutex_unlock() PoolLock error: %s line: %d", strerror(result), __LINE__);

	// Waiting for threads
	void* retVal = nullptr;
	for(auto &i: threadsID)
	{
		result = pthread_join(i, &retVal);
		if(result != 0)
		{
			ErrorMessage("pthread_join() error: %s", strerror(result));
		}
		else
		{
			if(retVal == PTHREAD_CANCELED)
				DebugMessage("pthread_join() thread was canceled");
			else
				DebugMessage("pthread_join() thread exited normally");
		}
	}


	// Destroy semaphore and attributes
	pthread_attr_destroy(&attr);
	sem_destroy(&server->Semaphore);

	pthread_exit(TCP_SERVER_SUCCESS);
}

void* TcpServer::ProcessClientRequest(void* _args)
{
	int result = 0;
	client_thread_args_t *args = reinterpret_cast<client_thread_args_t*>(_args);
	TcpServer* server = args->ptrServer;

	if(server != nullptr)
	{
		// Enable cancellation of thread
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		if(args->rqRoutine != nullptr)
		{
			result = args->rqRoutine(args->clientDesc, args->args);
		}

		if(result)
		{
			ErrorMessage("Processing of client request failure with code: %d", result);
		}

		// Disable cancellation of thread
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);

		// Unregister thread
		int mutexRes;
		mutexRes = pthread_mutex_lock(&server->PoolLock);
		if(mutexRes != 0)
			ErrorMessage("mutex_lock() PoolLock error: %s line: %d", strerror(mutexRes), __LINE__);

		int eraseNum = server->ThreadPool.erase(pthread_self());
		int threadsNum = server->ThreadPool.size();
		DebugMessage("Number of unregistred thread: %d of %d", eraseNum, threadsNum);

		mutexRes = pthread_mutex_unlock(&server->PoolLock);
		if(mutexRes != 0)
			ErrorMessage("mutex_unlock() PoolLock error: %s line: %d", strerror(mutexRes), __LINE__);

		if(sem_post(&server->Semaphore) == -1)
		{
			ErrorMessage("sem_post() error: %s", strerror(errno));
		}
	}


	pthread_exit(0);
}

