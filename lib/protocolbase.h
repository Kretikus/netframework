#pragma once

#include <string>

class ProtocolBase
{
public:
    ProtocolBase(const std::string & name) : name_(name) {}

protected:
    std::string name_;
};
