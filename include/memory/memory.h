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

This file deals with the inter-module communication, in what I think 
is a relatively nice manner. It also holds a shared memory repository
for other modules

*/

#ifndef _MEMORY_h_GUARD_
#define _MEMORY_h_GUARD_

// INCLUDES
#include <boost/any.hpp> 		// Boost ANY for payload storage
#include <map> 			 		// Map for STL Map (KV-pair storage)
#include <memory>				// for shared pointer
#include "common.h"				// For common includes
#include "memory/intent.h"
#include "modules/module.h" 	// For modules and module information


// A simple subscriber module, keeps track of the module to
// give the intent to, and the intent to give
struct Subscriber {
	Subscriber() : intent(""), module(0) {}
	Subscriber(std::string i, Module* m) : intent(i), module(m) {}
	std::string intent;
	Module* module;

	bool operator==(const Subscriber& rhs){ return rhs.intent == intent && rhs.module == module; }
};

// A data element - has a list of subscribers, and a 
// set of operations
struct KVdata {
	KVdata() : data(new boost::any()), pending_removal(false), last_updated(0), sub_id(0) {}
	KVdata(std::shared_ptr<boost::any> d) : data(d), pending_removal(false), last_updated(0), sub_id(0) {}
	KVdata(std::shared_ptr<boost::any> d, uint64_t time) : data(d), pending_removal(false), last_updated(time), sub_id(0) {}
	std::shared_ptr<boost::any> data;
	
	bool pending_removal;
	uint64_t last_updated;
	uint32_t sub_id;
	std::map<uint32_t, Subscriber> subs;
};

// This is a singleton class
class Memory {
	public:
		static Memory* Instance();

		bool addKVPair(std::string key, std::shared_ptr<boost::any> data);
		bool updateKVPair(std::string key, std::shared_ptr<boost::any> data);
		bool removeKVPair(std::string key);
		bool touchKVPair(std::string key);
		
		std::shared_ptr<boost::any> getKVPair(std::string key);

		uint32_t addSubscriber(std::string key, Subscriber s);
		bool removeSubscriber(std::string key, uint32_t id);
		
	private:
		Memory();

		static Memory* instance;
		std::map<std::string,KVdata> mem;
		uint64_t memory_time;
};

// Namespace for handling the memory interface
namespace Bazaar {

	// Add a KV-pair, returns an ID which can be used to update this key.
	bool Vend(std::string key, std::shared_ptr<boost::any> data);
	bool Touch(std::string key);
	bool UpdateListing(std::string key, std::shared_ptr<boost::any> data);
	
	// Get a KV-pair
	std::shared_ptr<boost::any> Get(std::string key);

	// Ask for the removal of a kv-pair
	bool AskRemove(std::string key);

	// Force the removal of a key - NOT SAFE
	bool ForceRemove(std::string key);

	// Subscription to KV-Pair changes
	uint32_t Subscribe(std::string key, Module* module, std::string intent);
	bool Unsubscribe(std::string key, uint32_t sub_id);
}



#endif /*_MEMORY_h_GUARD_*/
