#include <functional>
#include <muduo/base/Logging.h>
#include <vector>

#include "chat_service.hpp"
#include "public.hpp"

using namespace muduo;
using namespace std;

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
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});
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

// 登录
void ChatService::login(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int id = js["id"];
    string pwd = js["password"];

    User user = _userModel.query(id);
    if (user.getId() != -1)
    {
        if (user.getPwd() == pwd)
        {
            if (user.getState() != "online")
            {
                // 登陆成功，记录用户连接信息
                {
                    lock_guard<mutex> lock(_connMutex);
                    _userConnMap.insert({ id, connection });
                }

                // 正常登录，更新用户状态信息
                user.setState("online");
                _userModel.uptateState(user);

                json responce;
                responce["msgId"] = LOGIN_MSG_ACK;
                responce["errno"] = 0;
                responce["id"] = user.getId();
                responce["name"] = user.getName();

                // 查询用户是否有离线消息
                vector<string> vec = _offlineMsgModel.query(id);
                if (!vec.empty()) 
                {
                    responce["offlinemsg"] = vec;
                    _offlineMsgModel.remove(id);
                }

                // 查询用户的好友信息并返回
                vector<User> userVec = _friendModel.query(id);
                if (!userVec.empty())
                {
                    vector<string> vec;
                    for (User &user : userVec)
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        vec.push_back(js.dump());
                    }
                    responce["friends"] = vec;
                }

                connection->send(responce.dump());
            }
            else
            {
                // 用户已经登录，不允许重复登陆
                json responce;
                responce["msgId"] = LOGIN_MSG_ACK;
                responce["errno"] = 3;
                responce["errmsg"] = "该账号已经登录，不允许重复登陆";
                connection->send(responce.dump());
            }
        }
        else
        {
            json responce;
            responce["msgId"] = LOGIN_MSG_ACK;
            responce["errno"] = 2;
            responce["errmsg"] = "用户名或密码错误";
            connection->send(responce.dump());
        }
    }
    else
    {
        json responce;
        responce["msgId"] = LOGIN_MSG_ACK;
        responce["errno"] = 1;
        responce["errmsg"] = "用户不存在";
        connection->send(responce.dump());
    }
}

// 注册
void ChatService::reg(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json responce;
        responce["msgId"] = REG_MSG_ACK;
        responce["errno"] = 0;
        responce["id"] = user.getId();
        connection->send(responce.dump());
    }
    else
    {
        json responce;
        responce["msgId"] = REG_MSG_ACK;
        responce["errno"] = 1;
        responce["errmsg"] = "注册失败";
        connection->send(responce.dump());
    }
}

// 单聊
void ChatService::oneChat(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int toId = js["toId"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if (it != _userConnMap.end())
        {
            // 在线，转发消息
            it->second->send(js.dump());
            return ;
        }
    }
    // 不在线，储存消息
    _offlineMsgModel.insert(toId, js.dump());

}

// 添加好友
void ChatService::addFriend(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    int friendId = js["friendId"].get<int>();

    // 存储好友信息
    _friendModel.insert(userId, friendId);
    _friendModel.insert(friendId, userId);
}

// 创建群组
void ChatService::createGroup(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int userId = js["id"].get<int>();
    string name = js["groupName"];
    string desc = js["groupDesc"];

    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        _groupModel.addGroup(userId, group.getId(), "creator");
    }
}

// 加入群组
void ChatService::addGroup(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int userId = js["id"];
    int groupId = js["groupId"].get<int>();
    _groupModel.addGroup(userId, groupId, "normal");
}

// 群组聊天
void ChatService::groupChat(const TcpConnectionPtr& connection, json& js, Timestamp time)
{
    int userId = js["id"];
    int groupId = js["groupId"].get<int>();
    vector<int> userIdVec = _groupModel.queryGroupUsers(userId, groupId);

    lock_guard<mutex> lock(_connMutex);
    for (int id : userIdVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            // 存储离线消息
            _offlineMsgModel.insert(id, js.dump());
        }
    }
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& connection)
{
    User user;
    {
        // 查询连接列表，剔除用户
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == connection)
            {
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 更新信息
    user.setState("offline");
    _userModel.uptateState(user);
}

// 重置业务
void ChatService::reset()
{
    // 把 online 设置成 offline
    _userModel.resetState();
}