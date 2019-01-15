#pragma once

#include <string>

class PreparedStatement
{
public:
    PreparedStatement();
    ~PreparedStatement();

    bool prepare(const std::string & query);
    bool execute();

    bool bindValue(unsigned int param, int v);
    bool bindValue(unsigned int param, const std::string & v);

    bool nextResult();

    bool value(const std::string & colname, int & v);
    bool value(const std::string & colname, std::string &v);

private:
    class PSImpl;
    PSImpl* impl_;
};


class Database
{
public:
    static bool connect(const std::string & host, const std::string & userName, const std::string & passwd);

    static bool setSchema(const std::string & schema);

    static bool execute(const std::string & s);
};
