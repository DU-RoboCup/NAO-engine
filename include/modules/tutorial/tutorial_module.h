#ifndef TUTORIAL_MODULE
#define TUTORIAL_MODULE

#include "include/common.h"

class TutorialModule : public Module {
    public:
        static TutorialModule *Instance();

        void Reconfigure(std::string config_file, uint16_t id);
        bool RunFrame();
        bool ProcessIntent(Intent &i);
        bool Install();
        bool Uninstall();
        void HelloWorld();
    private:
        static TutorialModule *instance;
};

#endif
