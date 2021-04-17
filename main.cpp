#include <iostream>
#include "Client.h"

int main(){
    asio::io_context ioc;
    asio::ssl::context ssl_ctx(asio::ssl::context::sslv23);
    SecureClient c = SecureClient(ioc,ssl_ctx,"https","game-platform.site");
    c.connect();
    c.send_request("/");
    std::map<std::string,std::string> headers = c.get_response_headers();
    for( auto el : headers ){
        std::cout<<el.first<<": "<<el.second<<std::endl;
    }
    std::cout<<c.body_text()<<std::endl;
    std::cout<<std::endl;
    c.connect();
    c.send_request("/wiki/Http");
    headers = c.get_response_headers();
    for( auto el : headers ){
        std::cout<<el.first<<": "<<el.second<<std::endl;
    }
    std::cout<<c.body_text()<<std::endl;
    ioc.run();
    return 0;
}
