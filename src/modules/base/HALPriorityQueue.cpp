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

#include "include/modules/base/HALPriorityQueue.h"

HAL_PQ::HAL_PQ() 
{

}

HAL_PQ::HAL_PQ(std::vector<queued_intent> priority_intent_list)
{
    for(auto &p_intent : priority_intent_list)
    {
        intent_priority_queue.push(p_intent);
    }
}

HAL_PQ::~HAL_PQ()
{
}

void HAL_PQ::push(const queued_intent &q)
{
    intent_priority_queue.push(q);
}

const queued_intent &HAL_PQ::top() const
{
    return intent_priority_queue.top();
}

void HAL_PQ::pop()
{
    intent_priority_queue.pop();
}
const queued_intent HAL_PQ::pop_top() { 
    queued_intent item = intent_priority_queue.top(); 
    intent_priority_queue.pop();
    return item; 
}
std::size_t HAL_PQ::size() const
{
    return intent_priority_queue.size();
}

void HAL_PQ::print_and_clear()
{
    auto &q = intent_priority_queue;
    std::cout << "printing queue:\n\t";
    while (!q.empty())
    {
        auto &intent_priority_queue = q.top();
        std::cout << "(" << std::get<0>(intent_priority_queue) << ", " << std::get<1>(intent_priority_queue) << ", " << std::get<2>(intent_priority_queue) << ")\n\t";
        q.pop();
    }
    std::cout << "\n";
}

void HAL_PQ::print()
{
    auto q = intent_priority_queue;
    std::cout << "printing queue:\n\t";
    while (!q.empty())
    {
        auto &intent_priority_queue = q.top();
        std::cout << "(" << std::get<0>(intent_priority_queue) << ", " << std::get<1>(intent_priority_queue) << ", " << std::get<2>(intent_priority_queue) << ")\n\t";
        q.pop();
    }
    std::cout << "\n";
}
void HAL_PQ::clear()
{
    //oh boy...
    while(!intent_priority_queue.empty())
        intent_priority_queue.pop();
}