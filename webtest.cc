#include "TcpServer.h"
#include "base/Log.h"


int main(){
    std::string path = "./web_log.txt";
    int ThreadNum = 4;
    int port = 8089;
    Log::setFilename(path);
    EventLoop mainloop;
    std::string ip = "127.0.0.1";
    TcpServer Server(&mainloop, ThreadNum, const_cast<char*>(ip.data()), port);
    Server.start();
    mainloop.loop();
    return 1;
}