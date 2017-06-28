#include "include/modules/com/udp_handler.h"

udp_handler::udp_handler(boost::asio::io_service &io_service, short port)
    :
        _socket(io_service, udp::endpoint(udp::v4(), port))
{
    LOG_DEBUG << "Server spawned";
    handle_receive();
}

udp_handler::~udp_handler()
{
}

void udp_handler::handle_receive()
{
    _socket.async_receive_from(
            boost::asio::buffer(data_, buffer),
            _sender_endpoint,
            [this](boost::system::error_code ec, std::size_t bytes_recvd)
            {
                if(!ec && bytes_recvd > 0)
                {    
                    LOG_DEBUG << "Buffer: " << data_;
                    handle_send(bytes_recvd);
                } else {
                    LOG_DEBUG"test2";
                    handle_receive();
                }
                LOG_DEBUG << "test3";
                    
            });
            LOG_DEBUG << "Done handling network receive";
}

void udp_handler::handle_send(std::size_t length)
{
    _socket.async_send_to(
            boost::asio::buffer(data_, length),
            _sender_endpoint,
            [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/)
            {
                handle_receive();
            });
            LOG_DEBUG << "Done handling network send";
}

/*
int main(int argc, char* argv[])
{
    try
    {
        if(argc != 2)
        {
            std::cerr << "Async Server Usage: async <port>\n";
            return 1;
        }
        boost::asio::io_service io_service;

        udp_handler s(io_service, std::atoi(argv[1]));
        io_service.run_one();
        std::cout << "Hello, World." << std::endl;
        std::cout << "This shouldn't print if blocking" << std::endl;
    } 
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}
*/