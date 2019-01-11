#pragma once

#include "handlerbase.h"
#include "netinfo.h"


#include <map>
#include <string>

class HandlerManager
{
public:
    HandlerManager();

    void registerHandler(HandlerBase * handlerBase);

    void handlePacket(const std::string & data, int64_t clientId);

    void sendAnswer(const NetInfo & netInfo, const std::string & data);

    void setSendCallback(const std::function<void(const std::string&, int)> & callback) { sendData_ = callback; }
public:
    std::map<std::string, HandlerBase *> handlers_;
    std::map<int64_t, NetInfo> netInfos_;
    std::function<void(const std::string&,int)> sendData_;
};
