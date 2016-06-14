#include <assert.h>
#include <iostream>
#include <algorithm>
#include <string>

#include "lib/luatables/luatables.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

int main() 
{
LuaTable ltable = LuaTable::fromFile("test.lua");
std::string name = ltable["name"].get<std::string> ();
double height = ltable["height"].get<double> ();
std::string country = ltable["address"]["country"].get<std::string> ();
std::cout << "Name: " << name << " Height: " << height << " Country: " << country<< std::endl;
}
