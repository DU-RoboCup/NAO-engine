# LuaTables

LuaTables has the aim to make accessing of Lua values from C++ as simple as
possible and to use Lua scripts instead of .ini, or JSON files as a
configuration file format.

As a side effect LuaTables can also be used as an intuitive bridge to
access Lua values from a given Lua virtual machine.

# Usage

Navigation to a Lua value is done using std::map-like accessors. E.g. for a
Lua file:

    :::lua
    -- myfile.lua
    person = {
      name = "James",
      height = 1.87,
      address = {
        country = "United Kingdom"
      }
    }

    return person

the contents can be retrieved from C++ using LuaTables using:

    :::c++
    LuaTable ltable = LuaTable::fromFile("myfile.lua")

    std::string name = ltable["name"].get<std::string> ("Unknown");
    double height = ltable["id"].get<double> (-1.);
    std::string country = ltable["address"]["country"].get<std::string> ("Unknown");

the arguments to the function .get<type>(value) are default values that are
returned if the value could not be found.

# Serializing Custom Data Types

LuaTables++ can be easily extended to allow reading and writing of custom
data types. For this two functions have to be provided:

* ```LuaTableNode::getDefault<CustomType>(const CustomType &default_value)```
to read values from a LuaTable and 
* ```LuaTableNode::set<CustomType>(const CustomType &value)``` to serialize data into a LuaTable.

## Example:

Given the data struct:

    :::c++
    struct CustomType {
      string name;
      double age;
      bool drunken;
      double position[3];
    };

the two functions can be implemented as:

    :::c++
    template<> CustomType LuaTableNode::getDefault<CustomType>(const CustomType &default_value) {
      CustomType result = default_value;
    
      if (exists()) {
        LuaTable custom_table = stackQueryTable();
        
        result.name = custom_table["name"].get<std::string>();
        result.age = custom_table["age"];
        result.drunken = custom_table["drunken"];
        result.position[0] = custom_table["position"][1];
        result.position[1] = custom_table["position"][2];
        result.position[2] = custom_table["position"][3];
      }
    
      stackRestore();
    
      return result;
    }
    
    template<> void LuaTableNode::set<CustomType>(const CustomType &value) {
      LuaTable custom_table = stackCreateLuaTable();
    
      // set the fields of the custom type
      custom_table["name"] = value.name;
      custom_table["age"] = value.age;
      custom_table["drunken"] = value.drunken;
      custom_table["position"][1] = value.position[0];
      custom_table["position"][2] = value.position[1];
      custom_table["position"][3] = value.position[2];
    }

# Licensing

The library is published under the zlib free software license. Here is the
full license text:

    LuaTables++
    Copyright (c) 2013-2014 Martin Felis <martin@fyxs.org>.
    All rights reserved.
    
    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:
    
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
