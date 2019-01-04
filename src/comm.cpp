//---------------------------------------------------
/*
 	\file		comm.cpp

 	\brief		This module defines communication
 				messages based on TCP protocol

	\date		25.10.2018
	\version	1.0

	\author		Bc. Jan Prasil

	\note		Implements ServerComm() only
*/
//---------------------------------------------------

#include "comm.h"
#include "hw_monitor.h"
#include "help.h"

#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>


//---------------------------------------------------
//! Commands definition
const char *cmd[] = {"cpu", "mem"};

//---------------------------------------------------
/*
	\brief	Function processes client's requests

	\param	_socket	Server's socket descriptor
			_comm	Passed arguments
*/
//---------------------------------------------------
int ServerComm(int _socket, void* _comm)
{
	int len;
	char recvBuff[BUFF_SIZE];
	char sendBuff[BUFF_SIZE];
	std::fill_n(recvBuff, BUFF_SIZE, 0);
	std::fill_n(sendBuff, BUFF_SIZE, 0);
	int sendBuffLen = 0;

	// Store received data to buffer
	len = recv(_socket, (void*)&recvBuff, (BUFF_SIZE - 1), 0);

	if(len >= 0)
	{
		// Make string by adding 0 at end of received bytes
		recvBuff[len] = 0;
		char* pBuff = recvBuff;
		DebugMessage("Data received:%s  len:%d", recvBuff, len);
		// Attempt to find character which indicates end of message
		while((pBuff = strpbrk(pBuff, "\n")) != nullptr)
		{
			// Character is found
			*pBuff = 0;
			if((pBuff - 3) >= recvBuff)
			{
				// Make pointer to received command
				pBuff -= 3;
				// Find out a type of command and composite respond message
				if(strcmp(pBuff, &cmd[Cmd::CPU_UTILIZATION][0]) == 0)
				{
					auto cpuUtil = HwMonitor::GetInstance()->GetCpuUtilization();
					char tmpBuff[20];
					sprintf(tmpBuff, "%.2Lf", cpuUtil);
					strcat(tmpBuff, " [%]\n");
					size_t tmpLen = strlen(tmpBuff);
					if((tmpLen + sendBuffLen) <= (BUFF_SIZE - 1))
					{
						sendBuffLen += tmpLen;
						strcat(sendBuff, tmpBuff);
					}
				}
				else if(strcmp(pBuff, &cmd[Cmd::MEM_USAGE][0]) == 0)
				{
					auto memUsg = HwMonitor::GetInstance()->GetMemUsage();
					char tmpBuff[20];
					sprintf(tmpBuff, "%u", memUsg);
					strcat(tmpBuff, " [kB]\n");
					size_t tmpLen = strlen(tmpBuff);
					if((tmpLen + sendBuffLen) <= (BUFF_SIZE - 1))
					{
						sendBuffLen += tmpLen;
						strcat(sendBuff, tmpBuff);
					}

				}

				// Increment pointer to next message
				pBuff += 3;
			}

			// Find out if next message exist in buffer
			if(pBuff != &recvBuff[len-1])
			{
				pBuff++;
			}
			else
			{
				// No message
				break;
			}
		}
	}
	else
	{
		ErrorMessage("recv() error: %s", strerror(errno));
	}

	DebugMessage("Send response:%s len:%d", sendBuff, sendBuffLen);

	// Send response
	len = strlen(sendBuff);
	len = send(_socket, (void*)&sendBuff, sendBuffLen, 0);

	return 0;
}


