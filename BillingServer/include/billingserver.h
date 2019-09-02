#ifndef BILLING_BILLINGSERVER_H
#define BILLING_BILLINGSERVER_H

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

class RequestHandler;
class Billing_Fd_Data : public Fd_Data{
public:
    Billing_Fd_Data(int fd1, int epfd1, int type,
                    std::shared_ptr<RequestHandler> requestHandler);
    ~Billing_Fd_Data() override = default;
    void read_handler() override;
private:
    int type;
    std::shared_ptr<RequestHandler> requestHandler;
};

class BillingServer {
public:
    BillingServer(std::string, std::string, std::string, std::string, std::string, std::string, std::string );
    bool registerFd(int epfd);
private:
    bool testConnect();
    std::shared_ptr<RequestHandler> requestHandler;
    std::string host;
    std::string port;
    std::string dbHost;
    std::string dbPort;
    std::string dbUser;
    std::string dbPassword;
    std::string dbName;
};


#endif
