#ifndef BILLING_FDDATA_H
#define BILLING_FDDATA_H

#include <unistd.h>
#include <vector>
#include <sys/epoll.h>
class Fd_Data{
public:
    Fd_Data(int f, int ef):fd(f), epfd(ef){}
    virtual ~Fd_Data() = default;
    virtual void read_handler() = 0;
    virtual void write_handler() {
        ssize_t len = this->cacheOutBuffer.size();
        ssize_t nWrite = write(fd, this->cacheOutBuffer.data(), len);
        if(nWrite==len){
            this->cacheOutBuffer.clear();
            struct epoll_event event{};
            event.data.ptr = this;
            event.events = EPOLLIN | EPOLLET;
            epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
        }else if(nWrite>0){
            this->cacheOutBuffer.erase(this->cacheOutBuffer.begin(), this->cacheOutBuffer.begin() + nWrite);
        }else {
            this->close_handler();
        }
    }
    virtual void close_handler() {
        close(fd);
        delete this;
    }
protected:
    int fd;
    int epfd;
    std::vector<char> cacheInBuffer;
    std::vector<char> cacheOutBuffer;
};
#endif
