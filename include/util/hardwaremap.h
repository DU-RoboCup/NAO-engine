#ifndef _HARDWARE_MAP_H
#define _HARDWARE_MAP_H

#include <boost/unordered_map.hpp>
#include <iostream>
#include <string>
#include <utility>
//#include "more_hardware_data.h"
#include "include/memory/hal_data.h"
#include <memory>
typedef std::pair<int, std::string> _value;
typedef boost::unordered_map<std::string, _value> hashmap;

class HardwareMap
{

public:

    HardwareMap();
    ~HardwareMap();
    const _value &operator[](const std::string key);
	_value get(const std::string &key);
    const hashmap getSensorMap();
    const hashmap getActuatorMap();

private:
    hashmap SensorMap;
    hashmap ActuatorMap;
    _value null_val;
};


#endif