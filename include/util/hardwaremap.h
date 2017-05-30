/*
TODO: Turn this into a class

#pragma once
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <utility> //for tuples
#include <iostream>
#include <boost/unordered_map.hpp>
#include "more_hardware_data.h"
struct HardwareMap 
{
    typedef std::pair<int, std::string> _value;
    typedef boost::unordered_map<std::string, _value> hashmap;
    //HardwareMap generator...brought to you by the power of Reflection.
    //Constructor ensures we populate both maps from the start
    HardwareMap() 
    {
        std::cout << "Constructing hardware map in hal_experimental" << std::endl;
        //Generate sensor map 

        for (size_t index = 0; index < BetterSensorIds::_values().size(); ++index)
        {
            BetterSensorIds _sensor = BetterSensorIds::_values()[index];
            _value v = std::make_pair<int, std::string>(_sensor, sensorNames[_sensor]);
            SensorMap.emplace(_sensor._to_string(), v);
        }
        std::cout << "Done initializing sensor map!\nInitializing Actuators Map" << std::endl;
        for (size_t index = 0; index < BetterActuatorIds::_values().size(); ++index)
        {
            BetterActuatorIds _actuator = BetterActuatorIds::_values()[index];
            _value v = std::make_pair<int, std::string>(_actuator, sensorNames[_actuator]);
            ActuatorMap.emplace(_actuator._to_string(), v);
        }
    }
    //Default destructor, nothing special
    ~HardwareMap() {}

    //Overloaded subscript operator for accessing keys from either map
     const _value& operator[](const std::string key)
    {
        //Handle runtime exceptions of non-existent key lookup
        //TODO: This can be optimized to constant time lookups
        if(!(this->SensorMap.find(key) == this->SensorMap.end()))
            return this->SensorMap[key];
        else if (!(this->ActuatorMap.find(key) == this->ActuatorMap.end()))
            return this->ActuatorMap[key];
        else
        {
            _value null_val = std::make_pair<int,std::string>(0,"NULL");
            return null_val;
        }
    }
    const hashmap getSensorMap()
    {
        return SensorMap;
    }
    const hashmap getActuatorMap()
    {
        return ActuatorMap;
    }


    private:
        hashmap SensorMap;
        hashmap ActuatorMap;
};
*/