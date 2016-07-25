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
#ifndef LIBNAO_H
#define LIBNAO_H

///Includes
#include "include/common.h"
#include "include/modules/hal/ananas_comosus.h"
#include <boost/shared_ptr.hpp>
#include <exception>
#include <string>

using namespace boost::interprocess;

class Nao : public AL::Module
{
public:
    Nao(boost::shared_ptr<AL::Broker> pBroker);

private:
    static Nao *instance;

    managed_shared_memory shm;

    AL::DCMProxy *dcmproxy;
    AL::ALMemoryProxy *memoryproxy;
    AL::ALValue positionRequest;
    AL::ALValue stiffnessRequest;
    AL::ALValue usRequest;
    AL::ALValue ledRequest;
};

#endif // LIBNAO_H
