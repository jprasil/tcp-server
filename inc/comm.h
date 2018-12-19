/*
 * comm_prot.h
 *
 *  Created on: Nov 17, 2018
 *      Author: root
 */

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

int ServerComm(int _socket, void* _comm);

int ClientComm(int _socket, char* _buff);

/*class Comm
{

using commfunc_t = ssize_t (*) (int _socket, void *_buffer, size_t _size, int _flags);


public:
	explicit Comm(commfunc_t _send, commfunc_t _recv);
//	int Send();
//	int Recv();


private:
	commfunc_t	Send;
	commfunc_t	Recv;
};
*/






#endif /* INC_COMM_H_ */
