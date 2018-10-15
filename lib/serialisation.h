#pragma once

#include <iostream>

#include <string>
#include <vector>
#include <set>

#include <sstream>

/*********************************************************************************
*  Serialize
*/
std::string replaceInString(std::string subject, const std::string& search, const std::string& replace);

std::ostream & serialize(std::ostream & out, int d);
std::ostream & serialize(std::ostream & out, const std::string & str);

template<typename T>
inline std::ostream & serializeSequence(std::ostream & out, const T & vec)
{
    out << "[";
    bool first = true;
    for (const auto & elem : vec) {
        if (!first) out << ",";
        else first = false;
        serialize(out, elem);
    }
    out << "]";
    return out;
}

template<typename T>
inline std::ostream & serialize(std::ostream & out, const std::vector<T> & vec)
{
    return serializeSequence(out, vec);
}

template<typename T>
inline std::ostream & serialize(std::ostream & out, const std::set<T> & set)
{
    return serializeSequence(out, set);
}

template <class T>
inline std::ostream & serialize(std::ostream & out, const T& obj)
{
    obj.serialize(out);
    return out;
}

class ObjectSer
{
public:
    ObjectSer(std::ostream & out);
    ~ObjectSer();

    template <typename T>
    ObjectSer & operator <<(const T & t) {
        if (!_first) {
            _out << ",";
        }
        else {
            _first = false;
        }
        serialize(_out, t);
        return *this;
    }

private:
    bool _first;
    std::ostream & _out;
};

/*********************************************************************************
*  Deserialize
*/
std::string nextToken(std::istream & in);

std::istream & deserialize(std::istream & in, int & k);
std::istream & deserialize(std::istream & in, std::string & k);

template<typename T>
inline std::istream & deserializeSequence(std::istream & in, T & vec)
{
    std::string seqData = nextToken(in);
    if (seqData.length() > 2) {
        seqData.pop_back();
        std::istringstream sq(seqData);
        sq.get(); // consume [

        while (!sq.eof()) {
            std::string elem = nextToken(sq);
            std::istringstream elemData(elem);
            typename T::value_type element;
            deserialize(elemData, element);
            vec.push_back(element);
        }
    }
    return in;
}

template<typename T>
inline std::istream & deserialize(std::istream & in, std::vector<T> & vec)
{
    return deserializeSequence(in, vec);
}

template<class T>
inline std::istream & deserialize(std::istream & in, T & obj)
{
    obj.deserialize(in);
    return in;
}

class ObjectDeser
{
public:
    ObjectDeser(std::istream & in);

    template <typename T>
    ObjectDeser & operator >>(T & t) {
        deserialize(_in, t);
        return *this;
    }

private:
    std::istringstream _in;
};
