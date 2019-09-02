#include "requesthandler.h"


void RequestHandler::processRequest(BillingData &requestData, BillingData &responseData) {
    unsigned char payloadType = requestData.getPayloadType();
    responseData.setPayloadType(requestData.getPayloadType());
    responseData.setId(requestData.getId());

    if(payloadType == 0xf1){ //registration handler
        registrationProcess(requestData, responseData);
    }else if(payloadType == 0xa1){ //ping handler
        pingProcess(requestData, responseData);
    }else if(payloadType == 0xa4){ //logout handler
        logoutProcess(requestData, responseData);
    }else if(payloadType == 0xa2){ //login handler
        loginProcess(requestData, responseData);
    }else if(payloadType==0xa9){ //kick handler
        kickupProcess(requestData, responseData);
    }else if(payloadType==0xa6){ //keep handler
        keepProcess(requestData, responseData);
    }else if(payloadType==0xa3){//enter game
        enterGameProcess(requestData, responseData);
    }else if(payloadType==0xa0){//connect
        connectProcess(requestData, responseData);
    }else if(payloadType==0xe2){
        pointProcess(requestData, responseData);
    }else{
        responseData.setDataValid(false);
    }
}

void RequestHandler::registrationProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;

    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    offset++;
    auto superPassLength = (unsigned char)payloadData[offset];
    std::string superPassword;
    superPassword.resize(superPassLength);
    superPassword.clear();
    for (i = 0; i < superPassLength; i++) {
        offset++;
        superPassword.append(1, payloadData[offset]);
    }
    offset++;
    auto passLength = (unsigned char)payloadData[offset];
    std::string password;
    password.resize(passLength);
    password.clear();
    for (i = 0; i < passLength; i++) {
        offset++;
        password.append(1, payloadData[offset]);
    }
    offset++;
    auto ipLength = (unsigned char)payloadData[offset];
    std::string loginIp;
    loginIp.resize(ipLength);
    loginIp.clear();
    for (i = 0; i < ipLength; i++) {
        offset++;
        loginIp.append(1, payloadData[offset]);
    }
    offset++;
    auto emailLength = (unsigned char)payloadData[offset];
    std::string email;
    email.resize(emailLength);
    email.clear();
    for (i = 0; i < emailLength; i++) {
        offset++;
        email.append(1, payloadData[offset]);
    }
    unsigned char regResult = this->billingMysql.getRegResult(username, password, superPassword, email);
    Logger::write(std::string("user [") + username + "] (" + email + ")try to reg from " + loginIp + " : " + (regResult==1?"ok":"error"));
    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    responseData.appendChar(regResult);
}

void RequestHandler::pingProcess(BillingData &requestData, BillingData &responseData) {
    responseData.setPayloadData("0100");
}

void RequestHandler::logoutProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;
    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    this->billingMysql.updateFieldStatus(username, "is_online", false);
    unsigned char logoutResult = 0;
    Logger::write(std::string("user [") + username + "] logout");
    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    responseData.appendChar(logoutResult);
}

void RequestHandler::loginProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;
    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    offset++;
    auto passLength = (unsigned char)payloadData[offset];
    std::string password;
    password.resize(passLength);
    password.clear();
    for (i = 0; i < passLength; i++) {
        offset++;
        password.append(1, payloadData[offset]);
    }
    offset++;
    auto ipLength = (unsigned char)payloadData[offset];
    std::string loginIp;
    loginIp.resize(ipLength);
    loginIp.clear();
    for (i = 0; i < ipLength; i++) {
        offset++;
        loginIp.append(1, payloadData[offset]);
    }
    unsigned char loginResult = this->billingMysql.getLoginResult(username, password);

    const char* loginResultStr[] = {
            "-",//0 invalid
            "login success",//1
            "account not exists",//2
            "password not correct" ,//3
            "account is allready online",//4
            "account is allready online on other server",//5
            "connect failed",//6
            "account baned",//7
            "point is not available",//8
            "regisiter"//9
    };
    Logger::write(std::string("user [") + username + "] try to login from " + loginIp + " : " + loginResultStr[loginResult]);
    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    responseData.appendChar(loginResult);
}

void RequestHandler::enterGameProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;
    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    offset++;
    auto charLength = (unsigned char)payloadData[offset];
    std::string charName;
    charName.resize(charLength);
    charName.clear();
    for (i = 0; i < charLength; i++) {
        offset++;
        charName.append(1, payloadData[offset]);
    }
    this->billingMysql.updateFieldStatus(username, "is_online", true);
    unsigned char loginResult = 1;

    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    responseData.appendChar(loginResult);
}

void RequestHandler::kickupProcess(BillingData &requestData, BillingData &responseData) {
    responseData.setPayloadData("01");
}

void RequestHandler::keepProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;
    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    unsigned short userLevel;
    offset++;
    userLevel = payloadData[offset] << 8;
    offset++;
    userLevel += payloadData[offset];
    Logger::write(std::string("user [") + username + "] level: " + std::to_string(userLevel));
    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    responseData.appendChar(1);
}

void RequestHandler::connectProcess(BillingData &requestData, BillingData &responseData) {
    responseData.setPayloadData("2000");
}

void RequestHandler::pointProcess(BillingData &requestData, BillingData &responseData) {
    auto payloadData = requestData.getPayloadData();
    size_t offset = 0, i;
    auto usernameLength = (unsigned char)payloadData[offset];
    std::string username;
    username.resize(usernameLength);
    username.clear();
    for (i = 0; i < usernameLength; i++) {
        offset++;
        username.append(1, payloadData[offset]);
    }
    offset++;
    auto ipLength = (unsigned char)payloadData[offset];
    std::string loginIp;
    loginIp.resize(ipLength);
    loginIp.clear();
    for (i = 0; i < ipLength; i++) {
        offset++;
        loginIp.append(1, payloadData[offset]);
    }
    offset++;
    auto charLength = (unsigned char)payloadData[offset];
    std::string charName;
    charName.resize(charLength);
    charName.clear();
    for (i = 0; i < charLength; i++) {
        offset++;
        charName.append(1, payloadData[offset]);
    }
    responseData.appendChar(usernameLength);
    responseData.appendText(username);
    Logger::write(std::string("user [") + username + "] " + charName + " check point at " + loginIp);
    unsigned int pointResult = this->billingMysql.getUserPoint(username);
    pointResult = (pointResult + 1) * 1000;
    unsigned char tmpChar;
    tmpChar = (unsigned char)(pointResult >> 24);
    responseData.appendChar(tmpChar);
    tmpChar = (unsigned char)((pointResult >> 16) & 0xff);
    responseData.appendChar(tmpChar);
    tmpChar = (unsigned char)((pointResult >> 8) & 0xff);
    responseData.appendChar(tmpChar);
    tmpChar = (unsigned char)(pointResult & 0xff);
    responseData.appendChar(tmpChar);
}
