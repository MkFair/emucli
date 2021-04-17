#include "Client.h"
Client::Client(asio::io_context & ioc,std::string protocol,std::string addr)
    :IClient( ioc, protocol, addr),
    s_(ioc)
{
    
};

SecureClient::SecureClient(asio::io_context & ioc,asio::ssl::context &ssl_ctx,std::string protocol,std::string addr)
    ://ssl_ctx(asio::ssl::context::sslv23)
    //,
    IClient( ioc, protocol, addr)
    ,s_(ioc,ssl_ctx)
{
    //std::cout<<"client init"<<std::flush;
}
void IClient::send_request(std::string uri){
    std::string headers_raw = "GET "+uri+" HTTP/1.0\r\n";
    for( auto el : headers_){
        headers_raw+=el.first+":"+el.second+"\r\n";
    }
    headers_raw+="\r\n";
    //std::cout<<headers_raw<<std::endl;
    send(headers_raw);
    std::string buf;
    buf.resize(1024);
    
    size_t s=0;
    while( (s = recv(buf))>0 ){
        
        raw_response_.append(buf,0,s);
        buf.clear();
        buf.resize(1024);
        s=0;
    }
    //std::cout<<raw_response_.substr(0,raw_response_.find("\r\n\r\n"))<<std::endl;
    //std::cout<<raw_response_;
    close_socket();
}

size_t Client::recv(std::string& buf){
    asio::error_code ec;
    return s_.receive(asio::buffer(buf,buf.size()),0,ec);
}

void Client::send(std::string headers_raw){
    s_.send(asio::buffer(headers_raw,headers_raw.size()));
}

std::string IClient::body_text(){
    return raw_response_.substr(raw_response_.find("\r\n\r\n"),std::string::npos);
}

void Client::close_socket(){
  s_.close();  
};
std::map<std::string,std::string> IClient::get_response_headers(){
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
                
            }else{
                //startline 
                headers["startline"] = substr;
            }
            last_pos=pos_end_line+2;
        }
    }
    return headers;
}
void IClient::connect(){
    headers_["Host"] = addr_;
    
    asio::ip::tcp::resolver::query query(addr_,protocol_);
    asio::ip::tcp::resolver::iterator itr = resolver_.resolve(query);
    //asio::ip::tcp::resolver::iterator end;
    asio::error_code ec;
    connect_s(itr);
   
}
asio::error_code Client::connect_s( asio::ip::tcp::resolver::iterator itr ){
    asio::error_code ec;
    asio::connect( s_, itr );  
    return ec;
};

asio::error_code SecureClient::connect_s(asio::ip::tcp::resolver::iterator itr){
    asio::error_code ec;
    //std::cout<<"client connect"<<std::flush;
    //ssl_ctx.set_verify_mode(asio::ssl::verify_none);
    asio::connect( s_.lowest_layer(), itr );  
    
    //std::cout<<"client handshake"<<std::flush;
    s_.handshake(asio::ssl::stream_base::client,ec);
    return ec;
};

void SecureClient::close_socket(){
  s_.next_layer().close();  
};
void SecureClient::send(std::string headers_raw){
    s_.write_some(asio::buffer(headers_raw,headers_raw.size()));
}
size_t SecureClient::recv(std::string& buf){
    asio::error_code ec;
    return s_.read_some(asio::buffer(buf,buf.size()),ec);
}
