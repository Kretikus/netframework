#include "handlermanager.h"

#include "rmipacket.h"

HandlerManager::HandlerManager()
{
}

void HandlerManager::registerHandler(HandlerBase *handlerBase)
{
    if (handlerBase == nullptr) return;

    handlers_.insert({handlerBase->threadName(), handlerBase});
}

void HandlerManager::handlePacket(const std::string & data, int64_t clientId)
{
    std::istringstream in(data);
    RMIPacket pkg;
    deserialize(in, pkg);

    auto it = handlers_.find(pkg.handlerName);
    if (it != handlers_.end()) {
        NetInfo netInfo = netInfos_[clientId];
        netInfo.clientId(clientId);

        it->second->handleCall(pkg.signature, pkg.data, netInfo);
    } else {
        // log error
    }

}

void HandlerManager::sendAnswer(const NetInfo &netInfo, const std::string &data)
{

}
