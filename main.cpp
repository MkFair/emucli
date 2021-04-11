#include <iostream>
#include "Client.h"

int main(){
    asio::io_context ioc;
    Client c = Client(ioc,"http","ru.wikipedia.org");
    c.connect();
    c.send_request("/");
    std::map<std::string,std::string> headers = c.get_response_headers();
    for( auto el : headers ){
        std::cout<<el.first<<": "<<el.second<<std::endl;
    }
    c.connect();
    c.send_request("/wiki/Http");
    headers = c.get_response_headers();
    for( auto el : headers ){
        std::cout<<el.first<<": "<<el.second<<std::endl;
    }
    ioc.run();
    return 0;
}
