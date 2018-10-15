#pragma once

#include <retvalserialisation.h>

class RemoteCall
{
public:
    virtual std::string sendAndWait(const std::string& protocol, const std::string & sig, const std::string & data) = 0;

    template<typename TR, typename T1>
    TR make_call(const std::string& protocol, const char* sig) {
        const std::string answer = sendAndWait(protocol, sig, std::string());
        std::istringstream in(answer);
        TR retval = TR();
        parseAnswer<TR>(in, retval);
        return retval;
    }

    template<typename TR, typename T1,  bool r1>
    TR make_call(const std::string& protocol, const char* sig, T1& t1) {
        std::ostringstream out;
        ObjectSer ser(out);
        ser << t1;
        const std::string answer = sendAndWait(protocol, sig, out.str());
        std::istringstream in(answer);
        TR retval = TR();
        parseAnswer<TR, T1, r1>(in, retval, t1);
        return retval;
    }

    template<typename TR, typename T1,  bool r1, typename T2, bool r2>
    TR make_call(const std::string& protocol, const char* sig, T1& t1, T2& t2) {
        std::ostringstream out;
        ObjectSer ser(out);
        ser << t1 << t2;
        const std::string answer = sendAndWait(protocol, sig, out.str());
        std::istringstream in(answer);
        TR retval = TR();
        parseAnswer<TR, T1, r1, T2, r2>(in, retval, t1, t2);
        return retval;
    }

    template<typename TR, typename T1,  bool r1, typename T2, bool r2, typename T3, bool r3>
    TR make_call(const std::string& protocol, const char* sig, T1& t1, T2& t2, T3& t3) {
        std::ostringstream out;
        ObjectSer ser(out);
        ser << t1 << t2 << t3;
        const std::string answer = sendAndWait(protocol, sig, out.str());
        std::istringstream in(answer);
        TR retval = TR();
        parseAnswer<TR, T1, r1, T2, r2, T3, r3>(in, retval, t1, t2, t3);
        return retval;
    }
};

#define MAKE_RMI0(rettype, name) \
    rettype name() override \
    { \
        return make_call<rettype>(name_); \
    }

#define MAKE_RMI1(rettype, name, type1) \
    rettype name(type1 t1) override \
    { \
        return make_call<rettype, type1, is_retval(#type1)>(name_, #name, t1); \
    }

#define MAKE_RMI2(rettype, name, type1, type2) \
    rettype name(type1 t1, type2 t2) override \
    { \
        return make_call<rettype, type1, is_retval(#type1), type2, is_retval(#type2)>(name_, #name, t1, t2); \
    }

#define MAKE_RMI3(rettype, name, type1, type2, type3) \
    rettype name(type1 t1, type2 t2, type3 t3) override \
    { \
        return make_call<rettype, type1, is_retval(#type1), type2, is_retval(#type2), type3, is_retval(#type3)>(name_, #name, t1, t2, t3); \
    }
