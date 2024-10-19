#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
private:
    TcpServer _server;
    EventLoop* _loop;

    void onConnection(const TcpConnectionPtr& connection);
    void onMessage(const TcpConnectionPtr& connection, Buffer* buffer, Timestamp time);
public:
    ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg);
    void start();
};



