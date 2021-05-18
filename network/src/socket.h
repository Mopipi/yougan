#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "define.h"

#ifdef WIN32
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define PI_SD_BOTH			SD_BOTH

#elif LINUX
typedef int SOCKET
#define INVALID_SOCKET (int)(~0)
#define SOCKET_ERROR (-1)
#endif

typedef uint16 Port;
typedef uint32 Host;
typedef uint32 NetID;

namespace Socket {
    // 开始网络
    int startup();
    // 结束网络
    int cleanup();

    // 初始化套接字
    SOCKET createSocket(int family, int type, int protocol);

    // 关闭套接字
    int clostSocket(SOCKET sock);

    // 连接目标地址
    int connect(SOCKET sock, const struct sockaddr *addr);

    // 停止套接字
    int shutdown(SOCKET sock, int mode);

    // 绑定端口
    int bind(SOCKET sock, const struct sockaddr *addr);

    // 监听消息
    int listen(SOCKET sock, int count);

    // 接收连接
    SOCKET accept(SOCKET sock, struct sockaddr *addr);

    // 设置套接字参数
    int setSockopt(SOCKET sock, int level, int optname, const char *optval, int optlen);

    // 调用ioctlsocket，设置输出输入参数
    int ioctl(SOCKET sock, long cmd, unsigned long *argp);

    // 取得套接字所连接地址
    int peerName(SOCKET sock, struct sockaddr_in *addr);

    // 获取主机地址
    ulong getHostByName(const char* host);

    // 发送消息
    int send(SOCKET sock, const void *buf, int size, int mode);

    // 接收消息
    int recv(SOCKET sock, void *buf, int size, int mode);
}

#endif