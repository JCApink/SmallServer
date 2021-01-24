#include "TcpServer.h"
#include "base/Log.h"


int main(){
    std::string path = "./web_log.txt";
    int ThreadNum = 4;
    int port = 8089;
    Log::setFilename(path);
    EventLoop mainloop;
    TcpServer Server(&mainloop, ThreadNum, port);
    Server.start();
    mainloop.loop();
    return 1;
}