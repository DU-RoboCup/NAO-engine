#ifndef _NETWORKING_H_
#define _NETWORKING_H_

#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

#include "include/modules/com/udp_handler.h"


class Networking : public Module {

public:
    static Networking* Instance();
    void Reconfigure(std::string config_file, uint16_t id);
    bool RunFrame();
    bool ProcessIntent(Intent &i);
    bool Install();
    bool Uninstall();

    void UDPListen();

private:
    static Networking *instance;

    
    boost::asio::io_service io_service;
    udp_handler udpserver;

    Networking();
    

};

#endif