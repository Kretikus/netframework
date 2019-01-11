#include "netbase.h"

#include "coreproto.h"
#include "handlerbase.h"
#include "handlermanager.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <functional>
#include <iostream>

enum { max_length = 4048 };
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SslSocket;

class SessionImpl {
public:
    typedef std::function<void (SessionImpl*)> SessionDestroyedFnc;
public:
    SessionImpl(boost::asio::io_context &io_context, boost::asio::ssl::context &ssl_context,
                HandlerManager & handlerManager
                )
    : clientId_(-1), socket_(io_context, ssl_context), handlerManager_(handlerManager)
    {}

    ~SessionImpl()
    {
        if (destroyedFnc_) destroyedFnc_(this);
    }

    SslSocket::lowest_layer_type& socket()
    {
      return socket_.lowest_layer();
    }

    void start()
    {
      socket_.async_handshake(boost::asio::ssl::stream_base::server,
          boost::bind(&SessionImpl::handleHandshake, this,
            boost::asio::placeholders::error));
    }

    void handleHandshake(const boost::system::error_code& error)
     {
       if (!error)
       {
         socket_.async_read_some(boost::asio::buffer(data_, max_length),
             boost::bind(&SessionImpl::processRead, this,
               boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
       } else {
         delete this;
       }
     }

    void processRead(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            uint32_t pkgSize;
            if (incBuffer_.empty()) {
                if (coreproto::isPacketComplete(data_, pkgSize)) {
                    handlePacket(coreproto::fetchData(data_, pkgSize));
                } else if (coreproto::isPacketStart(data_)) {
                    incBuffer_.insert(std::end(incBuffer_), std::begin(data_), std::begin(data_) + bytes_transferred);
                } else {
                    // discard...
                }
            } else {
                incBuffer_.insert(std::end(incBuffer_), std::begin(data_), std::begin(data_) + bytes_transferred);
                if (coreproto::isPacketComplete(incBuffer_, pkgSize)) {
                    handlePacket(coreproto::fetchData(incBuffer_, pkgSize));
                    incBuffer_.clear();
                } else {
                    if (pkgSize > coreproto::maxPacketSize) {
                        incBuffer_.clear();
                        // disconnect!
                    }
                }
            }

            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&SessionImpl::processRead, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
      } else {
        delete this;
      }
    }

    void handlePacket(const std::string & data) {
        //sendData(coreproto::createPacket(data));
        handlerManager_.handlePacket(data, clientId_);
    }

    void sendData(const std::vector<char>& data)
    {
        outBuffer_.insert(std::end(outBuffer_), std::begin(data), std::end(data));
        if (outBufferProcessing_.empty()) {
            handleWrite(boost::system::error_code(0, boost::system::generic_category()));
        }
    }

    void handleWrite(const boost::system::error_code& error)
    {
      if (!error)
      {
          outBufferProcessing_.swap(outBuffer_);
          if (!outBufferProcessing_.empty())
          {
              boost::asio::async_write(socket_,
                  boost::asio::buffer(&outBufferProcessing_[0], outBufferProcessing_.size()),
                  boost::bind(&SessionImpl::handleWrite, this,
                    boost::asio::placeholders::error));
          }

      } else {
        delete this;
      }
    }

public:
    int clientId_;
    SslSocket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    SessionDestroyedFnc destroyedFnc_ = nullptr;
    HandlerManager & handlerManager_;

    std::vector<char> incBuffer_;
    std::vector<char> outBuffer_;
    std::vector<char> outBufferProcessing_;
};


class NetBase::Impl
{
public:
  boost::asio::io_context io_context_;
  boost::asio::ssl::context ssl_context_ = boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
  boost::asio::ip::tcp::acceptor acceptor_;
  HandlerManager handlerManager_;
  std::vector<SessionImpl*> sessions_;

public:
  Impl(unsigned short port)
      : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
      ssl_context_.set_options(
          boost::asio::ssl::context::default_workarounds
          | boost::asio::ssl::context::no_sslv2
          | boost::asio::ssl::context::single_dh_use);

      handlerManager_.setSendCallback([this](const std::string& data, int clientId) {
          auto it = std::find_if(sessions_.begin(), sessions_.end(), [clientId](SessionImpl* session) -> bool {
                        return session->clientId_ == clientId;
                     });
          if (it != sessions_.end()) {
            (*it)->sendData(coreproto::createPacket(data));
          }
      });
    }

  void start_accept()
  {
      SessionImpl* newSession = new SessionImpl(io_context_, ssl_context_, handlerManager_);
      acceptor_.async_accept(newSession->socket(),
          boost::bind(&Impl::handleAccept, this, newSession,
            boost::asio::placeholders::error));
  }

  void clientSessionDestroyed(SessionImpl* impl)
  {
      auto it = std::find(sessions_.begin(), sessions_.end(), impl);
      if (it != sessions_.end()) {
          sessions_.erase(it);
      }
  }

  void handleAccept(SessionImpl* newSession,
      const boost::system::error_code& error)
  {
    static int clientId = 0;
    if (!error) {
      newSession->clientId_ = clientId++;
      newSession->destroyedFnc_ = [&](SessionImpl* session) { clientSessionDestroyed(session); };
      sessions_.push_back(newSession);
      newSession->start();

    } else {
      delete newSession;
    }

    start_accept();
  }


};

NetBase::NetBase(unsigned short port, const std::string & certificateDir)
    : impl_( new Impl(port))
{
    impl_->ssl_context_.use_certificate_chain_file(certificateDir + "/server.pem");
    impl_->ssl_context_.use_private_key_file(certificateDir + "/server.pem", boost::asio::ssl::context::pem);
    impl_->ssl_context_.use_tmp_dh_file(certificateDir + "/dh2048.pem");
}

NetBase::~NetBase()
{
    delete impl_;
}

void NetBase::registerHandler(HandlerBase *handlerBase)
{
    impl_->handlerManager_.registerHandler(handlerBase);
}

void NetBase::start()
{
    impl_->start_accept();
    impl_->io_context_.run();
}

void NetBase::stop()
{
    impl_->io_context_.stop();
}
