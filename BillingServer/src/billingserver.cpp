#include "billingserver.h"
#include "requesthandler.h"
#include <utility>


BillingServer::BillingServer(std::string host, std::string port,
                             std::string dbhost, std::string dbport,
                             std::string dbuser, std::string dbpasswd,
                             std::string dbname) {
    this->host = std::move(host);
    this->port = std::move(port);
    this->dbHost = std::move(dbhost);
    this->dbPort = std::move(dbport);
    this->dbUser = std::move(dbuser);
    this->dbPassword = std::move(dbpasswd);
    this->dbName = std::move(dbname);
}

bool BillingServer::testConnect() {
    auto mysql = std::make_shared<MYSQL>();
    requestHandler = std::make_shared<RequestHandler>(*std::make_shared<BillingMysql>(mysql));

    MYSQL *mysqlPointer = mysql.get();
    mysql_init(mysqlPointer);
    const unsigned connectTimeOut = 3;
    mysql_options(mysqlPointer, MYSQL_OPT_CONNECT_TIMEOUT, &connectTimeOut);
    mysql_options(mysqlPointer, MYSQL_SET_CHARSET_NAME, "utf8");
    my_bool reconnect = 1;
    mysql_options(mysqlPointer, MYSQL_OPT_RECONNECT, &reconnect);
    if (!mysql_real_connect(mysqlPointer, this->dbHost.c_str(), this->dbUser.c_str(),
                            this->dbPassword.c_str(), this->dbName.c_str(),
                            std::atoi(this->dbPort.c_str()), nullptr, 0))
    {
        Logger::write(std::string("Connect to database Error: ") + mysql_error(mysqlPointer));
        return false;
    }
    Logger::write("connect to mysql server ok !");
    Logger::write(std::string("mysql version: ") + mysql_get_server_info(mysqlPointer));
    if (mysql_query(mysqlPointer, "SHOW COLUMNS FROM account") != 0)
    {
        Logger::write(std::string("Get account table info Error: ") + mysql_error(mysqlPointer));
        return false;
    }
    MYSQL_RES *res = mysql_store_result(mysqlPointer);
    if (!res) {
        Logger::write(std::string("Get account table info Error: ") + mysql_error(mysqlPointer));
        return false;
    }
    MYSQL_FIELD *fields = mysql_fetch_fields(res);
    unsigned int fieldsCount = mysql_num_fields(res), fieldIndex = 0, i;
    for (i = 0; i < fieldsCount; i++) {
        if (strcmp("Field", fields[i].name) == 0) {
            fieldIndex = i;
            break;
        }
    }
    bool hasExtraFields[] = { false,false };
    const char* extraFields[] = { "is_online","is_lock" };
    auto row = mysql_fetch_row(res);
    while (row) {
        for (i = 0; i < 2; i++) {
            if (strcmp(row[fieldIndex], extraFields[i]) == 0) {
                hasExtraFields[i] = true;
            }
        }
        row = mysql_fetch_row(res);
    }
    mysql_free_result(res);
    for (i = 0; i < 2; i++) {
        if (hasExtraFields[i]) {
            continue;
        }
        std::string sql = "ALTER TABLE `account` ADD COLUMN `";
        sql += extraFields[i];
        sql += "`  smallint(1) UNSIGNED NOT NULL DEFAULT 0";
        if (mysql_real_query(mysqlPointer, sql.c_str(), sql.length()) != 0) {
            Logger::write(std::string("add extra column ") + extraFields[i] + " failed: " + mysql_error(mysqlPointer));
            return false;
        }
    }
    return true;
}

bool BillingServer::registerFd(int epfd) {
    if(this->testConnect()){
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd==-1){
            std::stringstream ss;
            ss << "BillingServer::start #"<< __LINE__ <<","<<strerror(errno);
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
            ss << "BillingServer::start #"<< __LINE__ <<","<<strerror(errno)<< " host:"<<this->host<<" port:"<<this->port;
            Logger::write(ss.str());
            return false;
        }
        auto * fd_data = new Billing_Fd_Data(fd, epfd, 1, this->requestHandler);
        struct epoll_event event{};
        event.data.ptr = fd_data;
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
        return true;
    }
    return false;
}


Billing_Fd_Data::Billing_Fd_Data(int fd, int epfd, int type,
                                 std::shared_ptr<RequestHandler> requestHandler) : Fd_Data(fd, epfd) {
    this->type = type;
    this->requestHandler = std::move(requestHandler);
}

void Billing_Fd_Data::read_handler() {
    if(type){
        struct sockaddr_in addr;
        socklen_t socklen = sizeof(sockaddr);
        int client_fd = accept(fd, (struct sockaddr *)&addr, &socklen);
        if(fd==-1){
            std::stringstream ss;
            ss << "process #"<< __LINE__ <<","<<strerror(errno);
            Logger::write(ss.str());
            return;
        }else {
            std::stringstream ss;
            ss << "accept from "<< inet_ntoa(addr.sin_addr);
            Logger::write(ss.str());
        }
        fcntl(client_fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

        struct epoll_event event{};
        auto * fd_data = new Billing_Fd_Data(client_fd, epfd, 0, this->requestHandler);
        event.data.ptr = fd_data;
        event.events = EPOLLIN | EPOLLET;
        epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
    }else{
        int max_size = 1024;
        char buf[max_size];
        bzero(buf, max_size);
        int nRead = read(fd, buf, max_size);
        if(nRead > 0){
            this->cacheInBuffer.insert(this->cacheInBuffer.end(), buf, buf + nRead);
            BillingData requestData(this->cacheInBuffer);
            while (requestData.isDataValid()) {
                BillingData responseData;
                this->requestHandler->processRequest(requestData, responseData);
                if(responseData.isDataValid()){
                    auto resp = std::make_shared<std::vector<char>>();
                    responseData.packData(*resp);
                    this->cacheOutBuffer.insert(this->cacheOutBuffer.end(),
                                                    resp->begin(), resp->end());

                    struct epoll_event event{};
                    event.data.ptr = this;
                    event.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(this->epfd, EPOLL_CTL_MOD, this->fd, &event);
                }
                requestData = BillingData(this->cacheInBuffer);
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