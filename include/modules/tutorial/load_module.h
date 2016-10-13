#ifndef LOAD_MODULE
#define LOAD_MODULE

#define REGISTER_MODULE(module_name) extern "C" {Module *mkrfn() {return module_name::Instance();}}

#endif 
