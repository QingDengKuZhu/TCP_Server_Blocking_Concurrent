#include <WinSock2.h>
#include <iostream>
#include "my_function.h"

using std::endl;
using std::cout;

SOCKET BindListen(void)
{
	SOCKET hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == hsock)
	{
		cout << "socket error : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}


	// 填充本地套接字地址
	struct sockaddr_in serv_addr = {0};	/*本地套接字地址*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEF_PORT);
	if(SOCKET_ERROR == bind(hsock, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr)))
	{
		cout << "bind error : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}

	// 开始监听(将其设置为监听状态)
	if (SOCKET_ERROR == listen(hsock, SOMAXCONN))
	{
		cout << "listen error : " << WSAGetLastError() << endl;
		closesocket(hsock);
		return INVALID_SOCKET;
	}

	return hsock;
}

SOCKET AcceptConnection(SOCKET hListenSocket)
{
	struct sockaddr_in clien_addr = {0};
	int addr_len = sizeof(struct sockaddr_in);

	SOCKET hsock  = accept(hListenSocket, (struct sockaddr *)&clien_addr, &addr_len);
	if (INVALID_SOCKET == hsock)
	{
		cout << "accept error : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}

	return hsock;
}

bool ProcessConnection(SOCKET hClientSocket)
{
	char buf[BUFFER_SIZE];
	int result= 0;

	/*循环直到客户端关闭数据连接*/
	do 
	{
		// 接搜数据端的数据.由于套接字hClientSock(默认)是阻塞模式,对其调用recv将会阻
		// 塞,直到recv完成返回.当recv返回0时,表明客户端完成数据发送并且关闭了连接,
		//此时就可以退出循环
		result = recv(hClientSocket, buf, BUFFER_SIZE, 0);
		if (SOCKET_ERROR == result)
		{
			cout << "recv error : " << WSAGetLastError() << endl;
			return false;
		}

		else
		{	
			//把数据原封不动发送回客户端,即回显
			return	CompleteSend(hClientSocket, buf, result);	
		}
	} while (0 != result);

	return true;
}

bool ShutdownConnection(SOCKET hClientSocket)
{
	char buff[BUFFER_SIZE];
	int result = 0;

	if (SOCKET_ERROR == shutdown(hClientSocket,SD_SEND))
	{
		cout << "shutdown error : " << WSAGetLastError() << endl;	
		return false;
	}

	do 
	{
		result = recv(hClientSocket, buff, sizeof(buff)/sizeof(char), 0);
		if (SOCKET_ERROR == result)
		{
			cout << "recv error : " << WSAGetLastError() << endl;
			return false;
		}
		else
		{
			cout << result << " unexpected bytes received." << endl;
		}
	} while (0 != result);

	/*关闭套接字*/
	if (SOCKET_ERROR == closesocket(hClientSocket))
	{
		cout << "closesocket error : " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	SOCKET sd = (SOCKET) lpParam;

	//第二阶段, 服务一个客户端连接
	if (false== ProcessConnection(sd))
	{
		return -1;
	}
	cout << "成功服务一个客户端连接" << endl;

	//第三阶段,关闭一个客户端连接
	if (false == ShutdownConnection(sd))
	{
		return -1;
	}

	return 0;
}

void DoWork(void)
{
	/*获取监听套接字并进入监听状态*/
	SOCKET hListenSocket = BindListen();

	if (INVALID_ATOM == hListenSocket)
	{
		return;
	}
	
	cout << "Server is running..." << endl;
	
	//服务器循环
	while (true)
	{	
		/*第一个阶段,接受一个客户端连接*/
		SOCKET hClientSocket = AcceptConnection(hListenSocket);
		if (INVALID_SOCKET == hClientSocket)
		{
			break;
		}
	
		/*创建一个新的线程来服务刚刚接受的客户端连接*/
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(0, 0, ThreadProc, (LPVOID)hClientSocket, 0, &dwThreadId);
		CloseHandle(hThread);
	
	}

	/*关闭监听套接字*/
	if(SOCKET_ERROR == closesocket(hListenSocket))
	{
		cout << "closesocket error : " << WSAGetLastError() << endl;
	}

	return;
}

bool CompleteSend(SOCKET s, const char *data, int len)
{
	int idex = 0;
	char *p = (char *)data;

	while (idex < len)
	{
		int nTemp = send(s, p+idex, len-idex, 0);
		if (nTemp > 0)
		{
			idex += nTemp;
		}
		else if (nTemp == SOCKET_ERROR)
		{
			cout << "send error : " << WSAGetLastError() << endl;
		}
		else	//nTemp==0表示连接关闭
		{
			cout << "Connection closed unexpectedly by peer." << endl;
			return true;
		}
	}

	return true;	//当传入的len为0时,将执行此语句
}

bool CompleteRecv(SOCKET s, char *buffer, int len)
{
	int idex = 0;
	while (idex < len)
	{
		int nTemp = recv(s, buffer, len-idex, 0);
		if (nTemp == SOCKET_ERROR)
		{
			cout << "recv error : " <<WSAGetLastError() << endl;

			return false;
		}
		else if (nTemp > 0)
		{
			idex += nTemp;
		}
		else
		{
			cout << "Connection closed unexpectedly by peer." << endl;

			return true;
		}
	}
	//当传入的len为0时,将执行此语句

	return true;
}

