#ifndef BILLING_RESSERVER_H
#define BILLING_RESSERVER_H

#include <string>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include <unistd.h>
#include <memory>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "logger.h"
#include "fddata.h"

class Res_Fd_Data : Fd_Data{
public:
    Res_Fd_Data(int f, int ef, int t):Fd_Data(f, ef), type(t){}
    ~Res_Fd_Data() override = default;
    void read_handler() override;
private:
    int type;
};
class ResServer{
public:
    ResServer(std::string &host, std::string &port);
    bool registerFd(int epfd);
private:
    std::string host;
    std::string port;
};

#endif //BILLING_RESSERVER_H
