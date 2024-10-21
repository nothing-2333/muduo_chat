#include <functional>

#include "json.hpp"
#include "char_server.hpp"
#include "chat_service.hpp"

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        _server.setThreadNum(4);
    }

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& connection)
{
    // 断开连接
    if (!connection->connected())
    {
        ChatService::instance()->clientCloseException(connection);
        connection->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr& connection, Buffer* buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);

    // 获取处理器
    auto handler = ChatService::instance()->getHandler(js["msgId"].get<int>());
    handler(connection, js, time);
}