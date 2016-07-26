#include "include/modules/hal/libpineapple.h"
#include "include/modules/hal/ananas_comosus.h"

extern "C"
{
    Module *mkrfn(){
        return Pineapple::Instance();
    }
}
using namespace boost::interprocess;


Pineapple *Pineapple::instance;
Pineapple *Pineapple::Instance()
{
    if(UNLIKELY(Pineapple::instance == 0))
        Pineapple::instance = new Pineapple();
    return Pineapple::instance;
}

Pineapple::Pineapple() :
    shm(open_only, "Fridge")
{
}

void Pineapple::Reconfigure(std::string config_file, uint16_t id)
{
    this->ModuleID = id;
    LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
    this->ModuleName = mconfig["name"].get<std::string>();
    this->ModuleFPS = (int)mconfig["rfps"].get<double>();
    this->ModulePriority = (int)mconfig["mprio"].get<double>();
    this->ModuleThread = (int)mconfig["mthr"].get<double>();
}
bool Pineapple::RunFrame()
{
    return true;
}

bool Pineapple::ProcessIntent(Intent &i)
{
    return true;
}

bool Pineapple::Install()
{
    LOG_DEBUG << "Installing the 'Pineapple' Module. Trying to open shared memory 'PineappleJuice'";
    try {
         pineappleJuice = shm.find_or_construct<LPData_Buffer>("PineappleJuice")();
         LOG_DEBUG << "Shared Memory 'PineappleJuice' was successfully opened";
    } catch(interprocess_exception &ex) {
        LOG_FATAL << "Could not find the shared memory block PineappleJuice because: " << ex.what();
    }

    LOG_DEBUG << "Installed the 'Pineapple' module";
    return true;
}
bool Pineapple::Uninstall()
{
    LOG_DEBUG << "Uninstalled the 'Pineapple' module";
    return true;
}










