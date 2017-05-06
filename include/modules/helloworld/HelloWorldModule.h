/* HelloWorldModule Take 2 */
#ifndef _HELLOWORLDMODULE_H_
#define _HELLOWORLDMODULE_H_

#include "include/common.h"
#include "include/modules/module.h"
#include "include/memory/intent.h"
#include "include/debug/debugging.h"

class HelloWorldModule : public Module {
    public:
        static HelloWorldModule* Instance();

        bool RunFrame();
        bool ProcessIntent(Intent &i);
        bool Install();
        bool Uninstall();

        void PrintHelloWorld();
    private:
        static HelloWorldModule* instance;
};


#endif /* _HELLOWORLDMODULE_H_ */
