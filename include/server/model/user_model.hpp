#pragma once

#include "user.hpp"

// user 表的操作类
class UserModel
{
private:

public:
    // 添加
    bool insert(User& user);
    
    // 查询
    User query(int id);

    // 更新状态
    bool uptateState(User user);

    // 重置用户状态
    void resetState();
};