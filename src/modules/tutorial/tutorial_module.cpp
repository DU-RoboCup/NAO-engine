#include "include/modules/tutorial/tutorial_module.h"
#include "include/modules/tutorial/load_module.h"

REGISTER_MODULE(TutorialModule)

using std::string;

TutorialModule *TutorialModule::instance;
TutorialModule *TutorialModule::Instance() {
    if(UNLIKELY(TutorialModule::instance = 0)) {
        TutorialModule::instance = new TutorialModule();
    }
    return TutorialModule::instance;
}

void TutorialModule::Reconfigure(string config_file, uint16_t id) {
    this->ModuleID = id;

    LuaTable mconfig = LuaTable::fromFile(config_file.c_str());
    this->ModuleName = mconfig["name"].get<string>();
    this->ModuleFPS = (int)mconfig["rfps"].get<double>();
    this->ModulePriority = (int)mconfig["mprio"].get<double>();
    this->ModuleThread = (int)mconfig["mthr"].get<double>();
}

bool TutorialModule::RunFrame() {
    HelloWorld();
    return true;
}

bool TutorialModule::ProcessIntent(Intent &i) {
    LOG_DEBUG << "Received intent:" << i.value;
    return true;
}

bool TutorialModule::Install() {
    LOG_DEBUG << "MODULE PRIMED. ARMING WEAPONS";
    return true;
}

bool TutorialModule::Uninstall() {
    LOG_DEBUG << "WEAPONS DISARMED. YOU NEVER LET ME HAVE ANY FUN";
    return true;
}

void TutorialModule::HelloWorld() {
    LOG_DEBUG << "DESTROY DESTROY DESTROY DESTROY DESTROY";
}
