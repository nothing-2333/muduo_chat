#pragma once

enum EnMsgType
{
    LOGIN_MSG = 1,      // 登录
    REG_MSG,            // 注册
    REG_MSG_ACK,        // 注册响应
    LOGIN_MSG_ACK,      // 登录响应
    ONE_CHAT_MSG,       // 单聊
    ADD_FRIEND_MSG,     // 添加好友
    CREATE_GROUP_MSG,   // 创建群组
    ADD_GROUP_MSG,      // 加入群组
    GROUP_CHAT_MSG,     // 群聊天
};


