#ifndef _UDP_HANDLER_H_
#define _UDP_HANDLER_H_

#include <cstdlib>
#include "include/common.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
    //boost::asio::deadline_timer deadline_timer;
    enum { buffer = 1024 };
    char data_[buffer];
};

#endif