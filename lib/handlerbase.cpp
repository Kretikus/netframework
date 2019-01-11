#include "handlerbase.h"

HandlerBase::HandlerBase(const char *handlerName)
    :ThreadedClass(handlerName)
{
}

std::string HandlerBase::handlerName() const
{
    return threadName();
}

void HandlerBase::handleCall(const std::string & signature, const std::string & data, const NetInfo & netInfo)
{
    if (!checkThread(this, &HandlerBase::handleCall, signature, data, netInfo)) return;

    auto it = functions_.find(signature);
    if (it != functions_.end()) {
        currNetInfo_ = netInfo;
        it->second(data, netInfo);
        currNetInfo_ = NetInfo();
    } else {
        // TODO error handling
    }
}

void HandlerBase::sendAnswer(const NetInfo &netInfo, const std::string &data)
{
    netInfo.returnCallbackType()(netInfo, data);
}

