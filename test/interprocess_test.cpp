/* Interprocess test with real-time threading to simulate the NAO reading and writing vals */

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/permissions.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include "../include/memory/hal_data.h"
using namespace boost::interprocess;

#define ARRAY_1_SIZE NumOfActuatorIds
#define ARRAY_2_SIZE NumOfSensorIds

#define LOG(msg) std::cout << "[" << __func__ << "]: " << msg << std::endl;

//Specify your OS below (sorry mac users)
//Enable Real-Time thread for the process that initially creates the memory
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#elif __linux__
#include <pthread.h>
#endif
struct shm_object
{
	float test_array_a[ARRAY_1_SIZE];
	float test_array_b[ARRAY_2_SIZE];
    //char test_speak_array[35];
    //interprocess_semaphore semaphore_a, semaphore_b;

	shm_object() /*: semaphore_a(1), semaphore_b(1)*/
	{};
};

class shared_mem_controller
{
	
public:
	shared_mem_controller()
        :
            sem_a(open_or_create, "actuator_semaphore", 1, 0666),
            sem_b(open_or_create, "sensor_semaphore", 1, 0666)

	{
		std::cout << "shared_mem_controller constructed!" << std::endl;
	}
	~shared_mem_controller()
	{
		destroy_shared_memory();
		std::cout << "shared_mem_controller destructed!" << std::endl;
	}

	void test_write_a()
	{
		LOG("Starting test...");
		//s_mem_object->semaphore_a.wait();
		sem_a.wait();
        for (int i = 0; i < ARRAY_1_SIZE; ++i)
		{
			s_mem_object->test_array_a[i] = std::rand() % 100;
		}
        sem_a.post();
		//s_mem_object->semaphore_a.post();
		LOG("Done writing vals!");
	}
	void test_read_a()
	{
		LOG("Starting test...");
		//s_mem_object->semaphore_a.wait();
		sem_a.wait();
        for(int i = 0; i < ARRAY_1_SIZE; ++i)
		{
			auto val = s_mem_object->test_array_a[i];
			//std::string msg = "READING: test_array_a[" + std::to_string(i) + "]: = " + std::to_string(val);
            //std::string msg = "READING: " + std::to_string(*actuatorNames[i]) + " = " + std::to_string(val);
            //LOG(msg);
            std::cout << "ACTUATOR: " << actuatorNames[i] << " = " << val << '\n';
		}
        std::cout << std::endl;
		sem_a.post();
        //s_mem_object->semaphore_a.post();
		LOG("Done reading vals!");
	}
	void test_write_b()
	{
		LOG("Starting test...");
		//s_mem_object->semaphore_b.wait();
        sem_b.wait();
		for (int i = 0; i < ARRAY_2_SIZE; ++i)
		{
			s_mem_object->test_array_b[i] = std::rand() % 100;
		}
        sem_b.post();
		//s_mem_object->semaphore_b.post();
		LOG("Done writing vals!");
	}
	void test_read_b()
	{
		LOG("Starting test...");

		//s_mem_object->semaphore_b.wait();
		sem_b.wait();
        for (int i = 0; i < ARRAY_2_SIZE; ++i)
		{
			auto val = s_mem_object->test_array_b[i];
            //std::string msg = "READING: " + std::to_string(*sensorNames[i]) + " = " + std::to_string(val);
			//LOG(msg);
            std::cout << "SENSORS: " << sensorNames[i] << " = " << val << '\n';
		}
        std::cout << std::endl;
		sem_b.post();
        //s_mem_object->semaphore_b.post();
		LOG("Done reading vals!");
	}
	/*
	 * initialize_shared_memory: Creates the inteprocess mapped memory stuff
	 */
	void initialize_shared_memory()
	{
		LOG("test logger");
		try
		{
            //Create a permissions object to allow unrestricted access.
            //Be carefuly with this, as the way I'm doing it could pose
            //a significant security risk.
            std::cout << "Current permissions before perm change: " << mem_permissions.get_permissions() << std::endl;
		    mem_permissions.set_unrestricted();
            LOG("Permissions were set!");
            std::cout << "Current Permissions: " << mem_permissions.get_permissions() << std::endl;
            //Create our shared memory region
            //shm = managed_shared_memory(open_or_create, "PineappleJuice", 65536, 0, mem_permissions);
            shm = managed_shared_memory(open_or_create, "PineappleJuice", 65536, 0, mem_permissions/*mem_permissions*/);
            LOG("PineappleJuice created!");
			if (shm.get_size() < 65536)
			{
				std::cerr << "Error, could not allocate enough space!\n";
			}
			s_mem_object = shm.find_or_construct<shm_object>("juicyData")(/* Constructor doesn't take anything*/);
            shared_data_ptr = shm.find<shm_object>("juicyData");
			if (!shared_data_ptr.first)
			{
				std::cerr << "Could not find shared memory region juicyData!" << std::endl;
			}
			else
			{
				std::cout << "juicyData created in the mapped memory region!" << std::endl;
			}
            
             
		}
		catch (const boost::interprocess::interprocess_exception &e) {
			std::cerr << "Something bad happened: " << e.what() << std::endl;
			destroy_shared_memory();
		}
	}
	/*
	 * open_shared_memory: Trys to open already created shared memory
	 */
	void open_shared_memory()
	{
		
		try
		{
			LOG("Trying to open shared memory...");
			shm = managed_shared_memory(open_only, "PineappleJuice");
			if (shm.get_size() < 65536)
			{
				std::cerr << "Error, there is not enough space for PineappleJuice" << std::endl;
			}
			std::cout << "Size of PineappleJuice: " << shm.get_size() << std::endl;
			//shared_data_ptr = shm.find_no_lock<shm_object>("juicyData");
			shared_data_ptr = shm.find_no_lock<shm_object>("juicyData");
			if (!shared_data_ptr.first)
			{
				std::cerr << "Error! Could not find shared memory region juicyData!" << std::endl;
			}
			else
			{
				LOG("shared memory object juicyData found!");
				s_mem_object = shared_data_ptr.first;
			}
		}
		catch (const boost::interprocess::interprocess_exception &e) {
			std::cerr << "Error occurred while opening shared memory" << std::endl;
		}
		LOG("Done opening shared memory stuff!");
	}
	/*
	* destroy_shared_memory: Kills the inteprocess mapped memory stuff
	*/
	void destroy_shared_memory()
	{
		try
		{
			shm.destroy<shm_object>("juicyData");
			std::cout << "Juicy Data destroyed!" << std::endl;
			shared_memory_object::remove("PineappleJuice");
			std::cout << "PineappleJuice mapped region was destroyed!" << std::endl;
		}
		catch (const boost::interprocess::interprocess_exception &e) {
			std::cerr << "Failed to destroy shared memory regions: " << e.what() << std::endl;
		}
	}
	/*
	 * test_read_write: Tests interprocess reading and writing
	 */
	void test_read_write(std::size_t loop_delay_time)
	{
		try
		{
			while (true)
			{
				test_write_a();
				test_read_a();
				test_write_b();
				test_read_b();
				//Time between loops (This may negatively affect the Real-Time threading)
				std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay_time));
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "An exception occured while the process was running: " << e.what() << std::endl;
			destroy_shared_memory();
		}
	}
	void test_read_only(std::size_t loop_delay_time)
	{
		try
		{
			while (true)
			{
				test_read_a();
				test_read_b();
				//Time between loops (This may negatively affect the Real-Time threading)
				std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay_time));
			}
		}
		catch (const std::exception &e)
		{
			std::cerr << "An exception occured while the process was running: " << e.what() << std::endl;
			destroy_shared_memory();
		}
	}
private:
	shm_object *s_mem_object;
    permissions mem_permissions;
	managed_shared_memory shm;
	std::pair<shm_object *, std::size_t> shared_data_ptr;
    named_semaphore sem_a, sem_b;
    //named_semaphore *sem_a;//, sem_b;
};
int main(int argc, char **argv)
{
	shared_mem_controller test_process_1;

	if (std::atoi(argv[1]) == 0)
	{
		//Primary proces which runs in a real-time thread (simulating robotics behavior here ok...geez)
		LOG("PROCESS 1 Spawning...");
#ifdef _WIN32
		DWORD dwError, dwPriClass;

		if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		{
			dwError = GetLastError();
			_tprintf(TEXT("Failed to enter background mode (%d)\n"), dwError);
			return 0;
		}
		else {
			std::cout << "Woohoo probably real-time thread!" << std::endl;
		}
		dwPriClass = GetPriorityClass(GetCurrentProcess());
		_tprintf(TEXT("Current priority class is 0x%x\n"), dwPriClass);
		LOG("Process Running in a Real-Time Thread!");
#elif __linux__
		//Linux (soft)real-time thread (untested). Based off the NAO-Engine watchdog scheduler
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);
		if (sched_setscheduler(0, SCHED_FIFO, &param) == -1)
		{
			LOG("You need to be root to allow this thread to accidentally kill linux");
			return 1;
		}
#endif
		//1 second pause before we kick the speed up.
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//No inteprocess memory setup yet, so we need to create it otherwise segfaults.
		test_process_1.initialize_shared_memory();
		//10 Millisecond pause between runs to simulate the NAO's callback time for
		//real-time function calls.
		test_process_1.test_read_write(10);
	}
	else if (std::atoi(argv[1]) == 1)
	{
		LOG("PROCESS 2 Spawning...");
		//Open Shared Memory already created by the first process
		//test_process_1.open_shared_memory();
	    test_process_1.initialize_shared_memory();	
        //1 second pause between loops so you can read the data.
		test_process_1.test_read_write(1000);
	}
	else if (std::atoi(argv[1]) == 2)
	{
		//Clean up the interprocess memory for the events where the destructor wasn't called (this is just me being lazy)
		LOG("Attempting to destroy the shared mem object");
		test_process_1.open_shared_memory();
		test_process_1.destroy_shared_memory();
		LOG("Hopefully the shared memory is cleaned up now!");
	}
    else if(std::atoi(argv[1]) == 3)
    {
        test_process_1.open_shared_memory();
        test_process_1.test_read_only(1000);

    }
    else if(std::atoi(argv[1]) == 4)
    {
        LOG("Creating shared memory from normal permissios level");
        test_process_1.initialize_shared_memory();
        test_process_1.test_read_write(500);
    }
	else
	{
		std::cerr << "Invalid Arguments! Please enter [0,1,2]" << std::endl;
		return 1;
	}
#ifdef _WIN32
	//For debugging in visual studio
	system("pause");
#endif
	return 0;
}
