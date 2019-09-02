#include <fddata.h>
#include "ini.h"
#include "billingserver.h"
#include "resserver.h"

int main(int argc, char**argv){

    printf("started\n");

    Ini ini("./billing.ini");

    BillingServer server(ini.ReadString("Billing","host"),
                         ini.ReadString("Billing","port"),
                         ini.ReadString("Billing","dbHost"),
                         ini.ReadString("Billing","dbPort"),
                         ini.ReadString("Billing","dbUser"),
                         ini.ReadString("Billing","dbPassword"),
                         ini.ReadString("Billing","dbName"));

    ResServer resServer(ini.ReadString("ResServer","host"),
                        ini.ReadString("ResServer","port"));

    struct epoll_event events[20];
    int epfd = epoll_create(256);

    if(!server.registerFd(epfd)){
        return 0;
    }
    if(!resServer.registerFd(epfd)){
        return 0;
    }

    for(;;){
        int nfds = epoll_wait(epfd, events, 20, -1);
        for(int i = 0; i < nfds; ++i) {
            auto *fd_data = static_cast<Fd_Data *>(events[i].data.ptr);
            if(events[i].events & EPOLLIN){
                fd_data->read_handler();
            }else if(events[i].events & EPOLLOUT){
                fd_data->write_handler();
            }
        }
        if(nfds==-1){
            printf("close epfd");
            return 0;
        }
    }
}