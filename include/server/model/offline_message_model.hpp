#pragma once

#include <string>
#include <vector>

using namespace std;

class OfflineMsgModel
{
private:
     
public:
    // 存储用户离线消息
    void insert(int userId, string msg);

    // 删除用户离线消息
    void remove(int userId);

    // 查询用户离线消息
    vector<string> query(int userId);
};