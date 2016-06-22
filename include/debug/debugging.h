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
-- Created by Paul Heinen 06/21/16

// FILE DESCRIPTION
Glorious Debug module to keep NAO safe
*/

#ifndef _DEBUG_h_GUARD
#define _DEBUG_h_GUARD

#ifdef PINEAPPLE_VERSION_0_0_1
// Put version specific code here
#endif

#ifdef NAO_SDK_VERSION_2_1_4_13
// Put Nao SDK version specific code here
#endif


// INCLUDES

////Logging Core, essential
#include <boost/log/core.hpp>

////Trivial allows for simple logging
#include <boost/log/trivial.hpp>

////Using a single global log
#include <boost/log/sources/global_logger_storage.hpp>

///Expressions allow filtering log information
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <ostream>


/**
* Doxygen style documentation for the class
*/

/** Define the log file name **/
#define LOGFILE "systemlog.log"

/** Set log message severity level threshold, anything with
 * a severity less than the threshold won't be logged
 */
#define SEVERITY_THRESHOLD logging::trivial::warning

/** Setup the global logger **/

BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

#define LOG(severity) BOOST_LOG_SEV(logger::get(), boost::log::trivial::severity)

/** Log Macros **/
#define LOG_TRACE	LOG(trace)
#define LOG_DEBUG	LOG(debug)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR	LOG(error)
#define LOG_FATA	LOG(fatal)

#endif 
