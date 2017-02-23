
#include "hal_experimental.h"

using namespace AL;
using std::string;
using namespace boost::interprocess;    
const std::string hal_experimental::name("hal_experimental");
hal_experimental::hal_experimental(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName) 
    :   ALModule(pBroker, pName),
        shm(open_or_create, "PineappleJuice", 65536) /** Allocate a 64KiB region in shared memory, with segment name "PineappleJuice", subsections of this region of memory need to be allocated to store data **/
{
    setModuleDescription("Communicates between the Naoqi process and our Pineapple");

    std::cout << "Starting hal_experimental!" << std::endl;
    dcm = new DCMProxy(pBroker);
    nao_memory_proxy = new ALMemoryProxy(pBroker);
    
    body_ID = (std::string) nao_memory_proxy->getData("Device/DeviceList/ChestBoard/BodyId", 0);
    // head_ID = (std::string) nao_memory_proxy->getData("RobotConfig/Head/FullHeadId", 0);
    // body_version = (std::string) nao_memory_proxy->getData("RobotConfig/Body/BaseVersion", 0);
    // head_version = (std::string) nao_memory_proxy->getData("RobotConfig/Head/BaseVersion", 0);
    std::cout << "HeadID proxy call value: " << body_ID << std::endl;

    std::cout << "Testing value set!" << std::endl;
    //This is a test
    try
    {
        ALValue commandsAlias;
        ALValue commands;
        commandsAlias.arraySetSize(2);
        commandsAlias[0] = string("ChestLeds");
        commandsAlias[1].arraySetSize(3);
        commandsAlias[1][0] = string("ChestBoard/Led/Red/Actuator/Value");
        commandsAlias[1][1] = string("ChestBoard/Led/Green/Actuator/Value");
        commandsAlias[1][2] = string("ChestBoard/Led/Blue/Actuator/Value");
        dcm->createAlias(commandsAlias);
        commands.arraySetSize(6);
        commands[0] = string("ChestLeds");
        commands[1] = string("ClearAll");
        commands[2] = string("time-separate");
        commands[3] = 0;
        commands[4].arraySetSize(6);
        commands[4][0] = dcm->getTime(10000);
        commands[4][1] = dcm->getTime(20000);
        commands[4][2] = dcm->getTime(30000);
        commands[4][3] = dcm->getTime(40000);
        commands[4][4] = dcm->getTime(50000);
        commands[4][5] = dcm->getTime(60000);
        commands[5].arraySetSize(3);
        // ChestBoard/Led/Red/Actuator/Value
        commands[5][0].arraySetSize(6);
        commands[5][0][0] = 1.0;
        commands[5][0][1] = 0.0;
        commands[5][0][2] = 1.0;
        commands[5][0][3] = 0.0;
        commands[5][0][4] = 1.0;
        commands[5][0][5] = 0.0;
        // ChestBoard/Led/Green/Actuator/Value
        commands[5][1].arraySetSize(6);
        commands[5][1][0] = 1.0;
        commands[5][1][1] = 0.5;
        commands[5][1][2] = 1.0;
        commands[5][1][3] = 0.25;
        commands[5][1][4] = 0.125;
        commands[5][1][5] = 0.0;
        // ChestBoard/Led/Blue/Actuator/Value
        commands[5][2].arraySetSize(6);
        commands[5][2][0] = 0.0625;
        commands[5][2][1] = 0.125;
        commands[5][2][2] = 0.25;
        commands[5][2][3] = 0.50;
        commands[5][2][4] = 0.75;
        commands[5][2][5] = 1.0;
        dcm->setAlias(commands);
        std::cout << "Success!" << std::endl;
    }catch(std::exception &e) { }


    try{
        pineappleJuice = shm.construct<hal_data>("juicyData")();
        std::cout << "hal_data created in shared memory" << std::cout;
    } catch(boost::interprocess::interprocess_exception &e) {std::cout << "Interprocess error: " << e.what() << std::endl;}
    //set up the callbacks for ALBroker
    // dcm->getGenericProxy()->getModule()->atPostProcess(boost::bind(postCallBack_, this));
    // dcm->getGenericProxy()->getModule()->atPreProcess(boost::bind(preCallBack_, this));
    // dcm->getGenericProxy()->getModule()->atPostProcess(&hal_experimental::onPreCallBack);
    // dcm->getGenericProxy()->getModule()->atPreProcess(&hal_experimental::onPostCallBack);

    // theInstance = NULL;
}
    /**
    * The method is called by NaoQi immediately before it communicates with the chest board.
    * It sets all the actuators.
    */
// static void hal_experimental::onPreCallback() 
// {
//     theInstance->setActuators();
// }
//     /**
//     * The method is called by NaoQi immediately after it communicated with the chest board.
//     * It reads all sensors.
//     */
// static void hal_experimental::postCallBack()
// {
//     theInstance->readSensors();
// }
hal_experimental::~hal_experimental()
{
    std::cout << "Destructing hal_experimental" << std::endl;
    /**
      * Note: If naoqi crashes, by default all data stored in shared memory will (probably)
      * be destroyed. This is why NAOInterface keeps a local copy of the data.
      **/
    shm.destroy<hal_data>("juicyData");
    shared_memory_object::remove("PineappleJuice");
    delete dcm;
    delete nao_memory_proxy;
}


// void hal_experimental::preCallBack(hal_experimental *hal)
// {
//     std::cout << "preCallBack called." << std::endl;
// }
// void hal_experimental::postCallBack(hal_experimental *hal)
// {
//     std::cout << "postCallBack called." << std::endl;
// }
void setActuators()
{
    std::cout << "Setting actuators" << std::endl;
}
extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
    AL::ALModule::createModule<hal_experimental>(pBroker, "hal_experimental");
    return 0;
}

extern "C" int _closeModule()
{
    return 0;
}
