//#ifndef SERVER_H
//#define SERVER_H

//#include <boost/asio.hpp>
//#include <boost/bind.hpp>
//#include <boost/any.hpp>

//#include "payload.h"
//#include <cstdlib>

//using boost::asio::ip::udp;

//class Server
//{
//public:
//    Server();
//    void handle_receive_from(const boost::system::error_code& error, size_t bytes_recvd);
//    void handle_send_to(const boost::system::error_code& /*error*/, size_t /*bytes_sent*/);

//private:
//    boost::asio::io_service &io_service_;
//    udp::socket socket_;
//    udp::endpoint sender_endpoint_;
//    enum { max_length = 1024 };
//    //Payload payload;
//    char payload[max_length];

//};

//#endif // SERVER_H
