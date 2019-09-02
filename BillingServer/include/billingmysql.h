#ifndef BILLING_MYSQL_H
#define BILLING_MYSQL_H

#include <memory>
#include <string>
#include <cstring>
#include "logger.h"
#include "mysql.h"

class Account{
public:
    Account() :id(0), point(0), is_online(0), is_lock(0) {};
    int id;
    char name[33];
    char password[33];
    char question[65];
    char answer[65];
    char email[65];
    char qq[17];
    int point;
    unsigned short is_online, is_lock;
};
class BillingMysql{
public:
    BillingMysql(std::shared_ptr<MYSQL> mysql);
    ~BillingMysql();
    unsigned char getLoginResult(std::string& username, std::string& password);
    unsigned char getRegResult(std::string& username, std::string& password, std::string& superPassword, std::string& email);
    unsigned int getUserPoint(std::string& username);
    void updateFieldStatus(std::string& username, std::string field, bool status);
private:
    std::shared_ptr<MYSQL> mysqlHandler;
    MYSQL * getMysql() {
        return this->mysqlHandler.get();
    };
    void getAccountInfo(std::string& username, Account& accountInfo);
};

#endif
