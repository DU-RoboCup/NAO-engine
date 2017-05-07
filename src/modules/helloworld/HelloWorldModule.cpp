/* HelloWorldModule Take 2 */

#include "include/modules/helloworld/HelloWorldModule.h"

REGISTER_MODULE(HelloWorldModule)


HelloWorldModule* HelloWorldModule::instance;
HelloWorldModule* HelloWorldModule::Instance(){
    if(UNLIKELY(HelloWorldModule::instance == 0)){
        HelloWorldModule::instance = new HelloWorldModule();
    }
    return HelloWorldModule::instance;
}

bool HelloWorldModule::RunFrame(){
    PrintHelloWorld();
    return true;
}

bool HelloWorldModule::ProcessIntent(Intent &i){
    //Ignore it.
    return true;
}

bool HelloWorldModule::Install() {
    LOG_DEBUG << "Installed the 'Hello World' module";
    return true;
}

bool HelloWorldModule::Uninstall(){
    LOG_DEBUG << "Uninstalled the 'Hello World' module";
    return true;
}

void HelloWorldModule::PrintHelloWorld(){
    LOG_DEBUG << "Hello World!";
}


