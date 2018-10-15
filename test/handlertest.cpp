#include "testutil.h"

#include <handlerbase.h>
#include <protocolbase.h>
#include <remotecall.h>

class AdminProtocol : public ProtocolBase
{
public:
    AdminProtocol() : ProtocolBase("AdminProtocol") {}
    virtual ~AdminProtocol(){}
    virtual int addUser(const std::string & user, std::string & salts, const std::string & pwhash) = 0;
};

class AdminHandler;

class AdminRemote : public AdminProtocol, public RemoteCall
{
public:
    AdminRemote(AdminHandler * handler) : handler_(handler) {}

    MAKE_RMI3(int, addUser, const std::string &, std::string &, const std::string &)

    std::string sendAndWait(const std::string& protocol, const std::string & sig, const std::string & data) override;

private:
    AdminHandler * handler_;
};

class AdminHandler : public AdminProtocol, public HandlerBase
{
public:
    AdminHandler()
    {
        REGISTER_RMI3(int, addUser, const std::string &, std::string &, const std::string &)
    }

public:
    virtual int addUser(const std::string & user, std::string & salts, const std::string & pwhash) override;

public:
    virtual void sendAnswer(const NetInfo& /*netInfo*/, const std::string & data) override {
        answer_ = data;
    }
    std::string answer_;
};

std::string AdminRemote::sendAndWait(const std::string& /*protocol*/, const std::string & sig, const std::string & data)
{
   handler_->handleCall(sig, data, NetInfo());
   return handler_->answer_;
}


class HandlerTest: public QObject
{
    Q_OBJECT
private slots:
    void readAnswerTest() {
        AdminHandler handler;
        AdminRemote remote(&handler);
        std::string salt;
        int userid = remote.addUser("username", salt, "HASH");
        QCOMPARE(userid, 55);
        QCOMPARE(salt, std::string("this is a new salt"));
    }
};

REGISTER_TEST(HandlerTest);

#include "handlertest.moc"

// IMPLEMENTATION OF RMIs

int AdminHandler::addUser(const std::string & /*user*/, std::string &salts, const std::string & /*pwhash*/)
{
    salts = "this is a new salt";
    return 55;
}
