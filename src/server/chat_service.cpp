#include <functional>
#include <muduo/base/Logging.h>

#include "chat_service.hpp"
#include "public.hpp"

using namespace muduo;

// 单例
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的Handler回调操作
ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}
// 获取处理器
MsgHandler ChatService::getHandler(int msgId)
{
    auto it = _msgHandlerMap.find(msgId);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr& connection, json& js, Timestamp time)
        {
            LOG_ERROR << "msgId: " << msgId << " 未找到...";
        };
    }
    return _msgHandlerMap[msgId];
}

void ChatService::login(const TcpConnectionPtr& connection, json& js, Timestamp time)
{

    LOG_INFO << "do login";
}



void ChatService::reg(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    LOG_INFO << "do reg";
}