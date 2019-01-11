#pragma once

#include <cstdint>
#include <functional>
#include <string>

class NetInfo
{
public:
    typedef std::function<void(const NetInfo &, const std::string)> ReturnCallbackType;
public:
    NetInfo();

    void clientId(int64_t clId) { clientId_ = clId; }
    int64_t clientId() const { return clientId_; }

    void returnCallbackType(const ReturnCallbackType & retCallback) { retCallback_ = retCallback; }
    ReturnCallbackType returnCallbackType() const { return retCallback_; }

private:
    int64_t clientId_;
    ReturnCallbackType retCallback_;
};
