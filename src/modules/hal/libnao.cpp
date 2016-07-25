/*
Copyright (c) 2016 "University of Denver"

This file is part of Pineapple.

Pineapple is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

VERSION HISTORY
-- Created by Paul Heinen 7/22/16

// FILE DESCRIPTION

LibNao is created on startup by the pineapple engine. It uses the
Aldebaran API's to directly write to and read from the DCM/HAL
as well as read and send information to the libpineapple module via
a the 'PineappleJuice' shared memory block found in the 'Fridge'.

Parts of this code are borrowed from the B-Human 2015 Release.

*/

// Includes
#include "include/modules/hal/libnao.h";

using namespace boost::interprocess;

Nao::Nao(boost::shared_ptr<AL::Broker> pBroker) :
    AL::Module(pBroker, "Pineapple"),
    shm(open_or_create, "Fridge", 65536),
    proxy(0), memory(0), dcmTime(0),
    lastReadingActuators(-1),
    actuatorDrops(0),
    frameDrops(allowedFrameDrops + 1),
    state(sitting),
    phase(0.f),
    ledIndex(0),
    rightEarLEDsChangedTime(0),
    startPressedTime(0),
    lastPineappleStartTime(0)
{


    //setModuleDescription("Forms a parasitic relationship with a Pineapple.");

    LOG_DEBUG << "libnao is starting up...Running some quick checks";

    assert(lpNumOfSensorIds == sizeof(sensorNames) / sizeof(*sensorNames));
    assert(lpNumOfActuatorIds == sizeof(actuatorNames) / sizeof(*actuatorNames));
    assert(lpNumOfTeamInfoIds == sizeof(teamInfoNames) / sizeof(*teamInfoNames));

    LOG_DEBUG << "All tests passed. Creating shared memory block 'PineappleJuice'";
    LPData_Buffer *pineappleJuice = shm.construct<LPData_Buffer>("PineappleJuice")();

    LOG_DEBUG << "Shared memory block PineappleJuice was created.";
    LOG_DEBUG << "Attempting to negotiate with naoqi...";

    try {

        memory = new AL::ALMemoryProxy(pBroker); //Gets the robots name
        LOG_INFO << "Setting BodyId";
        std::string bodyId = (std::string) memory->getData("Device/DeviceList/ChestBoard/BodyId", 0);
        strncpy(pineappleJuice->bodyId, bodyId.c_str(), sizeof(pineappleJuice->bodyId));
        pineappleJuice->bodyId[15] = 0;
        LOG_INFO << "BodyId Set";

        LOG_INFO << "Setting Head ID";
        std::string headId = (std::string) memory->getData("RobotConfig/Head/FullHeadId", 0);
        strncpy(pineappleJuice->headId, headId.c_str(), sizeof(pineappleJuice->headId));
        pineappleJuice->headId[15] = 0;
        LOG_INFO << "HeadId set";

        //Create aliases for stiffnessRequest and positionRequest
        proxy = new AL::DCMProxy(pBroker);

        AL::ALValue params, result;
        params.arraySetSize(1);
        params.arraySetSize(2);

        LOG_INFO << "Setting params values for positionActuators alias";
        params[0] = std::string("positionActuators");
        params[1].arraySetSize(lpNumOfPositionActuatorIds);
        for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
            params[1][i] = std::string(actuatorNames[i]);
        result = proxy->createAlias(params);

        LOG_INFO << "Setting params values for stiffnessActuators alias";
        params[0] = std::string("stiffnessActuators");
        params[1].arraySetSize(lpNumOfStiffnessActuatorIds);
        for(int i = 0; i < lpNumOfStiffnessActuatorIds; ++i)
            params[1][i] = std::string(actuatorNames[headYawStiffnessActuator + i]);
        result = proxy->createAlias(params);

        LOG_INFO << "Setting usRequest actuators";
        params[0] = std::string("usRequest");
        params[1].arraySetSize(1);
        params[1][0] = std::string(actuatorNames[usActuator]);
        result = proxy->createAlias(params);

        LOG_INFO << "preparing positionRequest actuators";
        // prepare positionRequest
        positionRequest.arraySetSize(6);
        positionRequest[0] = std::string("positionActuators");
        positionRequest[1] = std::string("ClearAll");
        positionRequest[2] = std::string("time-separate");
        positionRequest[3] = 0;
        positionRequest[4].arraySetSize(1);
        positionRequest[5].arraySetSize(lpNumOfPositionActuatorIds);
        for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
          positionRequest[5][i].arraySetSize(1);

        LOG_INFO << "Preparing stiffnessRequest";
        // prepare stiffnessRequest
        stiffnessRequest.arraySetSize(6);
        stiffnessRequest[0] = std::string("stiffnessActuators");
        stiffnessRequest[1] = std::string("ClearAll");
        stiffnessRequest[2] = std::string("time-separate");
        stiffnessRequest[3] = 0;
        stiffnessRequest[4].arraySetSize(1);
        stiffnessRequest[5].arraySetSize(lpNumOfStiffnessActuatorIds);
        for(int i = 0; i < lpNumOfStiffnessActuatorIds; ++i)
          stiffnessRequest[5][i].arraySetSize(1);

        LOG_INFO << "Preparing usRequest";
        // prepare usRequest
        usRequest.arraySetSize(6);
        usRequest[0] = std::string("usRequest");
        usRequest[1] = std::string("Merge"); // doesn't work with "ClearAll"
        usRequest[2] = std::string("time-separate");
        usRequest[3] = 0;
        usRequest[4].arraySetSize(1);
        usRequest[5].arraySetSize(1);
        usRequest[5][0].arraySetSize(1);

        LOG_INFO << "Preparing ledRequest";
        // prepare ledRequest
        ledRequest.arraySetSize(3);
        ledRequest[1] = std::string("ClearAll");
        ledRequest[2].arraySetSize(1);
        ledRequest[2][0].arraySetSize(2);
        ledRequest[2][0][1] = 0;

        LOG_INFO << "Preparing sensor pointers";
        // prepare sensor pointers
        for(int i = 0; i < lpNumOfSensorIds; ++i)
          sensorPtrs[i] = (float*) memory->getDataPtr(sensorNames[i]);
        resetUsMeasurements();

        LOG_INFO << "Initializing requested actuators";
        // initialize requested actuators
        memset(requestedActuators, 0, sizeof(requestedActuators));
        for(int i = faceLedRedLeft0DegActuator; i < chestBoardLedRedActuator; ++i)
          requestedActuators[i] = -1.f;

        LOG_INFO << "Registering onPreProcess and onPostProcess callbacks";
        // register "onPreProcess" and "onPostProcess" callbacks
        instance = this;
        proxy->getGenericProxy()->getModule()->atPreProcess(&onPreProcess);
        proxy->getGenericProxy()->getModule()->atPostProcess(&onPostProcess);

        LOG_DEBUG << "libpineapple successfully started!";
        return; // success
      } catch(AL::ALError& e) {
        LOG_FATAL << "libpineapple failed to start because: " << e.what();
      }
}
//}
//close(); // error
//}

//}
Nao::~Nao()
{
    LOG_WARNING << "libnao is shutting down";
    close();
}

// Naoqi module stuff
Nao *Nao::instance = 0;
extern "C" int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
{
  AL::ALModule::createModule<Nao>(pBroker);
  return 0;
}
