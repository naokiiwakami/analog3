#ifdef __linux // feature available only on Raspberry Pi

#include "I2CModuleDriver.h"
#include "ModuleRecognitionException.h"

#include "connector.pb.h"

#include "connector.pb.h"

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include <fcntl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <linux/i2c-dev.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <stack>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define ETX 0x03 // end of text
#define FF  0x0c // form feed

using namespace rapidjson;
using namespace google::protobuf;

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("I2CModuleDriver"));

I2CRackDriver::I2CRackDriver(const std::string& deviceName, const std::string& slaveAddress)
{
    const static std::string fname = "I2CRackDriver::I2CRackDriver()";

    m_device = "/dev/";
    m_device += deviceName;

    if (slaveAddress.empty() || (m_slaveAddress = atoi(slaveAddress.c_str())) <= 0) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << m_slaveAddress << ": invalid slave address"));
        // TODO: throw
    }
        
    m_fd = open("/dev/i2c-1", O_RDWR);
    if (m_fd < 0) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << m_device << ": cannot open device (" << strerror(errno) << ")"));
        // TODO: throw
    }
    char command = 'p'; // TODO: make command table
    if (!setupAddress() || write(m_fd, &command, 1) < 0) {
        LOG4CPLUS_ERROR(logger,
                        LOG4CPLUS_TEXT(fname << ": " << m_slaveAddress << ": slave not responding"));
    }
}

bool I2CRackDriver::discover(std::list<ModuleDriver*>* modulesList)
{
    static const std::string fname = "I2CRackDriver::discover()";

    std::string rackName;
    if (!sendCommand("n", &rackName)) {
        // TODO: log
        return false;
    }
    
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << " device name is " << rackName));

    std::string data;
    if (!sendCommand("d", &data)) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << " error in describe operation"));
        return false;
    }
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << " retrieved description. length=" << data.size()));

    connector::Description description;
    description.ParseFromString(data);
    std::string dump;
    io::StringOutputStream output(&dump);
    TextFormat::Print(description, &output);
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": description=" << dump));

    return true;
}

bool I2CRackDriver::setupAddress()
{
    const static std::string fname = "I2CRackDriver::setupAddress()";

    bool result = (ioctl(m_fd, I2C_SLAVE, m_slaveAddress) >= 0);
    if (!result) {
        LOG4CPLUS_ERROR(logger,
                        LOG4CPLUS_TEXT(fname << "failed to setup address to read"));
    }
    return result;
}

bool I2CRackDriver::sendCommand(const std::string& command, std::string* response)
{
    const static std::string fname = "I2CRackDriver::sendCommand()";

    write(m_fd, command.c_str(), command.size());

    if (response == NULL) {
        return true;
    }

    response->clear();

    while (true) {
        uint8_t chunkSize;
        useconds_t usec = 1;
        int credit = 20;
        
        do {
            if (usec > 1) {
                usleep(usec);
            }
            read(m_fd, &chunkSize, 1);
            usec *= 2;
        }
        while (chunkSize == 0xff && --credit > 0);

        if (chunkSize == 0xff) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << " device not responding"));
            return false;
        }
        uint8_t data[256];
        int nread = read(m_fd, data, chunkSize);
        response->append((char*) data, nread - 1);
        if (data[nread - 1] == ETX) {
            return true;
        }
    }

    return true;
}

I2CModuleDriver::I2CModuleDriver(const std::string& deviceName, const std::string& fileName)
{
}

I2CModuleDriver::~I2CModuleDriver()
{
}

const std::string&
I2CModuleDriver::getName()
{
    return "";
}

const std::string&
I2CModuleDriver::getFullName()
{
    return "";
}

bool
I2CModuleDriver::describe(connector::Component* component,
                           std::string* errorMessage)
{
    return true;
}

bool
I2CModuleDriver::modifyAttribute(const connector::Request& request,
                                  std::string* errorMessage)
{
    return true;
}

bool
I2CModuleDriver::addSubComponent(const connector::Request& request,
                                  std::string* errorMessage)
{
    return true;
}

bool
I2CModuleDriver::removeSubComponent(const connector::Request& request,
                                     std::string* errorMessage)
{
    return true;
}

#endif
