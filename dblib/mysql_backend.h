#pragma once

#include <my_global.h>
#include <mysql.h>

#include <string>

class MySQLStatement;

class MySQL
{
public:
    MySQL();

    bool isConnected() const;
    std::pair<bool, std::string> connect(const std::string & host, const std::string & user, const std::string & pw, uint port);
    bool setSchema(const std::string& schema);

    std::pair<bool, std::string> executeQuery(const std::string& query);


    MYSQL_STMT* createStatement();
    std::string getErrorString();

    MYSQL mysql_;
    bool isConnected_;
};
