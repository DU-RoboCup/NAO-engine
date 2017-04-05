/*
Copyright (c) 2017 "University of Denver"
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
-- Created by Paul Heinen 2/2/17
// FILE DESCRIPTION
This class is a Priority Queue used for managing the order in which intents are 
are sent to NAOqi. This may be useful if you wish to have high priority items
quickly sent to the NAO (either Hardware Write Requests or Hardware Reads). 

This class utilizes a few C++14 features.
*/

#ifndef _HAL_PRIORITY_QUEUE_H_ //_HAL_PRIORITY_QUEUE_H_
#define _HAL_PRIORITY_QUEUE_H_

#include <queue>
#include <vector>
#include <tuple>
#include <iostream>
#include <exception>

enum QPRIORITY_FLAG { DEFAULT, LOW = DEFAULT, MEDIUM, HIGH, URGENT};
typedef std::tuple<int, float, int> queued_intent;
class HAL_PQ
{
public:
    HAL_PQ(); //Default constructor
    HAL_PQ(std::vector<queued_intent> prority_intent_list);//Construct a HPQ from pre-initialized items
    ~HAL_PQ(); //Default Destructor

    void push(const queued_intent &q); //Add an intent to the queue
    const queued_intent &top() const; //Get the top queued item
    const queued_intent &pop_top(); //Get top item and remove it from the list
    void pop(); //Remove the top element from the queue
    std::size_t size() const; //Returns the number of items in the queue
    void print_and_clear(); //Prints out the queue using a stream and clears it
    void print(); //Prints out a copy of the queue using a stream
    void clear(); //clears the queue, but doesn't destroy the object


private:
    /* 
     * \brief: λ_cmp_obj is a Functor Object.
     * 
     * This object exists because Concepts haven't been added to C++ yet,
     * thus we cant use a lambda expression for sorting the tuples by flag.
     * Though you can compile with Concepts support in GCC, it hasn't been 
     * officially standardized in the ISO and could cause annoyances in the future...
     * 
     * Side Note: Apparently MSVC can compile fine with Unicode charcaters in function names but our GCC config cant...
     */
    struct cmp_Lambda_obj
	{
		auto operator()(queued_intent const& left, queued_intent const& right)->bool
		{
			const auto &L_FLAG = std::get<2>(left);
			const auto &R_FLAG = std::get<2>(right);
			if ((!(L_FLAG >= 0 && L_FLAG <= URGENT)) || (!(R_FLAG >= 0 && R_FLAG <= URGENT)))
				throw std::out_of_range("Error: invalid flag paramater!");
			return L_FLAG < R_FLAG;
		}
	};

    // intent_priority_queue is a priority queue (Heap data structure). Constant access time for pop, and top.
    // push requires comparing priority flags so O(n).
	std::priority_queue < queued_intent, std::vector<queued_intent>, cmp_Lambda_obj> intent_priority_queue;
};


#endif //_HAL_PRIORITY_QUEUE_H_