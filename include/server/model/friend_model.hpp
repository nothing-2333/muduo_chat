#pragma once

#include <vector>

#include "user.hpp"

using namespace std;

// 维护好友信息
class FriendModel
{
private:

public:
    // 添加好友关系
    void insert(int userId, int friendId);

    // 返回用户好友列表
    vector<User> query(int userId);
};