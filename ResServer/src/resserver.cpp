#include "resserver.h"

void *accept_handler(void *data, int epfd, int fd);
void *read_handler(void *data, int epfd, int fd);
void *write_handler(void *data, int epfd, int fd);
void *close_handler(void *data, int epfd, int fd);

ResServer::ResServer(std::string &host, std::string &port){
    this->host = host;
    this->port = port;
}

bool ResServer::registerFd(int epfd){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd==-1){
        std::stringstream ss;
        ss << "ResServer::start #"<< __LINE__ <<","<<strerror(errno);
        Logger::write(ss.str());
        return false;
    }
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    struct sockaddr_in addr{};
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_aton(this->host.c_str(), &(addr.sin_addr));
    addr.sin_port = htons(std::atoi(this->port.c_str()));

    if(bind(fd, (sockaddr *)&addr, sizeof(addr)) || listen(fd, 20)){
        std::stringstream ss;
        ss << "ResServer::start #"<< __LINE__ <<","<<strerror(errno)<< " host:"<<this->host<<" port:"<<this->port;
        Logger::write(ss.str());
        return false;
    }

    struct epoll_event ev;
    ev.data.ptr = new Res_Fd_Data(fd, epfd, 1);
    ev.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev)){
        std::stringstream ss;
        ss << "ResServer::start #"<< __LINE__ <<","<<strerror(errno)<< " when ctl added";
        Logger::write(ss.str());
        return false;
    }
    return true;
}

void Res_Fd_Data::read_handler() {
    if(type){
        struct sockaddr_in addr;
        socklen_t socklen = sizeof(sockaddr);
        int client_fd = accept(fd, (struct sockaddr *)&addr, &socklen);
        if(fd==-1){
            std::stringstream ss;
            ss << "process #"<< __LINE__ <<","<<strerror(errno);
            Logger::write(ss.str());
            return ;
        }else {
            std::stringstream ss;
            ss << "accept from "<< inet_ntoa(addr.sin_addr);
            Logger::write(ss.str());
        }
        fcntl(client_fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

        struct epoll_event event{};
        event.data.ptr = new Res_Fd_Data(client_fd, epfd, 0);
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
    }else{
        int max_size = 256;
        unsigned int buf[max_size];
        bzero(buf, max_size/4);
        int nRead = read(fd, buf, max_size);
        if(nRead > 0){
            if(nRead == 8){
                if(buf[0] == 0x876123){
                    uint32_t data[4] = {
                            0x90332219,
                            0x25874508,
                            0x90332219,
                            0x25874508
                    };
                    for(unsigned int & i : data){
                        i = buf[1]^i^0x12D82B;
                    }
                    this->cacheOutBuffer.insert(this->cacheOutBuffer.end(),
                                                (char*)data, ((char*)data) + 16);
                    struct epoll_event event{};
                    event.data.ptr = this;
                    event.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(this->epfd, EPOLL_CTL_MOD, this->fd, &event);
                }
            }
        }else if(nRead < 0){
            if (errno == EAGAIN){

            }else{
                this->close_handler();
            }
        }else {
            this->close_handler();
        }
    }
}

