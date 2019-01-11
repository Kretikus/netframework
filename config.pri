
#Config OPENSSL
OPENSSL_DIR = /usr/local/opt/openssl
EXT_LIBS += -L$${OPENSSL_DIR}/lib -lssl -lcrypto
INCLUDEPATH += $${OPENSSL_DIR}/include

#Config BOOST
BOOST_DIR = /usr/local/opt/boost
EXT_LIBS += -L$${BOOST_DIR}/lib -lboost_system
INCLUDEPATH += $${BOOST_DIR}/include

MYSQL_DIR = /usr/local/opt/mysql-client/
EXT_LIBS += -L$${MYSQL_DIR}/lib -lmysqlclient
INCLUDEPATH += $${MYSQL_DIR}/include

MYSQL_CONN_DIR = /usr/local/opt/mysql-connector-c++
EXT_LIBS += -L$${MYSQL_CONN_DIR}/lib -lmysqlcppconn-static
INCLUDEPATH += $${MYSQL_CONN_DIR}/include
