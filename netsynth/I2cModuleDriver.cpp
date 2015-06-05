#ifdef __linux // feature available only on Raspberry Pi

#include "I2cModuleDriver.h"

#include "Component.h"
#include "ModuleRecognitionException.h"

#include "connector.pb.h"
#include "compact_descriptor.pb.h"

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

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("I2cModuleDriver"));

class I2cModuleDriverData
{
public:
    static I2cModuleDriverData* create(I2cRackDriver* rackDriver, int slaveAddress,
                                       const compact_descriptor::Component& moduleDescriptor)
    {
        I2cModuleDriverData* data = new I2cModuleDriverData();

        data->m_rackDriver = rackDriver;
        data->m_i2cSlaveAddress = slaveAddress;

        data->m_component = Component::create(moduleDescriptor, &data->m_idTable);

        return data;
    }

    ~I2cModuleDriverData() {
        delete m_component;
    }

    Component* m_component;
    std::map<std::string, Component*> m_nameTable;
    std::map<int, Component*> m_idTable;

    I2cRackDriver* m_rackDriver;
    int m_i2cSlaveAddress;
    int m_componentId;

private:
    I2cModuleDriverData()
        : m_component(NULL)
    {}

};


I2cRackDriver::I2cRackDriver(const std::string& deviceName)
{
    const static std::string fname = "I2cRackDriver::I2cRackDriver()";

    m_device = "/dev/";
    m_device += deviceName;

}

/**
 * Discover the modules that are linked to the I2c network.
 * Strategy:
 *   - Scan through device addresses from 0x08 to 0x80 (128)
 *     by sending PING ('p') commands.
 *   - If you get a response at an address, send DESCRIBE command
 *     ('d') to the device.
 *   - Make I2cModuleDriver objects by parsing the response.
 *     The module descriptions are in Protocol Buffers compact_descriptor
 *     form, the process decodes the protocol buffer message first,
 *     the convert is to a driver object.
 */
bool I2cRackDriver::discover(std::list<ModuleDriver*>* modulesList)
{
    static const std::string fname = "I2cRackDriver::discover()";

    m_fd = open(m_device.c_str(), O_RDWR);
    if (m_fd < 0) {
        LOG4CPLUS_ERROR(logger,
                        LOG4CPLUS_TEXT(fname << ": " << m_device
                                       << ": cannot open device (" << strerror(errno) << ")"));
        return false;
    }

    // detect devices and retrieve module descriptions
    char command = 'p'; // TODO: make command table
    const int start = 0x08; // slave address to start
    const int end = 0x80; // slave address to end

    for (int slaveAddress = start; slaveAddress < end; ++slaveAddress)
    {
        // ping against the slave address.
        if (setupAddress(slaveAddress) && write(m_fd, &command, 1) >= 0)
        {
            // Execute "d"escribe command.
            std::string data;
            if (!sendCommand("d", &data)) {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << " error in describe operation"));
                return false;
            }
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << " retrieved description. length=" << data.size()));

            // Parse the command response to get module descriptors.
            compact_descriptor::Description description;
            description.ParseFromString(data);

            if (logger.getLogLevel() <= log4cplus::DEBUG_LOG_LEVEL) {    
                std::string dump;
                io::StringOutputStream output(&dump);
                TextFormat::Print(description, &output);
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": description=" << dump));
            }

            // Make module drivers from module descriptors.
            int numModules = description.component_size();
            for (int ic = 0; ic < numModules; ++ic) {
                I2cModuleDriver* driver = new I2cModuleDriver();
                const compact_descriptor::Component& module = description.component(ic);
                driver->m_data = I2cModuleDriverData::create(this, slaveAddress, module);
                if (driver->m_data == NULL) {
                    LOG4CPLUS_ERROR(logger,
                                    LOG4CPLUS_TEXT(fname << ": Failed to read module. "
                                                   << "name=" << module.name()));
                    delete driver;
                }
                else {
                    modulesList->push_back(driver);
                }
            }
        }
    }

    return true;
}

bool I2cRackDriver::setupAddress(int slaveAddress)
{
    const static std::string fname = "I2cRackDriver::setupAddress()";

    bool result = (ioctl(m_fd, I2C_SLAVE, slaveAddress) >= 0);
    if (!result) {
        LOG4CPLUS_DEBUG(logger,
                        LOG4CPLUS_TEXT(fname << ": failed to setup address. addr=" << slaveAddress));
    }
    return result;
}

bool I2cRackDriver::sendCommand(const std::string& command, std::string* response)
{
    const static std::string fname = "I2cRackDriver::sendCommand()";

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

I2cModuleDriver::I2cModuleDriver()
    : m_data(NULL)
{
}

I2cModuleDriver::~I2cModuleDriver()
{
    delete m_data;
}

const std::string&
I2cModuleDriver::getName()
{
    return m_data->m_component->getName();
}

const std::string&
I2cModuleDriver::getFullName()
{
    return m_data->m_component->getFullName();
}

bool
I2cModuleDriver::describe(connector::Component* component,
                          std::string* errorMessage)
{
    return m_data->m_component->convertToProtocolBuf(component, errorMessage);
}

bool
I2cModuleDriver::modifyAttribute(const connector::Request& request,
                                 std::string* errorMessage)
{
    static const std::string fname = "I2cModuleDriver::modifyAttribute()";
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": module " << m_data->m_component->getFullName() << " enter"));

    // Find the target component
    Component* component = resolveComponent(request, errorMessage);
    if (component == NULL) {
        *errorMessage = fname + " - " + *errorMessage;
        return false;
    }

    // Read the request and make command parameters
    if (!request.has_attribute()) {
        *errorMessage = fname + ": ModifyAttribute: Mandatory parameter \"attribute\" is missing.";
        return false;
    }
    const connector::Attribute& attr = request.attribute();

    const AttributeValue* attributeValue = component->getAttribute(attr.name());
    if (attributeValue == NULL) {
        *errorMessage = fname + ": ModifyAttribute: " + attr.name() + ": no such attribute with component " + component->getName();
        return false;
    }

    const char command = 'm';
    compact_descriptor::Attribute::Type attributeType = attributeValue->attributeType;
    uint8_t attributeId = attributeValue->id;

    int16_t value = 0;

    switch (request.command()) {
    case connector::Request::SET_ATTRIBUTE: {
        if (!attr.has_value()) {
            *errorMessage = fname + ": ModifyAttribute: Value in attribute is missing.";
            return false;
        }
        if (attr.value().has_ivalue()) {
            value = attr.value().ivalue();
        }
        else {
            *errorMessage = fname + ": ModifyAttribute: " + attr.name() + ": modifying value must be integer.";
            return false;
        }
        break;
    }
    case connector::Request::UNSET_ATTRIBUTE:
        if (attributeType != compact_descriptor::Attribute::WireId) {
            *errorMessage = fname + ": MudifyAttribute: " + compact_descriptor::Attribute::Type_Name(attributeType) + ": unset value is not supported";
            return false;
        }
        value = 0;
        break;
    default:
        // Unknown command. Do nothing.
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": " << connector::Request::Command_Name(request.command()) << ": unknown command"));
        return true;
    }

    // Make command parameters
    std::string message;
    message += command;
    message += (char) attributeType;
    message += (char) attributeId;
    uint8_t temp = 0xff & (value >> 8);
    message += (char) temp;
    temp = value & 0xff;
    message += (char) value;

    if (!m_data->m_rackDriver->setupAddress(m_data->m_i2cSlaveAddress) ||
        !m_data->m_rackDriver->sendCommand(message)) {
        *errorMessage = fname + " ModifyAttribute: command execution failed";
        return false;
    }

    // Update cache
    component->setAttribute(attr.name(), value);

    return true;
}

bool
I2cModuleDriver::addSubComponent(const connector::Request& request,
                                  std::string* errorMessage)
{
    return true;
}

bool
I2cModuleDriver::removeSubComponent(const connector::Request& request,
                                     std::string* errorMessage)
{
    static const std::string fname = "I2cModuleDriver::removeSubComponent()";
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": module " << m_data->m_component->getFullName() << " enter"));

    // Resolve the component to remove
    Component* component = resolveComponent(request, errorMessage);
    if (component == NULL) {
        *errorMessage = fname + " - " + *errorMessage;
        return false;
    }
    if (component->hasSubComponent()) {
        *errorMessage = fname + ": " + component->getName() + ": Subcomponents not empty";
        return false;
    }

    // Send command message
    const char command = 'm';
    uint8_t componentId = component->getId();
    std::string message;
    message += command;
    message += (char) componentId;

    if (!m_data->m_rackDriver->setupAddress(m_data->m_i2cSlaveAddress) ||
        !m_data->m_rackDriver->sendCommand(message)) {
        *errorMessage = fname + " RemoveSubComponent: command execution failed";
        return false;
    }

    // Update cache
    component->remove();
    delete component;

    return true;
}

Component*
I2cModuleDriver::resolveComponent(const connector::Request& request,
                                  std::string* errorMessage)
{
    static const std::string fname = "I2cModuleDriver::resolveComponent()";

    int num_paths = request.path_size();
    if (num_paths < 2) {
        *errorMessage = fname + ": Invalid request: target path is incomplete";
        return NULL;
    }

    Component* component = m_data->m_component;
    for (int level = 1; level < num_paths; ++level) {
        const std::string& name = request.path(level);
        component = component->findSubComponent(name);
        if (component == NULL) {
            *errorMessage = fname + ": target component " + name + " not found";
            return NULL;
        }
    }

    return component;
}

#endif
