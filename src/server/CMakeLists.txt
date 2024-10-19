# 定义 SRC_LIST 变量包含该目录下所有源文件
aux_source_directory(. SRC_LIST)

# 生成可执行文件
add_executable(ChatServer ${SRC_LIST})

# 指定可执行文件链接时需要的库文件
target_link_libraries(ChatServer muduo_net muduo_base pthread)