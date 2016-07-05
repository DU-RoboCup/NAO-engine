#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#define BOOST_LOG_DYN_LINK 1 // necessary when linking the boost_log library dynamically
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
-- Created by Paul Heinen 6/21/16

// FILE DESCRIPTION
    This is the global logger modules header file.
*/

// Includes
#include <boost/shared_ptr.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/exceptions.hpp>

#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>

#include <boost/log/support/date_time.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/utility/string_literal.hpp>

#include <boost/log/attributes/mutable_constant.hpp>
#include "null_deleter.hpp"
#include <exception>
#include <string>
#include <ostream>

// Boost.Log Namespaces
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;


// Basic exception handler
struct handler
{
    void operator()(const logging::runtime_error &ex) const
    {
        std::cerr << "boost::log::runtime_error: " << ex.what() << ", congrats, you broke the logger.\n";
    }
    void operator()(const std::exception &ex) const
    {
        std::cerr << "std::exception: " << ex.what() << ", congrats, you broke the logger.\n";
    }
};

// Logger Attributes

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope)
BOOST_LOG_ATTRIBUTE_KEYWORD(uptime, "Uptime", boost::posix_time::time_duration)
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string)


// Functions
std::string path_to_filename(std::string path);

// just log messages with severity >= SEVERITY_THRESHOLD are written
/* ========= ENTER SEVERITY THRESHOLD HERE =========
 * Options are:
 * 1. trace
 * 2. info
 * 3. debug
 * 4. warning
 * 5. error
 * 6. fatal
 */

#define SEVERITY_THRESHOLD logging::trivial::debug
// =================================================
// register a global logger
BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

#define LOG(severity) BOOST_LOG_SEV(logger::get(), boost::log::trivial::severity)

// ===== log macros =====
#define LOG_TRACE   LOG(trace)
#define LOG_DEBUG   LOG(debug)
#define LOG_INFO    LOG(info)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR   LOG(error)
#define LOG_FATAL   LOG(fatal)



#endif

