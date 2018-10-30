#include "clientnet.h"
#include "coreproto.h"

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <cstdlib>
#include <iostream>

enum { max_length = 16 * 1024 };

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

class ClientNet::Impl
{
private:
  boost::asio::io_context io_context_;
  boost::asio::ssl::context ssl_context_ = boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
  ssl_socket socket_;

  char reply_[max_length];

public:
    Impl()
    : socket_(io_context_, ssl_context_)
    {}

    boost::asio::ip::tcp::resolver::results_type getEndpoints(const std::string & hostname, int port);
    void loadVerifyFile(const std::string & fileName);
    void connect(boost::asio::ip::tcp::resolver::results_type endpoints);
    bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);

    std::string send(const std::vector<char> & data);
};

boost::asio::ip::tcp::resolver::results_type ClientNet::Impl::getEndpoints(const std::string & hostname, int port)
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
    return resolver.resolve(hostname, std::to_string(port));
}

void ClientNet::Impl::loadVerifyFile(const std::string &fileName)
{
    ssl_context_.load_verify_file(fileName);
}

void ClientNet::Impl::connect(boost::asio::ip::tcp::resolver::results_type endpoints)
{
    socket_.set_verify_mode(boost::asio::ssl::verify_peer);
    socket_.set_verify_callback(boost::bind(&Impl::verify_certificate, this, _1, _2));


    // do synchroneous connect
    boost::asio::connect(socket_.lowest_layer(), endpoints);

    socket_.handshake(boost::asio::ssl::stream_base::client);
}

bool ClientNet::Impl::verify_certificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";

    //return preverified;
    return true;
}

std::string ClientNet::Impl::send(const std::vector<char> & data)
{
    boost::system::error_code writeError;
    boost::asio::write(socket_, boost::asio::buffer(&data[0], data.size()), boost::asio::transfer_all(), writeError);

    const std::size_t bytesRead = socket_.read_some(boost::asio::buffer(reply_, max_length));
    uint32_t pkgSize;
    if (coreproto::isPacketComplete(reply_, pkgSize)) {
        return coreproto::fetchData(reply_, pkgSize);
    }
    return std::string();
}

ClientNet::ClientNet(const std::string & sslVerifyFile)
    : impl_(new Impl())
{
    impl_->loadVerifyFile(sslVerifyFile);
}

ClientNet::~ClientNet()
{
    delete impl_;
}

void ClientNet::connect(const std::string & addr, int port)
{
    impl_->connect(impl_->getEndpoints(addr, port));
}

std::string ClientNet::send(const std::string & data)
{
    return impl_->send(coreproto::createPacket(data));
}
