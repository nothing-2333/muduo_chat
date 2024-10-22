#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "json.hpp"
#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

using namespace std;
using json = nlohmann::json;

// 记录当前系统用户信息
User g_currentUser;

// 记录当前用户的好友列表
vector<User> g_currentUserFriendList;

// 显示当前用户的群组列表
vector<Group> g_currentUserGroupList;

// 显示当前用户基本信息
void showCurrentUserData();

// 接受线程
void readTaskHandler(int clientfd);

// 获取系统信息
string getCurrentTime();

// 聊天页面
void mainMenu();

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 获取 ip 和 port
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建 client 的 socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "socket create error" << endl;
        exit(-1);
    }

    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (-1 == connect(clientfd, (sockaddr*)&server, sizeof(sockaddr_in)))
    {
        cerr << "connect server error" << endl;
        close(clientfd);
        exit(-1);
    }

    // 显示首页面
    for (;;)
    {  
        cout << "===================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "===================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;
        cin.get();      // 读走缓冲区残留的回车

        switch (choice)
        {
        case 1:
        {
            int id = 0;
            char pwd[50] = { 0 };

            cout << "userId:";
            cin >> id;
            cin.get();
            cout << "password:";
            cin.getline(pwd, 50);

            json js;
            js["msgId"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (-1 == len) cerr << "send login msg error:" << request << endl;
            else
            {
                char buffer[1024] = { 0 };
                len  = recv(clientfd, buffer, 1024, 0);
                if (-1 == len) cerr << "recv login response error" << endl;
                else
                {
                    json response = json::parse(buffer);
                    if (0 != response["errno"].get<int>()) 
                        cerr << "login error! message: " << response["errmsg"] << endl;
                    else 
                    {
                        cout << "login success, userId is " << response["id"] << endl;

                        // 记录当前用户
                        g_currentUser.setId(response["id"].get<int>());
                        g_currentUser.setName(response["name"]);

                        // 记录当前用户好友列表信息
                        if (response.contains("friends"))
                        {
                            vector<string> vec = response["friends"];
                            for (string& str : vec)
                            {
                                json js = json::parse(str);
                                User user;
                                user.setId(js["id"].get<int>());
                                user.setName(js["name"]);
                                user.setState(js["state"]);
                                g_currentUserFriendList.push_back(user);
                            }
                        }

                        // 记录群组列表信息
                        if (response.contains("groups"))
                        {
                            vector<string> vec1 = response["groups"];
                            for (string& groupStr : vec1)
                            {
                                json groupJson = json::parse(groupStr);
                                Group group;
                                group.setId(groupJson["id"].get<int>());
                                group.setName(groupJson["groupName"]);
                                group.setDesc(groupJson["groupDesc"]);

                                vector<string> vec2 = groupJson["users"];
                                for (string& userStr : vec2)
                                {
                                    GroupUser user;
                                    json js = json::parse(userStr);
                                    user.setId(js["id"].get<int>());
                                    user.setName(js["name"]);
                                    user.setState(js["state"]);
                                    user.setRole(js["role"]);
                                    group.getUsers().push_back(user);
                                }

                                g_currentUserGroupList.push_back(group);
                            }
                        }

                        showCurrentUserData();

                        if (response.contains("offlinemsg"))
                        {
                            vector<string> vec = response["offlinemsg"];
                            for (string& str : vec)
                            {
                                json js = json::parse(str);
                                cout << js["time"] << " [" << js["id"] << "]" << js["name"]
                                    << " said: " << js["msg"] << endl;
                            }
                        }

                        // 登录成功，启动接受线程接受数据
                        std::thread readTask(readTaskHandler, clientfd);
                        readTask.detach();

                        // 进入聊天页面
                        mainMenu();
                    }
                }
            }
            break;
        }
        case 2:
        {
            char name[50] = { 0 };
            char pwd[50] = { 0 };
            
            cout << "name:";
            cin.getline(name, 50);
            cout << "password:";
            cin.getline(pwd, 50);

            json js;
            js["msgId"] = REG_MSG;
            js["name"] = name;
            js["password"] = pwd;
            string request = js.dump();

            int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
            if (-1 == len) cerr << "send reg msg error:" << request << endl;
            else
            {
                char buffer[1024] = { 0 };
                len  = recv(clientfd, buffer, 1024, 0);
                if (-1 == len) cerr << "recv reg response error" << endl;
                else
                {
                    json response = json::parse(buffer);
                    if (0 != response["errno"].get<int>()) 
                        cerr << "register error! message: " << response["errmsg"] << endl;
                    else
                    {
                        // 注册成功
                        cout << "register success, userId is " << response["id"]
                            << ", do not forget it!" << endl;
                    }
                }
            }

            break;
        }
        case 3:
        {
            close(clientfd);
            exit(0);
        }
        default:
        {
            cerr << "invalid input!" << endl;
            break;
        }
        }
    }

    return 0;
}

void showCurrentUserData()
{
    cout << "===================login user===================" << endl;
    cout << "current login user => id:" << g_currentUser.getId() << " name:" << g_currentUser.getName() << endl;
    cout << "------------------friend list------------------" << endl;

    for (User& user : g_currentUserFriendList)
    {
        cout << user.getId() << " " << user.getName() << " " << user.getState() << endl;
    }

    cout << "------------------group list------------------" << endl;

    for (Group& group : g_currentUserGroupList)
    {
        cout << group.getId() << " " << group.getName() << " " << group.getDesc() << endl;
        for (GroupUser& user : group.getUsers())
        {
            cout << user.getId() << " " << user.getName() << " " << user.getState() << user.getRole() << endl;
        }
    }

    cout << "================================================" << endl;
}

void readTaskHandler(int clientfd)
{
    while ()
    {
        char buffer[1024] = { 0 };
        int len = recv(clientfd, buffer, 1024, 0);
        if (-1 == len || 0 == len)
        {
            close(clientfd);
            exit(-1);
        }

        // 接受 ChatServer 的数据，反序列化成 json 数据对象
        json js = json::parse(buffer);
        if (ONE_CHAT_MSG == js["msgId"].get<int>())
        {
            cout << js["time"].get<string>() << " [" << js["id"] << "]" << js["name"].get<string>()
                << " said: " << js["msg"].get<string>() << endl;

            continue;
        }
    }
    
}

string getCurrentTime()
{

}

void mainMenu()
{

}