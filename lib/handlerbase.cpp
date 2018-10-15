#include "handlerbase.h"

void HandlerBase::handleCall(const std::string & signature, const std::string & data, const NetInfo & netInfo)
{
    auto it = functions_.find(signature);
    if (it != functions_.end()) {
        it->second(data, netInfo);
    } else {
        // TODO error handling
    }
}
