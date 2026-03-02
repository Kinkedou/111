
#include "input_manager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define SERVER_PORT 8888

static int g_iSocketServer;

static int NetinputGetInputEvent(PInputEvent ptInputEvent)
{

	struct sockaddr_in tSocketClientAddr;
	char aRecvBuf[1000];
	unsigned iAddrLen = sizeof(struct sockaddr);
	int iRecvLen = recvfrom(g_iSocketServer, aRecvBuf, 999, 0, (struct sockaddr*)&tSocketClientAddr, &iAddrLen);
	if (iRecvLen > 0)
	{
		aRecvBuf[iRecvLen] = '\0';
		//printf("Get Msg From %s : %s\n", inet_ntoa(tSocketClientAddr.sin_addr), ucRecvBuf);
		ptInputEvent->iType = INPUT_TYPE_NET;
		gettimeofday(&ptInputEvent->tTime, NULL);
		strncpy(ptInputEvent->str, aRecvBuf, 1000);
		ptInputEvent->str[999] = '\0';
		return 0;
	}
	else
		return -1;
}

static int NetinputDeviceInit(void)
{
	
	struct sockaddr_in tSocketServerAddr;
	int iRet;


	g_iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);//AF_INET:IPv4; SOCK_DGRAM:UDP,0:default protocol
	if (-1 == g_iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family = AF_INET;//这是IPv4协议
	tSocketServerAddr.sin_port = htons(SERVER_PORT);  /* host to net, short */
	tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;//这是一个特殊的IP地址，表示本机的所有IP地址
	memset(tSocketServerAddr.sin_zero, 0, 8);//sin_zero的作用是填充，使得sockaddr_in结构体的大小与sockaddr结构体相同

	iRet = bind(g_iSocketServer, (const struct sockaddr*)&tSocketServerAddr, sizeof(struct sockaddr));
	//bind函数的作用是将一个套接字与一个本地地址绑定起来
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}
	return 0;
}

static int NetinputDeviceExit(void)
{
	close(g_iSocketServer);
	return 0;
}


InputDevice g_tNetinputDev = {
.name = "Netinput",
.GetInputEvent = NetinputGetInputEvent,
.DeviceInit = NetinputDeviceInit,
.DeviceExit = NetinputDeviceExit,
};

void NetInputRegister(void)
{
	RegisterInputDevice(&g_tNetinputDev);
}