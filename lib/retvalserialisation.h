#pragma once

#include "consthelpers.hpp"
#include "serialisation.h"

constexpr bool is_retval(const StringLiteral & t)
{
    return t.contains("&") && !t.contains("const");
}

///
/// OptionalDeserialiser
///////

template <typename T, bool deser>
struct OptionalDeserialiser;

template <typename T>
struct OptionalDeserialiser<T,true> {
    void operator()(ObjectDeser & ser, T & t)
    {
        ser >> t;
    }
};

template <typename T>
struct OptionalDeserialiser<T,false> {
    void operator()(ObjectDeser & /*ser*/, T & /*t*/) {}
};

///
/// parseAnswer
///////

template<typename TR, typename T1,  bool r1>
inline void parseAnswer(std::istream & in, TR & tr)
{
    ObjectDeser deser(in);
    deser >> tr;
}

template<typename TR, typename T1,  bool r1>
inline void parseAnswer(std::istream & in, TR & tr, T1 & t1)
{
    ObjectDeser deser(in);
    deser >> tr;
    OptionalDeserialiser<T1, r1>()(deser, t1);
}

template<typename TR, typename T1,  bool r1,typename T2, bool r2>
inline void parseAnswer(std::istream & in, TR & tr, T1 & t1, T2 & t2)
{
    ObjectDeser deser(in);
    deser >> tr;
    OptionalDeserialiser<T1, r1>()(deser, t1);
    OptionalDeserialiser<T2, r2>()(deser, t2);
}

template<typename TR, typename T1,  bool r1,typename T2, bool r2, typename T3, bool r3>
inline void parseAnswer(std::istream & in, TR & tr, T1 & t1, T2 & t2, T3 & t3)
{
    ObjectDeser deser(in);
    deser >> tr;
    OptionalDeserialiser<T1, r1>()(deser, t1);
    OptionalDeserialiser<T2, r2>()(deser, t2);
    OptionalDeserialiser<T3, r3>()(deser, t3);
}

///
/// OptionalSerialiser
///////

template <typename T, bool deser>
struct OptionalSerialiser;

template <typename T>
struct OptionalSerialiser<T,true> {
    void operator()(ObjectSer & ser, T & t)
    {
        ser << t;
    }
};

template <typename T>
struct OptionalSerialiser<T,false> {
    void operator()(ObjectSer & /*ser*/, T & /*t*/) {}
};

///
/// serialiseAnswer
///////

template<typename TR, typename T1,  bool r1,typename T2, bool r2, typename T3, bool r3>
inline std::string serialiseAnswer(TR & tr, T1 & t1, T2 & t2, T3 & t3)
{
    std::stringstream out;
    ObjectSer ser(out);
    ser << tr;
    OptionalSerialiser<T1, r1>()(ser, t1);
    OptionalSerialiser<T2, r2>()(ser, t2);
    OptionalSerialiser<T3, r3>()(ser, t3);
    return out.str();
}
