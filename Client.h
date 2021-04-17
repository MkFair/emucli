#include <asio.hpp>
#include <asio/ssl.hpp>
#include <iostream>
#include <string>
#include <map>

class IClient{
    protected:
    asio::ip::tcp::resolver resolver_;
    std::map<std::string,std::string> headers_;
    std::string raw_response_;
    std::string protocol_;
    std::string addr_;
    std::string resp_body_;
    public:
    IClient(asio::io_context & ioc,std::string protocol,std::string addr)
        :resolver_(ioc),
        protocol_(protocol),
        addr_(addr)
    {
        
    }
    
    virtual ~IClient(){};
    virtual void connect();
    virtual std::string body_text();
    virtual void send_request(std::string);
    virtual std::map<std::string,std::string> get_response_headers();
    virtual void send(std::string)=0;
    virtual size_t recv(std::string&)=0;
    virtual void close_socket()=0;
    virtual asio::error_code connect_s(asio::ip::tcp::resolver::iterator)=0;
};


class Client: public IClient{
    protected:
    asio::ip::tcp::socket s_;
    public:
    Client(asio::io_context & ioc,std::string protocol,std::string addr);
    virtual ~Client(){};
    virtual void send(std::string);
    virtual size_t recv(std::string&);
    virtual void close_socket();
    virtual asio::error_code connect_s(asio::ip::tcp::resolver::iterator);
};

class SecureClient : public IClient{
    protected:
    asio::ssl::stream<asio::ip::tcp::socket> s_;
    //asio::ssl::context ssl_ctx;
    public:
    SecureClient(asio::io_context & ioc,asio::ssl::context& ssl_ctx,std::string protocol,std::string addr);
    virtual ~SecureClient(){};
    virtual void send(std::string);
    virtual size_t recv(std::string&);
    virtual void close_socket();
    virtual asio::error_code connect_s(asio::ip::tcp::resolver::iterator);
};

