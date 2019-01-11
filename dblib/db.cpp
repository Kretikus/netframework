#include "db.h"

/*
To have launchd start mariadb now and restart at login:
  brew services start mariadb
Or, if you don't want/need a background service you can just run:
  mysql.server start
*/

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <iostream>

void testdb()
{
    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        /* Create a connection */
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        /* Connect to the MySQL test database */
        con->setSchema("test");

        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT 'Hello World!' AS _message");
        while (res->next()) {
            std::cout << "\t... MySQL replies: ";
            /* Access column data by alias or column name */
            std::cout << res->getString("_message") << std::endl;
            std::cout << "\t... MySQL says it again: ";
            /* Access column data by numeric offset, 1 is the first column */
            std::cout << res->getString(1) << std::endl;
        }

        delete res;
        delete stmt;
        delete con;

    } catch (sql::SQLException &e) {
        std::cout << "# ERR: SQLException in " << __FILE__;
        std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
        std::cout << "# ERR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
}

struct Impl
{
    sql::Driver *driver = nullptr;
    sql::Connection *con = nullptr;

    bool ensureConnected() {
        if (!con) {
            std::cerr << "Database not open" << std::endl;
        }
        return con;
    }
};

static Impl * instance() {
    static Impl impl;
    return &impl;
}

static void printException(sql::SQLException &e, const char* filename, int line, const char* function)
{
    std::cout << "# ERR: SQLException in " << filename;
    std::cout << "(" << function << ") on line " << line << std::endl;
    std::cout << "# ERR: " << e.what();
    std::cout << " (MySQL error code: " << e.getErrorCode();
    std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
}

bool Database::connect(const std::string & uri, const std::string & userName, const std::string & passwd)
{
    Impl* impl = instance();
    try {
        if(!impl->driver) {
            impl->driver = get_driver_instance();
        }
        impl->con = impl->driver->connect(uri, userName, passwd);
    } catch (sql::SQLException &e) {
        printException(e, __FILE__, __LINE__, __FUNCTION__);
    }
    return impl->con != nullptr;
}

bool Database::setSchema(const std::string &schema)
{
    Impl* impl = instance();
    if (!impl->ensureConnected()) return false;

    bool ret = true;
    try {
        impl->con->setSchema(schema);
    } catch (sql::SQLException &e) {
       printException(e, __FILE__, __LINE__, __FUNCTION__);
       ret = false;
    }
    return ret;
}
