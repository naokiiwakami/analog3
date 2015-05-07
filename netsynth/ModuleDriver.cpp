#include "ModuleDriver.h"
#include "FileModuleDriver.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ModuleDriver"));

RackDriver* RackDriver::create(const char* rackURL)
{
    const static std::string fname = "RackDriver::create()";

    RackDriver* rackDriver = NULL;
    if (strncasecmp(rackURL, "file:", 5) == 0) {
        const char* ptr = rackURL + 5;
        if (*ptr == '/') {
            ++ptr;
        }
        rackDriver = new FileRackDriver(ptr);
    }
    else {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << rackURL << ": unknown protocol"));
    }

    return rackDriver;
}
