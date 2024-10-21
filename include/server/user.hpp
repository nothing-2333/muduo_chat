#pragma once

#include <string>

using namespace std;

// user 表的定义类
class User
{
private:
    int id;
    string name;
    string password;
    string state;
public:
    User(int id=-1, string name="", string pwd="", string state="offline")
    : id(id), name(name), password(pwd), state(state)
    {};

    void setId(int id) { this->id = id; }
    void setName(string name) { this->name = name; }
    void setPwd(string password) { this->password = password; }
    void setState(string state) { this->state = state; }

    int getId() { return this->id; }
    string getName() { return this->name; }
    string getPwd() { return this->password; }
    string getState() { return this->state; }

};

