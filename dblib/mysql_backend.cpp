#include "mysql_backend.h"

MySQL::MySQL()
: isConnected_(false)
{
    mysql_init(&mysql_);
}

bool MySQL::isConnected() const
{
    return isConnected_;
}

std::pair<bool, std::string> MySQL::connect(const std::string &host, const std::string &user, const std::string &pw, uint port)
{
    MYSQL * connret = mysql_real_connect(&mysql_, host.c_str(), user.c_str(), pw.c_str(),
                                         nullptr, port, nullptr, 0);
    isConnected_ = connret != nullptr;
    if (connret == nullptr) {
        return std::make_pair(false, std::string(mysql_error(&mysql_)));
    } else {
        return std::make_pair(true, std::string());
    }
}

bool MySQL::setSchema(const std::string &schema)
{
    return mysql_select_db(&mysql_, schema.c_str()) == 0;
}

std::pair<bool, std::string> MySQL::executeQuery(const std::string &query)
{
    const int ret = mysql_query(&mysql_, query.c_str());
    if (ret != 0) {
        return std::make_pair(false, std::string(mysql_error(&mysql_)));
    } else {
        return std::make_pair(true, std::string());
    }
}

MYSQL_STMT * MySQL::createStatement()
{
    return mysql_stmt_init(&mysql_);
}

std::string MySQL::getErrorString()
{
    return mysql_error(&mysql_);
}
