win32 {

OPENSSL_DIR = "C:\\OpenSSL-Win32"

BOOST_DIR = "C:\\Development\\boost_1_69_0"
BOOST_APPENDIX=-vc141-mt-gd-x64-1_69

MYSQL_DIR = "C:\\Development\\mysql\\Connector_c"

} else {

OPENSSL_DIR = /usr/local/opt/openssl
BOOST_DIR = /usr/local/opt/boost
MYSQL_DIR = /usr/local/opt/mysql-client/
}

win32 {

EXT_LIBS += /LIBPATH:$${OPENSSL_DIR}\\lib\\VC64 libeay32.lib ssleay32.lib
INCLUDEPATH += $${OPENSSL_DIR}\\include

EXT_LIBS += /LIBPATH:$${BOOST_DIR}\\stage\\lib libboost_system$${BOOST_APPENDIX}.lib
INCLUDEPATH += $${BOOST_DIR}

EXT_LIBS += /LIBPATH:$${MYSQL_DIR}/lib/vs14/debug mysqlclient.lib
INCLUDEPATH += $${MYSQL_DIR}\\include
EXT_LIBS += Advapi32.lib

} else {
#Config OPENSSL
EXT_LIBS += -L$${OPENSSL_DIR}/lib -lssl -lcrypto
INCLUDEPATH += $${OPENSSL_DIR}/include

#Config BOOST
EXT_LIBS += -L$${BOOST_DIR}/lib -lboost_system
INCLUDEPATH += $${BOOST_DIR}/include

EXT_LIBS += -L$${MYSQL_DIR}/lib -lmysqlclient
INCLUDEPATH += $${MYSQL_DIR}/include
}
