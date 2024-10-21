# 跟着敲系列

## 一个练手的集群聊天服务器。

### mysql 数据库创建命令
CREATE DATABASE chat;
use chat

CREATE TABLE user (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(50) NOT NULL,
    state ENUM('online', 'offline') DEFAULT 'offline'
);

CREATE TABLE friend (
    userid INT NOT NULL,
    friendid INT NOT NULL,
    PRIMARY KEY (userid, friendid),
    FOREIGN KEY (userid) REFERENCES user(id) ON DELETE CASCADE,
    FOREIGN KEY (friendid) REFERENCES user(id) ON DELETE CASCADE,
    CHECK (userid <> friendid)
);

CREATE TABLE allgroup (
    id INT AUTO_INCREMENT PRIMARY KEY,
    groupname VARCHAR(50) NOT NULL UNIQUE,
    groupdesc VARCHAR(200) DEFAULT ''
);

CREATE TABLE groupuser (
    groupid INT NOT NULL,
    userid INT NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal',
    PRIMARY KEY (groupid, userid),
    FOREIGN KEY (groupid) REFERENCES allgroup(id) ON DELETE CASCADE,
    FOREIGN KEY (userid) REFERENCES user(id) ON DELETE CASCADE,
    CHECK (groupid <> userid)
);

CREATE TABLE offlinemessage (
    userid INT NOT NULL,
    message TEXT NOT NULL,
    FOREIGN KEY (userid) REFERENCES user(id) ON DELETE CASCADE
);

### mysql 数据库一些查看命令
select * from user;
desc user;
show tables;