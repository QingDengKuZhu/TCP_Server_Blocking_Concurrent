#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

#include <stdio.h>
#include <WinSock2.h>

#define		BUFFER_SIZE		SO_MAX_MSG_SIZE
#define		DEF_PORT			10000

#pragma comment(lib, "ws2_32.lib")

// 创建一个监听套接字并把本地套接字地址(本地IP地址和本地端口)绑定到监听套接字                                                                     */
SOCKET BindListen(void);

// 接受客户端的连接请求
SOCKET AcceptConnection(SOCKET hListenSocket);

// 接受客户端的数据并回显
bool ProcessConnection(SOCKET hClientSocket);

// 关闭当前客户端连接
bool ShutdownConnection(SOCKET hClientSocket);

// 服务器主题函数
void DoWork(void);

//服务一个客户端连接的线程函数,参数lpParam就是客户端的套接字句柄
DWORD WINAPI ThreadProc(LPVOID lpParam);

//完整发送数据
//s: 已连接套接字
//data: 数据起始地址
//数据大小
//若成功发送,返回true;否者返回false.
bool CompleteSend(SOCKET s, const char *data, int len);

//完整接收数据
//s : 已连接套接字
//buffer : 数据起始地址
//len :"数据大小
//若成功接收,返回true;否者返回false.
bool CompleteRecv(SOCKET s, char *buffer, int len);



#endif