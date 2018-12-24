//---------------------------------------------------
/*
 	\file		comm.h

 	\brief		This module defines communication
 				messages based on TCP protocol

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil
*/
//---------------------------------------------------
#ifndef INC_COMM_H_
#define INC_COMM_H_

#include <cstdio>

#define MESSAGE_DATA_SIZE	50

#define BUFF_SIZE			100

enum Cmd
{
	CPU_UTILIZATION = 0,
	MEM_USAGE		= 1
};



/*struct Message
{
	Cmd ID;
	size_t			DataSize;
	char			Data[MESSAGE_DATA_SIZE];
};*/


//---------------------------------------------------
/*
	\brief	Function process client's requests

	\param	_socket	Server's socket descriptor
			_comm	Passed arguments
*/
//---------------------------------------------------
int ServerComm(int _socket, void* _comm);
//---------------------------------------------------
/*
	\brief	Function send request to server

	\param	_socket	Client's socket descriptor
			_comm	Passed pointer to buffer
*/
//---------------------------------------------------
int ClientComm(int _socket, char* _buff);



#endif /* INC_COMM_H_ */
