#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

/* socket
 * connect
 * send/recv
 */

#define SERVER_PORT 8888

int main(int argc, char **argv)
{
	int iSocketClient;
	struct sockaddr_in tSocketServerAddr;
	
	int iRet;
	int iSendLen;
	int iAddrLen;

	if (argc != 3)
	{
		printf("Usage:\n");
		printf("%s <server_ip> <str>\n", argv[0]);
		return -1;
	}

	iSocketClient = socket(AF_INET, SOCK_DGRAM, 0);//AF_INET是IPv4，SOCK_DGRAM是UDP

	tSocketServerAddr.sin_family      = AF_INET;
	tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
 	//tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	if (0 == inet_aton(argv[1], &tSocketServerAddr.sin_addr))//inet_aton把ip地址字符串转换为网络字节序的二进制形式
 	{
		printf("invalid server_ip\n");
		return -1;
	}
	memset(tSocketServerAddr.sin_zero, 0, 8);//这是为了让sockaddr_in结构体的大小和sockaddr结构体的大小相同而设置的填充字节

#if 0
	iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));	
	if (-1 == iRet)
	{
		printf("connect error!\n");
		return -1;
	}
#endif
			iAddrLen = sizeof(struct sockaddr);
			iSendLen = sendto(iSocketClient, argv[2], strlen(argv[2]), 0,
			                      (const struct sockaddr *)&tSocketServerAddr, iAddrLen);


				close(iSocketClient);

	return 0;
}

