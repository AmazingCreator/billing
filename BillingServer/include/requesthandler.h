#ifndef BILLING_REQUESTHANDLER_H
#define BILLING_REQUESTHANDLER_H

#include <cstddef>
#include "logger.h"
#include "billingdata.h"
#include "billingmysql.h"

class RequestHandler{
public:
    explicit RequestHandler(std::shared_ptr<BillingMysql> bm);
    virtual ~RequestHandler();
    void processRequest(BillingData& requestData, BillingData& responseData);

private:
    void registrationProcess(BillingData& requestData, BillingData& responseData);
    void pingProcess(BillingData& requestData, BillingData& responseData);
    void logoutProcess(BillingData& requestData, BillingData& responseData);
    void loginProcess(BillingData& requestData, BillingData& responseData);
    void enterGameProcess(BillingData& requestData, BillingData& responseData);
    void kickupProcess(BillingData& requestData, BillingData& responseData);
    void keepProcess(BillingData& requestData, BillingData& responseData);
    void connectProcess(BillingData& requestData, BillingData& responseData);
    void pointProcess(BillingData& requestData, BillingData& responseData);
    std::shared_ptr<BillingMysql> billingMysql;
};

#endif
