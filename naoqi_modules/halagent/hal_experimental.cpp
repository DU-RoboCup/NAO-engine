
#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;    
const std::string hal_experimental::name("hal_experimental");

hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        shm(open_or_create, "PineappleJuice", sizeof(pineappleJuice))
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");

    std::cout << "Starting hal_experimental!" << std::endl;
    dcm = new DCMProxy(pBroker);
    nao_memory_proxy = new ALMemoryProxy(pBroker);
    
    body_ID = (std::string) nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0);
    head_ID = (std::string) nao_memory_proxy->getData("RobotConfig/Head/FullHeadId", 0);
    body_version = (std::string) nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0);
    head_version = (std::string) nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0);

    pineappleJuice = shm.construct<hal_data>("PineappleJuice")();
}

hal_experimental::~hal_experimental()
{
    std::cout << "Destructing hal_experimental" << std::endl;
    delete dcm;
    delete nao_memory_proxy;
}

void preCallBack_(hal_experimental *instance)
{
    instance->preCallBack();
}
void postCallBack_(hal_experimental *instance)
{
    instance->postCallBack();
}

void hal_experimental::preCallBack()
{
    std::cout << "preCallBack called." << std::endl;
}
void hal_experimental::postCallBack()
{
    std::cout << "postCallBack called." << std::endl;
}

extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
    AL::ALModule::createModule<hal_experimental>(pBroker, "hal_experimental");
    return 0;
}

int main(int argc, char* argv[])
{
    std::cout << "main function called" << std::endl;
}