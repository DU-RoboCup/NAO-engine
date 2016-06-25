#include "debug/debugging.h"



std::string path_to_filename(std::string path)
{
    std::cout << "Path to filename called" << std::endl;
    return path.substr(path.find_last_of("/\\")+1);
}
BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
    src::severity_logger_mt<boost::log::trivial::severity_level> logger;

    logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));

    // add attributes
    logger.add_attribute("LineID", attrs::counter<unsigned int>(1));     // lines are sequentially numbered
    logger.add_attribute("TimeStamp", attrs::local_clock());             // each log line gets a timestamp
 //   logger.add_attribute("File", attrs::mutable_constant<std::string>(""));


    // add a text sink
    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

    // add a logfile stream to our sink
    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(LOGFILE));

    // add "console" output stream to our sink
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost_future::null_deleter()));

    // specify the format of the log message
    logging::formatter formatter = expr::stream
            << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
            << ": <" << boost::log::trivial::severity << "> "
            << '[' << "NULL" /*<< expr::attr<std::string>("File")*/
                   << ':' << "NULL"/*<< expr::attr<int>("Line")*/ << "] "
            << expr::smessage;
        /*<< std::setw(7) << std::setfill('0') << line_id << std::setfill(' ') << " | "
        << expr::format_date_time(timestamp, "%Y-%m-%d, %H:%M:%S.%f") << " "
        << "[" << logging::trivial::severity << "]"
        << " - " << expr::smessage;*/

    sink->set_formatter(formatter);

    // only messages with severity >= SEVERITY_THRESHOLD are written
    sink->set_filter(severity >= SEVERITY_THRESHOLD);

    // "register" our sink
    logging::core::get()->add_sink(sink);

    return logger;
}


