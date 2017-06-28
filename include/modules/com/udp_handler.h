#ifndef _UDP_HANDLER_H_
#define _UDP_HANDLER_H_

#include <cstdlib>
#include "include/common.h"
#include <iostream>
#include <boost/asio.hpp>


//A simple asyncronous UDP server using boost asio

using boost::asio::ip::udp;

class udp_handler
{

public:

    udp_handler(boost::asio::io_service &io_service, short port);
    ~udp_handler();

    void handle_receive();
    void handle_send(std::size_t length);

private:
    udp::socket _socket;
    udp::endpoint _sender_endpoint;
    enum { buffer = 1024 };
    char data_[buffer];
};

#endif