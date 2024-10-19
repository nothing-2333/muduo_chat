#pragma once
#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <functional>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace muduo;
using namespace muduo::net;

// 处理消息的回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr& connection, json& js, Timestamp time)>;

// 聊天服务器
class ChatService
{
private:
    ChatService();
    // 消息id以及对应的处理方法
    unordered_map<int , MsgHandler> _msgHandlerMap;

public:
    static ChatService* instance();
    void login(const TcpConnectionPtr& connection, json& js, Timestamp time);
    void reg(const TcpConnectionPtr& connection, json& js, Timestamp time);
    MsgHandler getHandler(int msgId);
};