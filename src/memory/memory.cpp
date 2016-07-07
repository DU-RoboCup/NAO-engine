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
-- Created by David Chan 7/7/16

// FILE DESCRIPTION
	Code for IMC

*/

#include "memory/memory.h"

Memory* Memory::instance;

// Get the instance of the singleton class
Memory* Memory::Instance() {
	if (UNLIKELY(!Memory::instance)) {
		instance = new Memory();
	}
	return instance;
}


// Construct a new memory
Memory::Memory() {
	this->memory_time = 1; // Set the memory time to one, as 0 means never modified
}

// Add back a KV-Pair
bool Memory::addKVPair(std::string key, std::shared_ptr<boost::any> data) {
	this->memory_time += 1;
	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to add memory key " << key << " with " << sizeof(d) << " bytes " << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) != this->mem.end()) {
		LOG_WARNING << "When adding to memory at memory time " << this->memory_time << ", key " << key << " already exists in the IMC map. Doing Nothing." ;
		return false;
	} else {

		//this->mem.emplace(std::make_pair(key, KVdata(data, this->memory_time)));
		this->mem[key] = KVdata(data, this->memory_time);
	}

	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Added memory key " << key << " with " << sizeof(d) << " bytes at memory time " << this->memory_time ;
	#endif

	return true;
}

// Get data from a KV-Pair
std::shared_ptr<boost::any> Memory::getKVPair(std::string key) {
	this->memory_time += 1;
	
	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to get memory key " << key << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) != this->mem.end()) {
		#ifdef DEBUG_LEVEL_7
			LOG_DEBUG << "Got memory key " << key ;
		#endif
		return this->mem[key].data;
	} else {
		LOG_WARNING << "When retrieving from memory, key " << key << " does not exist in the IMC map." ;
		return std::shared_ptr<boost::any>(new boost::any());
	}
}

// Remove a pair from memory
bool Memory::removeKVPair(std::string key) {
	this->memory_time += 1;
	
	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to remove memory key " << key << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) != this->mem.end()) {
		this->mem.erase(key);		
		#ifdef DEBUG_LEVEL_7
			LOG_DEBUG << "Removed memory key " << key ;
		#endif
		return true;
	} else {
		LOG_WARNING << "When retrieving from memory, key " << key << " does not exist in the IMC map." ;
		return false;
	}
}

// Add back a KV-Pair
bool Memory::updateKVPair(std::string key, std::shared_ptr<boost::any> data) {
	this->memory_time += 1;
	
	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to update memory key " << key << " with " << sizeof(d) << " bytes " << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) == this->mem.end()) {
		LOG_WARNING << "When adding to memory at memory time " << this->memory_time << ", key " << key << " does not exist in the IMC map. Adding." ;
		return this->addKVPair(key, data);
	} else {
		this->mem[key].data = data;
		this->mem[key].last_updated = this->memory_time;
		for (auto s = this->mem[key].subs.begin(); s != this->mem[key].subs.end(); s++) {
			#ifdef DEBUG_LEVEL_7
				LOG_DEBUG << "Notifying Module " << s.module->GetName() << " that key " << key << " has changed at memory time " << this->memory_time ;
			#endif

			Intent i((*s).second.intent);
			(*s).second.module->ProcessIntent(i);
		}
	}

	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Updated memory key " << key << " with " << sizeof(d) << " bytes at memory time " << this->memory_time ;
	#endif

	return true;
}

uint32_t Memory::addSubscriber(std::string key, Subscriber s) {

	this->memory_time += 1;

	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to add sub to key " << key << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) == this->mem.end()) { 
		LOG_WARNING << "Could not add subscriber to key " << key << " at memory time " << this->memory_time << " because the key does not exist in the IMC map." ;
		return false;
	} else {
		this->mem[key].sub_id += 1;
		this->mem[key].subs[this->mem[key].sub_id] = s;
	}

	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Added sub to key " << key << " at memory time " << this->memory_time ;
	#endif

	return this->mem[key].sub_id;
}


bool Memory::removeSubscriber(std::string key, uint32_t id) {
	this->memory_time += 1;

	#ifdef DEBUG_LEVEL_7
		LOG_DEBUG << "Trying to remove sub from key " << key << " at memory time " << this->memory_time ;
	#endif

	if (this->mem.find(key) == this->mem.end()) { 
		LOG_WARNING << "Could not add subscriber to key " << key << " at memory time " << this->memory_time << " because the key does not exist in the IMC map." ;
		return false;
	} else {
		if(this->mem[key].subs.erase(id)) {
			#ifdef DEBUG_LEVEL_7
				LOG_DEBUG << "Removed sub from key " << key << " at memory time " << this->memory_time ;
			#endif
			return true;
		} else {
			LOG_WARNING << "Could not remove subscriber from key " << key << " at memory time " << this->memory_time << " because the subscriber is not subscribed." ;
			return false;
		}
	}
}

//-------------------------------------------//

bool Bazaar::Vend(std::string key, std::shared_ptr<boost::any> data) {
	return Memory::Instance()->addKVPair(key, data);
}

bool Bazaar::UpdateListing(std::string key, std::shared_ptr<boost::any> data) {
	return Memory::Instance()->updateKVPair(key, data);
}

std::shared_ptr<boost::any> Bazaar::Get(std::string key) {
	return Memory::Instance()->getKVPair(key);
}


bool Bazaar::AskRemove(std::string key) {
	// Just removes right now
	return Memory::Instance()->removeKVPair(key);
}


bool Bazaar::ForceRemove(std::string key) {
	// Just removes right now
	return Memory::Instance()->removeKVPair(key);
}

uint32_t Bazaar::Subscribe(std::string key, Module* module, std::string intent) {
	Subscriber s(intent, module);
	return Memory::Instance()->addSubscriber(key, s);
}

bool Bazaar::Unsubscribe(std::string key, uint32_t sub_id) {
	return Memory::Instance()->removeSubscriber(key, sub_id);
}



