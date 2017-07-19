#include "include/util/hardwaremap.h"
//HardwareMap generator...brought to you by the power of Reflection.
//Constructor ensures we populate both maps from the start
HardwareMap::HardwareMap() 
{
    std::cout << "Constructing hardware map in hal_experimental" << std::endl;
    //Generate sensor map 
    for (size_t index = 0; index < BetterSensorIds::_values().size() - 1; ++index)
    {
        BetterSensorIds _sensor = BetterSensorIds::_values()[index];
       // std::cout << "Reflection: \n\t[enum integral val] " << _sensor._to_integral() <<"\n\t[enum name] " << _sensor << " \n\t[hardware path] " << sensorNames[_sensor] << std::endl; 
    
        _value v = std::make_pair<int, std::string>(_sensor, sensorNames[_sensor]);
        SensorMap.emplace(_sensor._to_string(), v);
    }
    std::cout << "Done initializing sensor map!\nInitializing Actuators Map" << std::endl;
    for (size_t index = 0; index < BetterActuatorIds::_values().size() - 1; ++index)
    {
        BetterActuatorIds _actuator = BetterActuatorIds::_values()[index];

        //std::cout << "Reflection: \n\t[enum integral val] " << _actuator._to_integral() <<"\n\t[enum name] " << _actuator << " \n\t[hardware path] " << actuatorNames[_actuator] << std::endl; 
       _value v = std::make_pair<int, std::string>(_actuator, actuatorNames[_actuator]);
        ActuatorMap.emplace(_actuator._to_string(), v);
    }
    null_val = std::make_pair<int,std::string>(0,"NULL");
}
//Default destructor, nothing special
HardwareMap::~HardwareMap() {}

//Overloaded subscript operator for accessing keys from either map.
//(This assumes each array contains unique items...I haven't verified this carefully)
const _value& HardwareMap::operator[](const std::string key)
{
    //Handle runtime exceptions of non-existent key lookup
    try
    {
        if(this->SensorMap[key].second != "")
            return this->SensorMap[key];
        else if(this->ActuatorMap[key].second != "")
            return this->ActuatorMap[key];
    } 
    catch(const std::exception &e)
    {
        std::cerr << "HardwareMap Lookup Error (probably non-existant value): " << e.what() << std::endl;
    }

    return null_val;

    /*
    if(!(this->SensorMap.find(key) == this->SensorMap.end()))
        return this->SensorMap[key];
    else if (!(this->ActuatorMap.find(key) == this->ActuatorMap.end()))
        return this->ActuatorMap[key];
    else
        return null_val;
    */
}
_value HardwareMap::get(const std::string &key)
{
    try
    {
        if(SensorMap[key].second != "")
            return SensorMap[key];
        else if(ActuatorMap[key].second != "")
            return ActuatorMap[key];
    } 
    catch(const std::exception &e)
    {
        std::cout << "HardwareMap Lookup Error (probably non-existant value): " << e.what() << std::endl;
    }

    return null_val;
}



const hashmap HardwareMap::getSensorMap()
{
    return SensorMap;
}
const hashmap HardwareMap::getActuatorMap()
{
    return ActuatorMap;
}


// int main() 
// {
//     std::cout << "hello world!" << std::endl;
//     HardwareMap hmap;
//     //std::unique_ptr<HardwareMap> hmap = make_unique<HardwareMap>();
//     std::cout << "sensor get test [lHandPositionSensor]: " << hmap.get("lHandPositionSensor").second << std::endl;

//     std::cout << "sensor get test [lHandPositionSensor]: " << hmap["lHandPositionSensor"].second << std::endl;
//     return 0;

// }
