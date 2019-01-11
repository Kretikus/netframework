#pragma once

#include <netinfo.h>
#include <retvalserialisation.h>
#include <threadedclass.h>

#include <functional>
#include <map>

class HandlerBase : public ThreadedClass<>
{
public:
    HandlerBase(const char* handlerName);
    std::string handlerName() const;

    void handleCall(const std::string & signature, const std::string & data, const NetInfo& netInfo);

    virtual void sendAnswer(const NetInfo & netInfo, const std::string & data);

protected:
    std::map<std::string, std::function<void(const std::string&, const NetInfo&)>> functions_;

    template<typename T1>
    void readData(const std::string & data, T1 & t1)
    {
        std::istringstream in(data);
        ObjectDeser deser(in);
        deser >> t1;
    }

    template<typename T1, typename T2>
    void readData(const std::string & data, T1 & t1, T2 & t2)
    {
        std::istringstream in(data);
        ObjectDeser deser(in);
        deser >> t1 >> t2;
    }

    template<typename T1, typename T2, typename T3>
    void readData(const std::string & data, T1 & t1, T2 & t2, T3 & t3)
    {
        std::istringstream in(data);
        ObjectDeser deser(in);
        deser >> t1 >> t2 >> t3;
    }

    NetInfo currNetInfo_;
};

#define REGISTER_RMI0(rettype, name) \
{ \
    functions_[#name] = [&](const std::string& data, const NetInfo& netInfo) -> void { \
                            rettype retval = name(); \
                            const std::string retdata = serialiseAnswer<rettype>(retval); \
                            sendAnswer(netInfo, retdata); \
                        }; \
}

#define REGISTER_RMI1(rettype, name, type1) \
{ \
    functions_[#name] = [&](const std::string& data, const NetInfo& netInfo) -> void { \
                            std::remove_const< std::remove_reference<type1>::type>::type t1; \
                            readData(data, t1); \
                            rettype retval = name(t1); \
                            const std::string retdata = serialiseAnswer<rettype, type1, is_retval(#type1)>(retval, t1); \
                            sendAnswer(netInfo, retdata); \
                        }; \
}

#define REGISTER_RMI2(rettype, name, type1, type2) \
{ \
    functions_[#name] = [&](const std::string& data, const NetInfo& netInfo) -> void { \
                            std::remove_const< std::remove_reference<type1>::type>::type t1; \
                            std::remove_const< std::remove_reference<type2>::type>::type t2; \
                            readData(data, t1, t2); \
                            rettype retval = name(t1, t2); \
                            const std::string retdata = serialiseAnswer<rettype, type1, is_retval(#type1), type2, is_retval(#type2)>(retval, t1, t2); \
                            sendAnswer(netInfo, retdata); \
                        }; \
}

#define REGISTER_RMI3(rettype, name, type1, type2, type3) \
{ \
    functions_[#name] = [&](const std::string& data, const NetInfo& netInfo) -> void { \
                            std::remove_const< std::remove_reference<type1>::type>::type t1; \
                            std::remove_const< std::remove_reference<type2>::type>::type t2; \
                            std::remove_const< std::remove_reference<type3>::type>::type t3; \
                            readData(data, t1, t2, t3); \
                            rettype retval = name(t1, t2, t3); \
                            const std::string retdata = serialiseAnswer<rettype, type1, is_retval(#type1), type2, is_retval(#type2), type3, is_retval(#type3)>(retval, t1, t2, t3); \
                            sendAnswer(netInfo, retdata); \
                        }; \
}
