cmake_minimum_required(VERSION 2.8)
set(LIB_SRC
    AsyncLogging.cc
    Condition.cc
    CountDownLatch.cc
    FileAppender.cc
    FixBuffer.cc
    Log.cc
    LogFile.cc
    LogStream.cc
    MutexLock.cc
    Thread.cc
)

add_library(libserver_base ${LIB_SRC})
target_link_libraries(libserver_base pthread rt)

set_target_properties(libserver_base PROPERTIES OUTPUT_NAME "server_base")

add_subdirectory(tests)