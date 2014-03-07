#ifndef MY_FUNCTION_H
#define MY_FUNCTION_H

#include <WinSock2.h>
#include <iostream>

#define  BUFFER_SIZE		SO_MAX_MSG_SIZE	//缓冲区大小
#define  DEF_PORT			10000

// 创建一个监听套接字并把本地套接字地址(本地IP地址和本地端口)绑定到监听套接字
SOCKET BindListen(void);

// 接受客户端的连接请求
SOCKET AcceptConnection(SOCKET hListenSocket);

//接受客户端的数据并回显
bool ProcessConnection(SOCKET hClientSocket);

//关闭当前客户端连接
bool ShutdownConnection(SOCKET hClientSocket);

// 服务器主题函数
void DoWork(void);

//服务一个客户端连接的线程函数,参数lpParam就是客户端的套接字句柄
DWORD WINAPI ThreadProc(LPVOID lpParam);

#endif