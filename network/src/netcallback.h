#ifndef _NET_CALLBACK_H_
#define _NET_CALLBACK_H_

#include "define.h"
#include "socket.h"

class Network;

class NetCallback
{
public:
    /*
    单有连接accept时则回调该函数
    @netid			该连接的netid
    @ip			远端地址，主机序
    @port			远端端口号
    */
    virtual void onAccept(Port listen_port, NetID netid, Host host, Port port) = 0;

    /*
    当收到网络消息时回调该函数
    @netid			网络消息来源的netid
    @data			网络数据
    @length		数据长度
    */
    virtual void onRecv(NetID netid, const char *data, int length) = 0;

    /*
    当有网络断开的时候回调该函数
    @netid			断开的网络连接的netid
    */
    virtual void onDisconnect(NetID netid) = 0;

    /*
    当调用异步ConnectAsyn结果返回后回调
    @result		连接结果是否成功
    @handle		ConnectAsyn时的输出参数handle
    @netid			连接成功时，netid时所得连接的网络层id
    @ip			连接ip
    @port			连接port
    */
    virtual void onConnect(bool result, uint32 handle, NetID netid, Host host, Port port) = 0;
};

#endif