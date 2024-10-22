#pragma once

#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "user_model.hpp"
#include "offline_message_model.hpp"
#include "friend_model.hpp"
#include "group_model.hpp"

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

    // 储存在线用户的通信连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    // 定义互斥锁，保证 _userConnMap 线程安全
    mutex _connMutex;

    // 数据操作对象
    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

public:
    static ChatService* instance();

    // 登录
    void login(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 注册
    void reg(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 一对一聊天
    void oneChat(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 添加好友
    void addFriend(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 创建群组
    void createGroup(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 加入群组
    void addGroup(const TcpConnectionPtr& connection, json& js, Timestamp time);

    // 群组聊天
    void groupChat(const TcpConnectionPtr& connection, json& js, Timestamp time);
    
    MsgHandler getHandler(int msgId);

    // 客户端异常退出
    void clientCloseException(const TcpConnectionPtr& connection);

    // 重置业务
    void reset();
};