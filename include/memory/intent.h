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
-- Modified by Paul Heinen 3/8/17

// FILE DESCRIPTION

This file deals with the inter-module communication, in what I think 
is a relatively nice manner. It also holds a shared memory repository
for other modules

*/

#ifndef _INTENT_H_
#define _INTENT_H_

#include <string>
#include <vector>
#include <deque>
#include <stdexcept>
#include <boost/algorithm/string.hpp>


//typedef std::vector<std::string> ParsedIntent; 
using ParsedIntent = std::vector<std::string>;
// Intents form the backbone of IMC, each intent is processed 
// by the modules, and so - any module can willingly ignore the
// pending intents. Each intent is string recognized and matched
struct Intent {
	Intent(std::string v) : value(v) {}
	std::string value;

	/// \brief: Default process intent function and returns as a vector. This can be overloaded as needed.
	virtual ParsedIntent Parse()
	{
		ParsedIntent expanded_intent;
		boost::split(expanded_intent, this->value, boost::is_any_of("/"));
		if(expanded_intent.size() <= 2) 
			throw std::length_error("Invalid Intent. An intent requires the format SOURCE_MODULE/DESTINATION_MODULE/FUNCTION/VAL1/VAL2/...");
		return expanded_intent;
	}
	virtual ParsedIntent Parse(Intent &i)
	{
		ParsedIntent expanded_intent;
		boost::split(expanded_intent, i.value, boost::is_any_of("/"));
		if(expanded_intent.size() <= 2) 
			throw std::length_error("Invalid Intent. An intent requires the format SOURCE_MODULE/DESTINATION_MODULE/FUNCTION/VAL1/VAL2/...");
		return expanded_intent;
	}
};
// PendingIntents is basically a wrapper around the std::deque structure.
// The primary modification is that the pop_front() and pop_back() methods
// now return the Intent in each respective situation.
typedef struct {
	std::deque<Intent> pending;
	Intent pop_front() 
	{
		if(pending.empty()) 
			throw std::range_error("Pending Intents Queue is empty.");

		Intent copy = pending.front();
		pending.pop_front();
		return copy;
	}
	Intent pop_back()
	{
		if(pending.empty()) 
			throw std::range_error("Pending Intents Queue is empty.");

		Intent copy = pending.back();
		pending.pop_back();
		return copy;
	}
	//Silly wrappers
	void push_front(Intent &i) { pending.push_front(i); }
	void push_back(Intent &i) { pending.push_back(i); }
	bool empty() { return pending.empty(); }
} PendingIntents;


#endif /*_INTENT_H_*/
