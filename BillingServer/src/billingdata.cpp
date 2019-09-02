#include "billingdata.h"

BillingData::BillingData(std::vector<char>& request):isValid(false)
{
    if (!request.empty()) {
        this->parseData(request);
    }
}

void BillingData::parseData(std::vector<char>& request)
{
    auto it = request.begin();
    auto offset = it;
    unsigned char mask0 = 0xAA;
    unsigned char mask1 = 0x55;
    unsigned char tmpChar = 0;
    bool matchMask = false;
    while (it != request.end()) {
        tmpChar = static_cast<unsigned char>(*it);
        if (tmpChar == mask0) {
            if (it + 1 == request.end()) {
                return;
            }
            tmpChar = static_cast<unsigned char>(*(it + 1));
            if (tmpChar == mask1) {
                matchMask = true;
                it++;
                offset = it + 1;
                break;
            }
        }
        it++;
    }
    if (!matchMask) {
        return;
    }
    if (it + 2 >= request.end()) {
        return;
    }
    it++;
    this->payloadLength = static_cast<unsigned short>(static_cast<unsigned char>(*it)) << 8;
    it++;
    this->payloadLength += static_cast<unsigned short>(static_cast<unsigned char>(*it));

    if (it + static_cast<int>(this->payloadLength) + 2 >= request.end()) {
        return;
    }
    it++;
    this->payloadType = static_cast<unsigned char>(*it);
    it++;
    this->id.insert(this->id.end(), it, it + 2);
    it += 2;
    this->payloadData.insert(this->payloadData.end(), it, it + static_cast<int>(this->payloadLength) - 3);

    it += (static_cast<int>(this->payloadLength) - 3);
    if ((static_cast<unsigned char>(*it) == mask1) && (static_cast<unsigned char>(*(it + 1)) == mask0)) {
        this->isValid = true;
        request.erase(request.begin(), it + 2);
    }
    else {
        request.erase(request.begin(), offset);
    }
}

BillingData::BillingData():isValid(true)
{
}

BillingData::~BillingData()
{

}

void BillingData::packData(std::vector<char>& buff)
{
    if (!this->isValid) {
        return;
    }
    buff.resize(4 + 2 + this->payloadLength);
    buff.clear();
    buff.emplace_back(0xAA);
    buff.emplace_back(0x55);
    unsigned char len0, len1;
    len0 = (unsigned char)(this->payloadLength >> 8);
    len1 = (unsigned char)(this->payloadLength & 0xff);
    buff.emplace_back(len0);
    buff.emplace_back(len1);
    buff.emplace_back(this->payloadType);
    buff.emplace_back(this->id[0]);
    buff.emplace_back(this->id[1]);
    for (char & it : this->payloadData) {
        buff.emplace_back(it);
    }
    buff.emplace_back(0x55);
    buff.emplace_back(0xAA);
}

void BillingData::doDump(std::string& debug)
{
    debug.clear();
    if (!this->isValid) {
        debug.append("[not valid]");
        return;
    }
    debug.append("{\r\nisValid: ").append(this->isValid ? "true" : "false").append(",\r\n");
    debug.append("payloadLength: ").append(std::to_string(this->payloadLength)).append(",\r\n");
    std::vector<char> payloadTypeBytes(1, this->payloadType);
    std::stringstream ss;
    for(char & it : payloadTypeBytes){
        ss << std::hex << it;
    }
    debug.append("payloadType: 0x").append(ss.str()).append(",\r\n");
    ss.clear();
    for(char & it : id){
        ss << std::hex << it;
    }
    debug.append("id: 0x").append(ss.str()).append(",\r\n");
    ss.clear();
    for(char & it : payloadData){
        ss << std::hex << it;
    }
    debug.append(ss.str()).append("\r\n}");
}