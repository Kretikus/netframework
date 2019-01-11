#ifndef RMIPACKET_H
#define RMIPACKET_H

#include "serialisation.h"

class RMIPacket
{
public:
    RMIPacket();

    std::ostream & serialize(std::ostream & out) const {
        ObjectSer obj(out);
        obj << handlerName << signature << requestId << data;
        return out;
    }

    std::istream & deserialize(std::istream & in) {
        ObjectDeser obj(in);
        obj >> handlerName >> signature >> requestId >> data;
        return in;
    }

public:
    std::string handlerName;
    std::string signature;
    int requestId {-1};
    std::string data;
};

#endif // RMIPACKET_H
