#include "Client.h"
Client::Client(asio::io_context & ioc,std::string protocol,std::string addr)
    :s_(ioc),
    resolver_(ioc),
    protocol_(protocol),
    addr_(addr)
{
    
};
void Client::send_request(std::string uri){
    std::string headers_raw = "GET "+uri+" HTTP/1.0\r\n";
    for( auto el : headers_){
        headers_raw+=el.first+":"+el.second+"\r\n";
    }
    headers_raw+="\r\n";
    //std::cout<<headers_raw<<std::endl;
    s_.send(asio::buffer(headers_raw,headers_raw.size()));
    std::string buf;
    buf.resize(1024);
    asio::error_code ec;
    size_t s=0;
    while( (s = s_.receive(asio::buffer(buf,buf.size()),0,ec))>0 ){
        raw_response_.append(buf,0,s);
        buf.clear();
        buf.resize(1024);
        s=0;
    }
    //std::cout<<raw_response_.substr(0,raw_response_.find("\r\n\r\n"))<<std::endl;
    s_.close();
}
std::map<std::string,std::string> Client::get_response_headers(){
    std::map<std::string,std::string> headers;
    if( !raw_response_.empty() ){
        std::string raw_response_headers = raw_response_.substr(0,raw_response_.find("\r\n\r\n"));
        //std::cout<<raw_response_headers;
        size_t pos_end_line=0,last_pos=0;
        size_t pos_param_delimiter=0;
        std::string substr;
        while( last_pos<raw_response_headers.size() and (pos_end_line = raw_response_headers.find("\r\n",last_pos))!=std::string::npos ){
            substr = raw_response_headers.substr(last_pos,pos_end_line-last_pos);
            
            pos_param_delimiter = substr.find(": ");
            if( pos_param_delimiter!=std::string::npos ){
                headers[ substr.substr(0,pos_param_delimiter) ] = substr.substr(pos_param_delimiter+2,std::string::npos);
                
            }
            last_pos=pos_end_line+2;
        }
    }
    return headers;
}
bool Client::connect(){
    headers_["Host"] = addr_;
    asio::ip::tcp::resolver::query query(addr_,protocol_);
    asio::ip::tcp::resolver::iterator itr = resolver_.resolve(query);
    asio::ip::tcp::resolver::iterator end;
    asio::error_code ec;
    bool connect_flag = false;
    for(auto el = itr; el!=end; el++ ){
        s_.connect(*el,ec);
        if(!ec){
            connect_flag = true;
            break;
        }
    }
    return connect_flag;
}
