#include "socket.h"

namespace Socket {

#ifdef WIN32
    typedef int SOCKLEN;
#elif LINUX
    typedef socklen_t SOCKLEN;
#endif

    // 开始网络
    int startup() {
        int ret = 0;
#ifdef WIN32
        struct WSAData wsa;
        WORD sockVersion = MAKEWORD(2, 2);
        ret = (int)WSAStartup(sockVersion, &wsa);
#elif LINUX
        signal(SIGPIPE, SIG_IGN);
#endif
        return ret;
    }

    // 结束网络
    int cleanup() {
        int ret = 0;
#ifdef WIN32
        ret = (int)WSACleanup();
#endif
        return ret;
    }

    // 初始化套接字
    SOCKET createSocket(int family, int type, int protocol) {
        return (SOCKET)socket(family, type, protocol);
    }

    // 关闭套接字
    int clostSocket(SOCKET sock) {
        int ret = 0;
        if (sock < 0) return 0;
#ifdef WIN32
        ret = closesocket(sock);
#elif LINUX
        ret = close(sock);
#endif
        return ret;
    }

    // 连接目标地址
    int connect(SOCKET sock, const struct sockaddr *addr) {
        SOCKLEN len = sizeof(struct sockaddr);
        return ::connect(sock, addr, len);
    }

    // 停止套接字
    int shutdown(SOCKET sock, int mode) {
        return ::shutdown(sock, mode);
    }

    // 绑定端口
    int bind(SOCKET sock, const struct sockaddr *addr) {
        SOCKLEN len = sizeof(struct sockaddr);
        return ::bind(sock, addr, len);
    }

    // 监听消息
    int listen(SOCKET sock, int count) {
        return ::listen(sock, count);
    }

    // 接收连接
    SOCKET accept(SOCKET sock, struct sockaddr *addr) {
        SOCKLEN len = sizeof(struct sockaddr);
        return ::accept(sock, addr, &len);
    }

    // 设置套接字参数
    int setSockopt(SOCKET sock, int level, int optname, const char *optval, int optlen) {
        SOCKLEN len = optlen;
        return setsockopt(sock, level, optname, optval, len);
    }

    // 调用ioctlsocket，设置输出输入参数
    int ioctl(SOCKET sock, long cmd, unsigned long *argp) {
        int ret;
#ifdef WIN32
        ret = ioctlsocket((SOCKET)sock, cmd, argp);
#elif LINUX
        ret = ioctl(sock, cmd, argp);
#endif
        return ret;
    }

    // 取得套接字所连接地址
    int peerName(SOCKET sock, struct sockaddr_in *addr) {
        SOCKLEN len = sizeof(struct sockaddr);
        return getpeername(sock, (sockaddr*)addr, &len);
    }

    ulong getHostByName(const char* host) {
        ulong addr = 0x00;
        struct addrinfo *info = NULL;
        struct addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        uint32 ret = getaddrinfo(host, NULL, &hints, &info);
        if (ret != 0)
            return INADDR_NONE;

        if (info != NULL) {
            addr = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr;
        }

        freeaddrinfo(info);
        return addr;
    }

    // 发送消息
    int send(SOCKET sock, const void *buf, int size, int mode) {
        return ::send(sock, (char*)buf, size, mode);
    }

    // 接收消息
    int recv(SOCKET sock, void *buf, int size, int mode) {
        return ::recv(sock, (char*)buf, size, mode);
    }
}
