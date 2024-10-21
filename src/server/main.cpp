#include <iostream>
#include <signal.h>

#include "char_server.hpp"
#include "chat_service.hpp"

using namespace std;

// 处理服务器 ctrl + c 结束后，重置 user 信息
void resetHandler(int )
{
    ChatService::instance()->reset();
    exit(0);
}

int main()
{
    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();
    
    return 0;
}