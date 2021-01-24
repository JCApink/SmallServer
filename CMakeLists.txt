cmake_minimum_required(VERSION 2.8)
Project(MyWebServer)
set(SRCS
    Channel.cc
    Timer.cc
    EventLoop.cc
    EventThreadLoop.cc
    EventThreadLoopPool.cc
    HttpData.cc
    webtest.cc
    TcpServer.cc
    #ThreadPool.cpp
    Util.cc
    Epoll.cc
)
include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(MyWebServer ${SRCS})
target_link_libraries(MyWebServer libserver_base)


add_subdirectory(base)
