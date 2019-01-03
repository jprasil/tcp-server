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



const char *cmd[] = {"cpu", "mem"};

int ServerComm(int _socket, void* _comm)
{
	int len;
	char recvBuff[BUFF_SIZE];
	char sendBuff[BUFF_SIZE];
	int sendBuffLen = 0;

	len = recv(_socket, (void*)&recvBuff, (BUFF_SIZE - 1), 0);
	len = strlen(recvBuff);

	if(len >= 0)
	{
		recvBuff[len] = 0;
		char* pBuff = recvBuff;
		DebugMessage("Data received:%s  len:%d", recvBuff, len);
		while((pBuff = strpbrk(pBuff, "\n")) != nullptr)
		{
			*pBuff = 0;
			if((pBuff - 3) >= recvBuff)
			{
				pBuff -= 3;

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

				pBuff += 3;
			}

			if(pBuff != &recvBuff[len-1])
			{
				pBuff++;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		ErrorMessage("recv() error: %s", strerror(errno));
	}

	DebugMessage("Send response:%s len:%d", sendBuff, sendBuffLen);

	len = strlen(sendBuff);
	len = send(_socket, (void*)&sendBuff, sendBuffLen, 0);

	return 0;
}


