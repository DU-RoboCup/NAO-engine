
#include "include/modules/com/networking.h"


#ifndef PORT
#define PORT 5000
#endif
REGISTER_MODULE(Networking)
//Singleton stuff
Networking* Networking::instance;
Networking* Networking::Instance()
{
	if(UNLIKELY(Networking::instance==0))
	{
		Networking::instance = new Networking();
	}
	return Networking::instance;
}

void Networking::Reconfigure(std::string config_file, uint16_t id) {
	this->ModuleID = id;
	LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
	this->ModuleName = mconfig["name"].get<std::string>();
	this->ModuleFPS = static_cast<int>(mconfig["rfps"].get<float>());
	this->ModulePriority = static_cast<int>(mconfig["mprio"].get<float>());
	this->ModuleThread = static_cast<int>(mconfig["mthr"].get<float>());
}

bool Networking::RunFrame(){
    UDPListen();
    return true;
}

bool Networking::ProcessIntent(Intent &i){
    //Ignore it.
    return true;
}

bool Networking::Install() {
    LOG_DEBUG << "Installed the 'Networking' module";
    return true;
}

bool Networking::Uninstall(){
    LOG_DEBUG << "Uninstalled the 'Networking' module";
    return true;
}

Networking::Networking() 
    : 
    udpserver(io_service, PORT)
{
    LOG_DEBUG << "com systems online";
    LOG_WARNING << "Listening for messages on port: " << PORT;
}


void Networking::UDPListen(){
    try
    {
        //LOG_DEBUG << "Listening for message on port: " << PORT;
        //io_service.run_one();
        io_service.run_one();
        //LOG_DEBUG << "End of communication";
    }
    catch(const std::exception &e)
    {
        LOG_WARNING << "Error! UDP com failure: " << e.what();
    }
}


