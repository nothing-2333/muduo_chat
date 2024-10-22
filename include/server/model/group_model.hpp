#pragma once

#include <string>
#include <vector>

#include "group.hpp"

using namespace std;

class GroupModel
{
private:

public:
    // 创建群组
    bool createGroup(Group& group);

    // 加入群组
    void addGroup(int userId, int groupId, string role);

    // 查询用户所在群组信息
    vector<Group> queryGroups(int userId);

    // 查询特定群组成员信息
    vector<int> queryGroupUsers(int userId, int groupId);
};