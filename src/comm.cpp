/*
 * comm.cpp
 *
 *  Created on: Nov 18, 2018
 *      Author: root
 */

#include "comm.h"
#include "hw_monitor.h"
#include "help.h"

#include <sys/socket.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
//#include <string>

/*Comm::Comm(commfunc_t _send, commfunc_t _recv):
		Send(_send),
		Recv(_recv)
{

}
*/

const char *cmd[] = {"cpu", "mem"};

int ServerComm(int _socket, void* _comm)
{
//	Comm *comm = reinterpret_cast<Comm*>(_comm);
	int len;
	char recvBuff[BUFF_SIZE+1];
	char sendBuff[BUFF_SIZE+1];
	int sendBuffLen = 0;

	len = recv(_socket, (void*)&recvBuff, BUFF_SIZE, 0);
//	strcpy(recvBuff,"\nasdfr fgt mem\ndfde cpu cpucpu\n");
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
					if((tmpLen + sendBuffLen) <= BUFF_SIZE)
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
					if((tmpLen + sendBuffLen) <= BUFF_SIZE)
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
