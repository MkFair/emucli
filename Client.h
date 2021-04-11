#include <asio.hpp>
#include <iostream>
#include <string>
#include <map>




class Client{
    asio::ip::tcp::socket s_;
    asio::ip::tcp::resolver resolver_;
    std::map<std::string,std::string> headers_;
    std::string raw_response_;
    std::string protocol_;
    std::string addr_;
    public:
    Client(asio::io_context & ioc,std::string protocol,std::string addr);
    ~Client(){};
    bool connect();
    void send_request(std::string);
    std::map<std::string,std::string> get_response_headers();
};
