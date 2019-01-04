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



//---------------------------------------------------
/*
	\brief	Communication messages (based on byte streams)
	 	 	Client sends: [cmd]\n
	 	 			cmd.........command, may be "cpu" or "mem"
	 	 			\n..........identifier of end message

	 	 	Server responds: val unit\n
	 	 			val.........measured value
	 	 			unit........unit of value
	 	 			\n..........identifier of end message

*/
//@{
//---------------------------------------------------
/*
	\brief	Function processes client's requests

	\param	_socket	Server's socket descriptor
			_comm	Passed arguments
*/
//---------------------------------------------------
int ServerComm(int _socket, void* _comm);
//---------------------------------------------------
/*
	\brief	Function sends request to server

	\param	_socket	Client's socket descriptor
			_comm	Passed pointer to buffer
*/
//---------------------------------------------------
int ClientComm(int _socket, char* _buff);
//@}

//---------------------------------------------------
//! Todo
/*struct Message
{
	Cmd 			ID;
	size_t			DataSize;
	char			Data[MESSAGE_DATA_SIZE];
};
*/
#endif /* INC_COMM_H_ */
