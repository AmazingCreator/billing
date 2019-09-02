#include <sstream>
#include "billingmysql.h"

BillingMysql::BillingMysql(std::shared_ptr<MYSQL> mysql) : mysqlHandler(mysql)
{
}

BillingMysql::~BillingMysql()
{
    mysql_close(this->getMysql());
}

unsigned char BillingMysql::getLoginResult(std::string & username, std::string & password)
{
    Account accountInfo;
    this->getAccountInfo(username, accountInfo);
    if (accountInfo.id == 0) {
        return 3;
    }
    if (strcmp(accountInfo.password, password.c_str()) != 0) {
        return 3;
    }
    if (accountInfo.is_lock != 0) {
        return 7;
    }
    if (accountInfo.is_online != 0) {
        return 4;
    }
    return 1;
}

unsigned char BillingMysql::getRegResult(std::string & username, std::string & password, std::string & superPassword, std::string & email)
{
    unsigned char regError = 4;
    Account accountInfo;
    this->getAccountInfo(username, accountInfo);
    if (accountInfo.id != 0) {
        return regError;
    }
    auto mysql = this->getMysql();
    auto stmt = mysql_stmt_init(mysql);
    if (!stmt) {
        return regError;
    }
    const char* sql = "INSERT INTO account (name, password, question, email) VALUES (?, ?, ?, ?)";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
        Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
        return regError;
    }
    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));
    unsigned char offset=0;
    unsigned long nameSize = username.length();
    bind[offset].buffer_type = MYSQL_TYPE_STRING;
    bind[offset].buffer = (void*)username.c_str();
    bind[offset].buffer_length = nameSize + 1;
    bind[offset].is_null = nullptr;
    bind[offset].length = &nameSize;
    offset++;
    unsigned long passwordSize = password.length();
    bind[offset].buffer_type = MYSQL_TYPE_STRING;
    bind[offset].buffer = (void*)password.c_str();
    bind[offset].buffer_length = passwordSize + 1;
    bind[offset].is_null = nullptr;
    bind[offset].length = &passwordSize;
    offset++;
    unsigned long questionSize = superPassword.length();
    bind[offset].buffer_type = MYSQL_TYPE_STRING;
    bind[offset].buffer = (void*)superPassword.c_str();
    bind[offset].buffer_length = questionSize + 1;
    bind[offset].is_null = nullptr;
    bind[offset].length = &questionSize;
    offset++;
    unsigned long emailSize = email.length();
    bind[offset].buffer_type = MYSQL_TYPE_STRING;
    bind[offset].buffer = (void*)email.c_str();
    bind[offset].buffer_length = emailSize + 1;
    bind[offset].is_null = nullptr;
    bind[offset].length = &emailSize;
    if (mysql_stmt_bind_param(stmt, bind)) {
        Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
        return regError;
    }
    if (mysql_stmt_execute(stmt))
    {
        Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
        return regError;
    }
    mysql_stmt_close(stmt);
    return 1;
}


unsigned int BillingMysql::getUserPoint(std::string & username)
{
    Account accountInfo;
    this->getAccountInfo(username, accountInfo);
    if (accountInfo.id != 0) {
        return accountInfo.point;
    }
    return 0;
}

void BillingMysql::getAccountInfo(std::string & username, Account& accountInfo)
{
    auto mysql = this->getMysql();
    auto stmt = mysql_stmt_init(mysql);
    if (!stmt) {
        return;
    }
    const char* sql = "SELECT id, name, password"
                      ",question, answer, email, qq, point, is_online, is_lock"
                      " FROM account WHERE name=?";
    if (mysql_stmt_prepare(stmt, sql, strlen(sql)) != 0) {
        Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    unsigned long nameSize = username.length();
    MYSQL_BIND bind[1];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (void*)username.c_str();
    bind[0].buffer_length = nameSize + 1;
    bind[0].is_null = nullptr;
    bind[0].length = &nameSize;
    if (mysql_stmt_bind_param(stmt, bind)) {
        Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    auto prepare_meta_result = mysql_stmt_result_metadata(stmt);
    if (!prepare_meta_result)
    {
        Logger::write("mysql_stmt_result_metadata() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    if (mysql_stmt_execute(stmt))
    {
        Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    MYSQL_BIND    bind_fetch[10];
    memset(bind_fetch, 0, sizeof(bind_fetch));
    unsigned long length[10];
    my_bool       is_null[10];
    my_bool       error[10];
    int i = 0;
    bind_fetch[i].buffer_type = MYSQL_TYPE_LONG;
    bind_fetch[i].buffer = &accountInfo.id;
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.name;
    bind_fetch[i].buffer_length = sizeof(accountInfo.name);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.password;
    bind_fetch[i].buffer_length = sizeof(accountInfo.password);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.question;
    bind_fetch[i].buffer_length = sizeof(accountInfo.question);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.answer;
    bind_fetch[i].buffer_length = sizeof(accountInfo.answer);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.email;
    bind_fetch[i].buffer_length = sizeof(accountInfo.email);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_STRING;
    bind_fetch[i].buffer = accountInfo.qq;
    bind_fetch[i].buffer_length = sizeof(accountInfo.qq);
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_LONG;
    bind_fetch[i].buffer = &accountInfo.point;
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_SHORT;
    bind_fetch[i].buffer = &accountInfo.is_online;
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    i++;
    bind_fetch[i].buffer_type = MYSQL_TYPE_SHORT;
    bind_fetch[i].buffer = &accountInfo.is_lock;
    bind_fetch[i].is_null = &is_null[i];
    bind_fetch[i].length = &length[i];
    bind_fetch[i].error = &error[i];
    if (mysql_stmt_bind_result(stmt, bind_fetch))
    {
        Logger::write("mysql_stmt_bind_result() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    if (mysql_stmt_store_result(stmt) != 0) {
        Logger::write("mysql_stmt_store_result() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    int result = mysql_stmt_fetch(stmt);
	if ((result != 0) && (result != MYSQL_NO_DATA)) {
		Logger::write("mysql_stmt_fetch() failed");
		Logger::write(std::to_string(result));
	}
    if (mysql_stmt_free_result(stmt)) {
        Logger::write("mysql_stmt_free_result() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    mysql_stmt_close(stmt);
}

void BillingMysql::updateFieldStatus(std::string & username, std::string field, bool status)
{
    unsigned short val = status ? 1 : 0;
    auto mysql = this->getMysql();
    auto stmt = mysql_stmt_init(mysql);
    if (!stmt) {
        return;
    }
    std::stringstream ss;
    ss << "UPDATE account SET " << field << "=?  WHERE name=?";
    std::string sql = ss.str();
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0) {
        Logger::write("mysql_stmt_prepare() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_SHORT;
    bind[0].buffer = &val;
    bind[0].is_null = nullptr;
    bind[0].length = nullptr;
    unsigned long nameSize = username.length();
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (void*)username.c_str();
    bind[1].buffer_length = nameSize + 1;
    bind[1].is_null = nullptr;
    bind[1].length = &nameSize;
    if (mysql_stmt_bind_param(stmt, bind)) {
        Logger::write("mysql_stmt_bind_param() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    if (mysql_stmt_execute(stmt))
    {
        Logger::write("mysql_stmt_execute() failed");
		Logger::write(mysql_stmt_error(stmt));
        return;
    }
    mysql_stmt_close(stmt);
}