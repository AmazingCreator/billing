#ifndef BILLING_BILLINGDATA_H
#define BILLING_BILLINGDATA_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class BillingData
{
public:
    BillingData(std::vector<char>& request);
    BillingData();
    ~BillingData();
    const unsigned char& getPayloadType() {
        return this->payloadType;
    }
    void setPayloadType(const unsigned char payloadType) {
        this->payloadType = payloadType;
    }
    const unsigned short& getPayloadLength() {
        return this->payloadLength;
    }
    const std::vector<char>&  getPayloadData() {
        return this->payloadData;
    }

    void setPayloadData(const std::vector<char>& payloadData) {
        this->payloadData.resize(payloadData.size());
        this->payloadData.clear();
        this->appendPayloadData(payloadData);
    }

    void setPayloadData(const char* hexStr) {
        this->payloadData.clear();
        this->appendPayloadData(hexStr);
    }

    void appendPayloadData(const std::vector<char>& payloadData) {
        for (auto it = payloadData.begin(); it != payloadData.end(); it++) {
            this->payloadData.emplace_back(*it);
        }
        this->payloadLength = (unsigned short)this->payloadData.size() + 3;
    }

    void appendPayloadData(const char* hexStr) {
        std::vector<char> data;
        char prev = 0;
        for(int i=0; hexStr[i] ;i++){
            char curr = hexStr[i];
            if ((curr >= '0') && (curr <= '9')) {
                curr -= '0';
            }else if ((curr >= 'A') && (curr <= 'F')) {
                curr -= '7';
            }else if ((curr >= 'a') && (curr <= 'f')) {
                curr -= 'W';
            }else {
                curr = 0;
            }
            if(i%2==1){
                data.emplace_back(prev << 4 | curr);
            }
            prev = curr;
        }
        this->appendPayloadData(data);
    }

    void appendText(std::string text) {
        for (auto it = text.begin(); it != text.end(); it++) {
            this->payloadData.emplace_back(*it);
        }
        this->payloadLength = (unsigned short)this->payloadData.size() + 3;
    }

    void appendChar(unsigned char charItem) {
        this->payloadData.emplace_back(charItem);
        this->payloadLength = (unsigned short)this->payloadData.size() + 3;
    }

    const std::vector<char>& getId() {
        return this->id;
    }

    void setId(const std::vector<char>& id) {
        this->id.clear();
        this->id.emplace_back(id[0]);
        this->id.emplace_back(id[1]);
    }

    void packData(std::vector<char>& buff);
    void doDump(std::string& debug);
    const bool& isDataValid() {
        return this->isValid;
    }
    void setDataValid(bool valid){
        this->isValid = valid;
    }

private:
    bool isValid;
    unsigned short payloadLength{3};
    unsigned char payloadType{0};
    std::vector<char> id;
    std::vector<char> payloadData;
    void parseData(std::vector<char>& request);
};

#endif
