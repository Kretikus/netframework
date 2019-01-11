#pragma once

#include <string>

class Database
{
public:
    static bool connect(const std::string & uri, const std::string & userName, const std::string & passwd);

    static bool setSchema(const std::string & schema);
};
