#include "include/modules/hal/libpineapple.h"

using namespace boost::interprocess;

////Borrow from the B-Human 2015 code release
static const char* sensorNames[] =
{
  // joint sensors
  "Device/SubDeviceList/HeadYaw/Position/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Position/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/HeadPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/Position/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LWristYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHand/Position/Sensor/Value",
  "Device/SubDeviceList/LHand/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHand/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RShoulderRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RElbowRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/Position/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RWristYaw/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHand/Position/Sensor/Value",
  "Device/SubDeviceList/RHand/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHand/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipYawPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/LAnkleRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipRoll/Temperature/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Position/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RHipPitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RKneePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Position/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnklePitch/Temperature/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/ElectricCurrent/Sensor/Value",
  "Device/SubDeviceList/RAnkleRoll/Temperature/Sensor/Value",

  // touch sensors
  "Device/SubDeviceList/Head/Touch/Front/Sensor/Value",
  "Device/SubDeviceList/Head/Touch/Middle/Sensor/Value",
  "Device/SubDeviceList/Head/Touch/Rear/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Back/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Left/Sensor/Value",
  "Device/SubDeviceList/LHand/Touch/Right/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Back/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Left/Sensor/Value",
  "Device/SubDeviceList/RHand/Touch/Right/Sensor/Value",

  // switches
  "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value",
  "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value",
  "Device/SubDeviceList/ChestBoard/Button/Sensor/Value",

  // inertial sensors
  "Device/SubDeviceList/InertialSensor/GyroscopeX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyroscopeY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/GyroscopeZ/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerY/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AccelerometerZ/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value",
  "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value",

  // sonar sensors
  "Device/SubDeviceList/US/Left/Sensor/Value",
  "Device/SubDeviceList/US/Left/Sensor/Value1",
  "Device/SubDeviceList/US/Left/Sensor/Value2",
  "Device/SubDeviceList/US/Left/Sensor/Value3",
  "Device/SubDeviceList/US/Left/Sensor/Value4",
  "Device/SubDeviceList/US/Left/Sensor/Value5",
  "Device/SubDeviceList/US/Left/Sensor/Value6",
  "Device/SubDeviceList/US/Left/Sensor/Value7",
  "Device/SubDeviceList/US/Left/Sensor/Value8",
  "Device/SubDeviceList/US/Left/Sensor/Value9",
  "Device/SubDeviceList/US/Right/Sensor/Value",
  "Device/SubDeviceList/US/Right/Sensor/Value1",
  "Device/SubDeviceList/US/Right/Sensor/Value2",
  "Device/SubDeviceList/US/Right/Sensor/Value3",
  "Device/SubDeviceList/US/Right/Sensor/Value4",
  "Device/SubDeviceList/US/Right/Sensor/Value5",
  "Device/SubDeviceList/US/Right/Sensor/Value6",
  "Device/SubDeviceList/US/Right/Sensor/Value7",
  "Device/SubDeviceList/US/Right/Sensor/Value8",
  "Device/SubDeviceList/US/Right/Sensor/Value9",

  // battery sensors
  "Device/SubDeviceList/Battery/Current/Sensor/Value",
  "Device/SubDeviceList/Battery/Charge/Sensor/Value",
  "Device/SubDeviceList/Battery/Temperature/Sensor/Value",

  // fsr sensors
  "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value",
  "Device/SubDeviceList/LFoot/FSR/TotalWeight/Sensor/Value",
  "Device/SubDeviceList/RFoot/FSR/TotalWeight/Sensor/Value",
};

static const char* actuatorNames[] =
{
  "HeadYaw/Position/Actuator/Value",
  "HeadPitch/Position/Actuator/Value",
  "LShoulderPitch/Position/Actuator/Value",
  "LShoulderRoll/Position/Actuator/Value",
  "LElbowYaw/Position/Actuator/Value",
  "LElbowRoll/Position/Actuator/Value",
  "LWristYaw/Position/Actuator/Value",
  "LHand/Position/Actuator/Value",
  "RShoulderPitch/Position/Actuator/Value",
  "RShoulderRoll/Position/Actuator/Value",
  "RElbowYaw/Position/Actuator/Value",
  "RElbowRoll/Position/Actuator/Value",
  "RWristYaw/Position/Actuator/Value",
  "RHand/Position/Actuator/Value",
  "LHipYawPitch/Position/Actuator/Value",
  "LHipRoll/Position/Actuator/Value",
  "LHipPitch/Position/Actuator/Value",
  "LKneePitch/Position/Actuator/Value",
  "LAnklePitch/Position/Actuator/Value",
  "LAnkleRoll/Position/Actuator/Value",
  "RHipRoll/Position/Actuator/Value",
  "RHipPitch/Position/Actuator/Value",
  "RKneePitch/Position/Actuator/Value",
  "RAnklePitch/Position/Actuator/Value",
  "RAnkleRoll/Position/Actuator/Value",

  "HeadYaw/Hardness/Actuator/Value",
  "HeadPitch/Hardness/Actuator/Value",
  "LShoulderPitch/Hardness/Actuator/Value",
  "LShoulderRoll/Hardness/Actuator/Value",
  "LElbowYaw/Hardness/Actuator/Value",
  "LElbowRoll/Hardness/Actuator/Value",
  "LWristYaw/Hardness/Actuator/Value",
  "LHand/Hardness/Actuator/Value",
  "RShoulderPitch/Hardness/Actuator/Value",
  "RShoulderRoll/Hardness/Actuator/Value",
  "RElbowYaw/Hardness/Actuator/Value",
  "RElbowRoll/Hardness/Actuator/Value",
  "RWristYaw/Hardness/Actuator/Value",
  "RHand/Hardness/Actuator/Value",
  "LHipYawPitch/Hardness/Actuator/Value",
  "LHipRoll/Hardness/Actuator/Value",
  "LHipPitch/Hardness/Actuator/Value",
  "LKneePitch/Hardness/Actuator/Value",
  "LAnklePitch/Hardness/Actuator/Value",
  "LAnkleRoll/Hardness/Actuator/Value",
  "RHipRoll/Hardness/Actuator/Value",
  "RHipPitch/Hardness/Actuator/Value",
  "RKneePitch/Hardness/Actuator/Value",
  "RAnklePitch/Hardness/Actuator/Value",
  "RAnkleRoll/Hardness/Actuator/Value",

  "Face/Led/Red/Left/0Deg/Actuator/Value",
  "Face/Led/Red/Left/45Deg/Actuator/Value",
  "Face/Led/Red/Left/90Deg/Actuator/Value",
  "Face/Led/Red/Left/135Deg/Actuator/Value",
  "Face/Led/Red/Left/180Deg/Actuator/Value",
  "Face/Led/Red/Left/225Deg/Actuator/Value",
  "Face/Led/Red/Left/270Deg/Actuator/Value",
  "Face/Led/Red/Left/315Deg/Actuator/Value",
  "Face/Led/Green/Left/0Deg/Actuator/Value",
  "Face/Led/Green/Left/45Deg/Actuator/Value",
  "Face/Led/Green/Left/90Deg/Actuator/Value",
  "Face/Led/Green/Left/135Deg/Actuator/Value",
  "Face/Led/Green/Left/180Deg/Actuator/Value",
  "Face/Led/Green/Left/225Deg/Actuator/Value",
  "Face/Led/Green/Left/270Deg/Actuator/Value",
  "Face/Led/Green/Left/315Deg/Actuator/Value",
  "Face/Led/Blue/Left/0Deg/Actuator/Value",
  "Face/Led/Blue/Left/45Deg/Actuator/Value",
  "Face/Led/Blue/Left/90Deg/Actuator/Value",
  "Face/Led/Blue/Left/135Deg/Actuator/Value",
  "Face/Led/Blue/Left/180Deg/Actuator/Value",
  "Face/Led/Blue/Left/225Deg/Actuator/Value",
  "Face/Led/Blue/Left/270Deg/Actuator/Value",
  "Face/Led/Blue/Left/315Deg/Actuator/Value",
  "Face/Led/Red/Right/0Deg/Actuator/Value",
  "Face/Led/Red/Right/45Deg/Actuator/Value",
  "Face/Led/Red/Right/90Deg/Actuator/Value",
  "Face/Led/Red/Right/135Deg/Actuator/Value",
  "Face/Led/Red/Right/180Deg/Actuator/Value",
  "Face/Led/Red/Right/225Deg/Actuator/Value",
  "Face/Led/Red/Right/270Deg/Actuator/Value",
  "Face/Led/Red/Right/315Deg/Actuator/Value",
  "Face/Led/Green/Right/0Deg/Actuator/Value",
  "Face/Led/Green/Right/45Deg/Actuator/Value",
  "Face/Led/Green/Right/90Deg/Actuator/Value",
  "Face/Led/Green/Right/135Deg/Actuator/Value",
  "Face/Led/Green/Right/180Deg/Actuator/Value",
  "Face/Led/Green/Right/225Deg/Actuator/Value",
  "Face/Led/Green/Right/270Deg/Actuator/Value",
  "Face/Led/Green/Right/315Deg/Actuator/Value",
  "Face/Led/Blue/Right/0Deg/Actuator/Value",
  "Face/Led/Blue/Right/45Deg/Actuator/Value",
  "Face/Led/Blue/Right/90Deg/Actuator/Value",
  "Face/Led/Blue/Right/135Deg/Actuator/Value",
  "Face/Led/Blue/Right/180Deg/Actuator/Value",
  "Face/Led/Blue/Right/225Deg/Actuator/Value",
  "Face/Led/Blue/Right/270Deg/Actuator/Value",
  "Face/Led/Blue/Right/315Deg/Actuator/Value",
  "Ears/Led/Left/36Deg/Actuator/Value",
  "Ears/Led/Left/72Deg/Actuator/Value",
  "Ears/Led/Left/108Deg/Actuator/Value",
  "Ears/Led/Left/144Deg/Actuator/Value",
  "Ears/Led/Left/180Deg/Actuator/Value",
  "Ears/Led/Left/216Deg/Actuator/Value",
  "Ears/Led/Left/252Deg/Actuator/Value",
  "Ears/Led/Left/288Deg/Actuator/Value",
  "Ears/Led/Left/324Deg/Actuator/Value",
  "Ears/Led/Left/0Deg/Actuator/Value",
  "Ears/Led/Right/0Deg/Actuator/Value",
  "Ears/Led/Right/36Deg/Actuator/Value",
  "Ears/Led/Right/72Deg/Actuator/Value",
  "Ears/Led/Right/108Deg/Actuator/Value",
  "Ears/Led/Right/144Deg/Actuator/Value",
  "Ears/Led/Right/180Deg/Actuator/Value",
  "Ears/Led/Right/216Deg/Actuator/Value",
  "Ears/Led/Right/252Deg/Actuator/Value",
  "Ears/Led/Right/288Deg/Actuator/Value",
  "Ears/Led/Right/324Deg/Actuator/Value",
  "ChestBoard/Led/Red/Actuator/Value",
  "ChestBoard/Led/Green/Actuator/Value",
  "ChestBoard/Led/Blue/Actuator/Value",
  "Head/Led/Rear/Left/0/Actuator/Value",
  "Head/Led/Rear/Left/1/Actuator/Value",
  "Head/Led/Rear/Left/2/Actuator/Value",
  "Head/Led/Rear/Right/0/Actuator/Value",
  "Head/Led/Rear/Right/1/Actuator/Value",
  "Head/Led/Rear/Right/2/Actuator/Value",
  "Head/Led/Middle/Right/0/Actuator/Value",
  "Head/Led/Front/Right/0/Actuator/Value",
  "Head/Led/Front/Right/1/Actuator/Value",
  "Head/Led/Front/Left/0/Actuator/Value",
  "Head/Led/Front/Left/1/Actuator/Value",
  "Head/Led/Middle/Left/0/Actuator/Value",
  "LFoot/Led/Red/Actuator/Value",
  "LFoot/Led/Green/Actuator/Value",
  "LFoot/Led/Blue/Actuator/Value",
  "RFoot/Led/Red/Actuator/Value",
  "RFoot/Led/Green/Actuator/Value",
  "RFoot/Led/Blue/Actuator/Value",

  "US/Actuator/Value"
};

static const char* teamInfoNames[] =
{
  "GameCtrl/teamNumber",
  "GameCtrl/teamColour",
  "GameCtrl/playerNumber"
};

static const float sitDownAngles[25] =
{
  0.f,
  0.f,

  0.89f,
  0.06f,
  0.26f,
  -0.62f,
  -1.57f,
  0.f,

  0.89f,
  -0.06f,
  -0.26f,
  0.62f,
  1.57f,
  0.f,

  0.f,
  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f,

  0.f,
  -0.87f,
  2.16f,
  -1.18f,
  0.f
};


LibPineapple::LibPineapple() :
    shm(create_only, "PineappleJuice", read_write),
    memregion(shm, read_write)
{
    LPData_Buffer *juicyData = shm.construct<LPData>("JuicyData")();
}

void Pineapple::shutdown()
{

    BOOST_LOG_FUNCTION();
    LOG_FATAL << "libpineapple shutting down";

    if(dcmproxy)
    {
        dcmproxy->GetGenericProxy()->getModule()->removeAllPreProcess();
        dcmproxy->GetGenericProxy()->getModule()->removeAllPostProcess();
        delete dcmproxy;
    }

    if(memoryproxy)
        delete memoryproxy;

    //TODO: Close semaphore and deal with shared mapped memory

    LOG_FATAL << "libpineapple has shut down";
}

void Pineapple::setEyeLEDS(float *actuators)
{
    for(int i = faceLedRedLeft0DegActuator; i <= faceLedBlueRight315DegActuator; ++i)
        actuators[i] = 0.f;

    if(state == shuttingDown)
    {
        actuators[faceLedRedLeft180DegActuator] = 1.f;
        actuators[faceLedRedRight180DegActuator] = 1.f;
    } else if (juicyData->state != okState /*TODO: Implement shared memory lookup here*/) {

        float error_blink = float(dcmTime / 500 & 1);
        for(int i = faceLedRedLeft0DegActuator; i <= faceLedRedLeft315DegActuator; ++i)
            actuators[i] = error_blink;
        for(int i = faceLedRedRight0DegActuator; i < faceLedRedRight315DegActuator; ++i)
            actuators[i] = error_blink;
    } else {
        //Normal running mode
        float okstate_blink = float(dcmTime / 500 & 1);
        actuators[faceLedBlueLeft180DegActuator] = okstate_blink;
        actuators[faceLedBlueRight180DegActuator] = okstate_blink;
    }
}

void Pineapple::setBatteryStatusLEDS(float *actuators)
{
    for(int i = earsLedRight0DegActuator; i < earsLedRight324DegActuator; ++i)
    {
        if(actuators[i] != requestedActuators[i])
        {
            rightEarLEDsChangedTime = dcmTime;
            requestedActuators[i] = actuators[i];
        }

        if(state != standing || dcmTime - rightEarLEDsChangedTime > 5000)
        {
            for(int i = 0; i < int(*sensorPtrs[batteryChargeSensor] * 10.f) && i < 10; ++i)
                actuators[earsLedLeft0DegActuator + i] = 1.f;
        }
    }
}

float* Pineapple::state_handler(float *actuators)
{
    static float controlledActuators[lpNumOfActuatorIds];

    switch(state)
    {
        sitting: state = sitting;
        case sitting:
            std::memset(controlledActuators, 0, sizeof(controlledActuators));
            if(frameDrops > allowedFrameDrops || (actuators[lHipPitchStiffnessActuator] == 0.f && actuators[rHipPitchStiffnessActuator] == 0.f))
                return controlledActuators;
            for(int i = 0; i < lpNumOfActuatorIds; ++i)
                startAngles[i] = *sensorPtrs[i * 3];

        standing:
            state = standingUp;
            phase = 0.f;
        case standingUp:
            if(phase < 1.f && frameDrops <= allowedFrameDrops)
            {
                std::memset(controlledActuators, 0, sizeof(controlledActuators));
                phase = std::min(phase + 0.005f, 1.f);
                for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                {
                    controlledActuators[i] = actuators[i] * phase + startAngles[i] * (1.f - phase);
                    float h = std::min(actuators[i + headYawStiffnessActuator], 0.5f);
                    controlledActuators[i + headYawStiffnessActuator] = actuators[i + headYawStiffnessActuator] * phase + h * (1.f - phase);
                }
                return controlledActuators;
            }
            state = standing;


        case standing: if(frameDrops <= allowedFrameDrops) return actuators;

        case preShutDown:
            for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
            {
                startAngles[i] = positionRequest[5][i][0];
                if(actuators[lHipPitchStiffnessAcutator] = 0.f && actuators[rHipPitchStiffnessActuator] == 0.f)
                    startingStiffness[i] = 0.f;
                else if(i >= lShoulderPitchPositionActuator && i <= rElbowRollPositionActuator)
                    startStiffness[i] = 0.4f;
                else
                    startStiffness[i] = std::min<float>(stiffnessRequest[5][i][0], 0.3f);
            }
            state = state == preShutDown ? shuttingDown : sittingDown;
            phase = 0.f;

        case shuttingDown:
            shuttingDown:
                if((phase < 1.f && frameDrops > allowedFrameDrops) || state == shuttingDown)
                {
                    std::memset(controlledActuators, 0, sizeof(controlledActuators));
                    phase =  std::min(phase + 0.005f, 1.f);
                    for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                    {
                        controlledActuators[i] = sitDownAngles[i] * phase + startAngles[i] * (1.f - phase);
                        controlledActuators[i + headYawStiffnessActuator] = startStiffness[i];
                    }
                    return controlledActuators;
                } else if(frameDrops <= allowedFrameDrops) {
                    for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                        startAngles[i] = positionRequest[5][i][0];
                    goto standingUp;
                } else {
                    goto sitting;
                }
        case preSittingShutDown:
            for(int i = 0; i < lpNumOfPositionActuatorIds; ++i)
                startStiffness[i] = 0.f;
            state = shuttingDown;
            phase = 0.995f;
            goto shuttingDown;
    }
}

void LibPineapple::setActuators()
{
#ifdef DEBUG
    BOOST_LOG_FUNCTION();
    LOG_DEBUG << "Trying to set actuator values";
#endif
    try {
        //Set actuators to values stored in the shared memory
        dcmTime = proxy->getTime(0);

        juicyData->readingActuators = juicyData->newestActuators;
        LOG_TRACE << "readingActuators values updated";

        if(juicyData->readingActuators == lastReadingActuators)
        {
            if(actuatorDrops == 0)
                LOG_FATAL << "Actuator request was missed";
            ++actuatorDrops;
        } else {
            actuatorDrops = 0;
        }
        lastReadingActuators = juicyData->readingActuators;
        float *readingActuators = juicyData->readingActuators;
        float *actuators = handleState(readingActuators);


        if(state != standing)
        {
            if(frameDrops > 0 || state == shuttingDown)
                setEyeLEDS(actuators);
            else
                copyServoData(readingActuators, actuators);
        }
        setBatteryLeds(actuators);




}

