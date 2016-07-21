/*Copyright (c) 2016 "University of Denver"

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
    This is the global logger module.
*/

// Includes
#include "debug/debugging.h"


/**
 * BOOST_LOG_GLOBAL_LOGGER_INIT is the global logger shared across the project.
 * We define the global loggers properties here.
 */
BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
    src::severity_logger_mt<boost::log::trivial::severity_level> logger;

//    logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));

    // add attributes
    logger.add_attribute("LineID", attrs::counter<unsigned int>(1));     // lines are sequentially numbered
    logger.add_attribute("TimeStamp", attrs::local_clock());             // each log line gets a timestamp
    logger.add_attribute("Scope", attrs::named_scope());
    logger.add_attribute("Uptime", attrs::timer());

    // add a text sink, this streams to the console
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    // add a multi file sink for creating/writing to logfiles
    typedef sinks::synchronous_sink<sinks::text_multifile_backend> multifile_sink;
    boost::shared_ptr<multifile_sink> filesink = boost::make_shared<multifile_sink>();

    {
        //This scope is used for communicating with the logging backend. Once the scope has been exited,
        //there shouldn't be any changes made to the backend.
        text_sink::locked_backend_ptr textsink_backend_ptr = sink->locked_backend();
        multifile_sink::locked_backend_ptr filesink_backend_ptr = filesink->locked_backend();
        //Console streaming
        boost::shared_ptr<std::ostream> console_stream(&std::clog, boost::null_deleter());
        textsink_backend_ptr->add_stream(console_stream);

        //Setup the multifile text sink
        filesink_backend_ptr->set_file_name_composer(
                    sinks::file::as_file_name_composer(expr::stream
                                                       << "../../../logs/" //Set log file destination here
                                                       << channel.or_default<std::string>("Pineapple") << "-"
                                                       << severity.or_default(0) << ".log"));


    }
    // log message format
    logging::formatter formatter = expr::stream
            << "\033[0m" << expr::attr < unsigned int > ("LineID")
            << " [" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S.%f")
            << "] [" << expr::attr<logging::trivial::severity_level>("Severity") //boost::log::trivial::severity
            << "] [" << expr::attr<boost::posix_time::time_duration>("Uptime")
            << "] [" << expr::format_named_scope("Scope", keywords::format = "%n", keywords::iteration = expr::reverse) << "] "
            << expr::smessage;

    //Assign the above log format style to both of our sinks
    sink->set_formatter(formatter);
    filesink->set_formatter(formatter);

    // only messages with severity >= SEVERITY_THRESHOLD are written
    // you can change the SEVERITY_THRESHOLD in the header file.
    sink->set_filter(severity >= SEVERITY_THRESHOLD);
    filesink->set_filter(severity >= SEVERITY_THRESHOLD);


    // "register" our sinks with the logging core
    logging::core::get()->add_sink(sink);
    logging::core::get()->add_sink(filesink);

    return logger;
}

