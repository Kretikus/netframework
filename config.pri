
#Config OPENSSL
OPENSSL_DIR = /usr/local/opt/openssl
EXT_LIBS += -L$${OPENSSL_DIR}/lib -lssl -lcrypto
INCLUDEPATH += $${OPENSSL_DIR}/include

#Config BOOST
BOOST_DIR = /usr/local/opt/boost
EXT_LIBS += -L$${BOOST_DIR}/lib -lboost_system
INCLUDEPATH += $${BOOST_DIR}/include
