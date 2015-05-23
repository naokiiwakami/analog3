#ifdef __linux // feature available only on Raspberry Pi

#include "I2CModuleDriver.h"
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

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("I2CModuleDriver"));

struct AttributeValue
{
    int ivalue;
    bool has_ivalue;
    std::vector<std::string> svalue;

    AttributeValue()
        : ivalue(0), has_ivalue(false)
    {}

    ~AttributeValue()
    {}

    void setInt(int value)
    {
        ivalue = value;
        has_ivalue = true;
    }

    void addString(const std::string& value)
    {
        svalue.push_back(value);
    }
};

struct I2cComponent
{
    std::string name;
    std::string fullName;
    int id;
    std::map<std::string, AttributeValue> attributes;
    std::map<std::string, I2cComponent*> subComponentsDict;
    std::vector<I2cComponent*> subComponents;

    I2cComponent()
        : id(0)
    {}

    ~I2cComponent()
    {
        std::vector<I2cComponent*>::iterator it = subComponents.begin();
        std::vector<I2cComponent*>::iterator end = subComponents.end();
        for (; it != end; ++it) {
            delete *it;
        }
    }
};

class I2CModuleDriverData
{
public:
    static I2CModuleDriverData* create(I2CRackDriver* rackDriver, int slaveAddress,
                                       const compact_descriptor::Component& moduleDescriptor)
    {
        I2CModuleDriverData* data = new I2CModuleDriverData();

        data->m_rackDriver = rackDriver;
        data->m_i2cSlaveAddress = slaveAddress;

        data->m_component = createComponent(moduleDescriptor);

        int numSubComponent = moduleDescriptor.sub_component_size();
        for (int ic = 0; ic < numSubComponent; ++ic) {
            const compact_descriptor::Component& scDescriptor = moduleDescriptor.sub_component(ic);
            I2cComponent* subComponent = createComponent(scDescriptor);
            data->m_component->subComponentsDict[subComponent->fullName] = subComponent;
            data->m_component->subComponents.push_back(subComponent);
        }

        return data;
    }

    static I2cComponent* createComponent(const compact_descriptor::Component& componentDesc)
    {
        I2cComponent* component = new I2cComponent();
        component->name = componentDesc.name();
        compact_descriptor::Component_Type componentType = componentDesc.type();
        component->fullName = componentTypes[componentType];
        component->fullName += component->name;
        component->id = componentDesc.id();

        int numAttributes = componentDesc.attribute_size();
        for (int iattr = 0; iattr < numAttributes; ++iattr) {
            const compact_descriptor::Attribute& attribute = componentDesc.attribute(iattr);
            if (attribute.type() < NumAttributeTypes) {
                std::string name = attrTypes[attribute.type()];
                AttributeValue value;
                if (attribute.has_ivalue()) {
                    value.setInt(attribute.ivalue());
                }

                int nstr = attribute.svalue_size();
                for (int istr = 0; istr < nstr; ++istr) {
                    value.addString(attribute.svalue(istr));
                }
                component->attributes[name] = value;
            }
        }

        // Special attributes
        const char* direction = NULL;
        const char* signal = NULL;
        switch (componentType) {
        case compact_descriptor::Component_Type_ValueInputPort:
            direction = directionInput;
            signal = signalValue;
            break;
        case compact_descriptor::Component_Type_ValueOutputPort:
            direction = directionOutput;
            signal = signalValue;
            break;
        case compact_descriptor::Component_Type_GateInputPort:
            direction = directionInput;
            signal = signalGate;
            break;
        case compact_descriptor::Component_Type_GateOutputPort:
            direction = directionOutput;
            signal = signalGate;
            break;
        default:
            // do nothing
            break;
        }

        if (direction != NULL) {
            component->attributes[attrTypes[compact_descriptor::Attribute_Type_Direction]].svalue.push_back(direction);
        }
        if (signal != NULL) {
            component->attributes[attrTypes[compact_descriptor::Attribute_Type_Signal]].svalue.push_back(signal);
        }

        const char* attrName = attrTypes[compact_descriptor::Attribute_Type_ModuleType];
        if (componentType == compact_descriptor::Component_Type_Module &&
            component->attributes.find(attrName) == component->attributes.end()) {
            component->attributes[attrName].svalue.push_back(component->name);
        }

        return component;
    }

    bool convertToProtocolBuf(connector::Component* pbComponent, std::string* errorMessage)
    {
        return convert(pbComponent, m_component, errorMessage);
    }

    bool convert(connector::Component* pbComponent, I2cComponent* component, std::string* errorMessage)
    {
        pbComponent->set_name(component->fullName);
        pbComponent->set_id(component->id);

        // attributes
        std::map<std::string, AttributeValue>::iterator it_attr = component->attributes.begin();
        std::map<std::string, AttributeValue>::iterator it_attrEnd = component->attributes.end();
        for (; it_attr != it_attrEnd; ++it_attr) {
            connector::Attribute* pbAttribute = pbComponent->add_attribute();
            pbAttribute->set_name(it_attr->first);
            connector::Value* pbValue = pbAttribute->mutable_value();
            AttributeValue& value = it_attr->second;
            if (value.has_ivalue) {
                pbValue->set_ivalue(value.ivalue);
            }
            else if (value.svalue.size() == 1) {
                pbValue->set_svalue(value.svalue[0]);
            }
            else if (value.svalue.size() > 1) {
                for (size_t iv = 0; iv < value.svalue.size(); ++iv) {
                    connector::Value* avalue = pbValue->add_avalue();
                    avalue->set_svalue(value.svalue[iv]);
                }
            }
        }

        // sub components
        for (size_t i = 0; i < component->subComponents.size(); ++i) {
            if (!convert(pbComponent->add_sub_component(), component->subComponents[i], errorMessage)) {
                return false;
            }
        }

        return true;
    }

    ~I2CModuleDriverData() {}

    I2cComponent* m_component;

    I2CRackDriver* m_rackDriver;
    int m_i2cSlaveAddress;
    int m_componentId;

private:
    I2CModuleDriverData()
        : m_component(NULL)
    {}

    static const char* componentTypes[];
    static const int NumComponentTypes;
    static const char* attrTypes[];
    static const int NumAttributeTypes;
    static const char* directionInput;
    static const char* directionOutput;
    static const char* signalValue;
    static const char* signalGate;
};

const char* I2CModuleDriverData::componentTypes[] = {
    "Rack.",
    "Module.",
    "Knob.",
    "Selector.",
    "Port.",
    "Port.",
    "Port.",
    "Port.",
};
const int I2CModuleDriverData::NumComponentTypes = 8;

const char* I2CModuleDriverData::attrTypes[] = {
    "value",
    "scale",
    "choices",
    "wireId",
    "direction",
    "signal",
    "moduleType",
};
const int I2CModuleDriverData::NumAttributeTypes = 7;

const char* I2CModuleDriverData::directionInput = "INPUT";
const char* I2CModuleDriverData::directionOutput = "OUTPUT";
const char* I2CModuleDriverData::signalValue = "value";
const char* I2CModuleDriverData::signalGate = "note";


I2CRackDriver::I2CRackDriver(const std::string& deviceName)
{
    const static std::string fname = "I2CRackDriver::I2CRackDriver()";

    m_device = "/dev/";
    m_device += deviceName;

}

/**
 * Discover the modules that are linked to the I2C network.
 * Strategy:
 *   - Scan through device addresses from 0x08 to 0x80 (128)
 *     by sending PING ('p') commands.
 *   - If you get a response at an address, send DESCRIBE command
 *     ('d') to the device.
 *   - Make I2CModuleDriver objects by parsing the response.
 *     The module descriptions are in Protocol Buffers compact_descriptor
 *     form, the process decodes the protocol buffer message first,
 *     the convert is to a driver object.
 */
bool I2CRackDriver::discover(std::list<ModuleDriver*>* modulesList)
{
    static const std::string fname = "I2CRackDriver::discover()";

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
                I2CModuleDriver* driver = new I2CModuleDriver();
                const compact_descriptor::Component& module = description.component(ic);
                driver->m_data = I2CModuleDriverData::create(this, slaveAddress, module);
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

bool I2CRackDriver::setupAddress(int slaveAddress)
{
    const static std::string fname = "I2CRackDriver::setupAddress()";

    bool result = (ioctl(m_fd, I2C_SLAVE, slaveAddress) >= 0);
    if (!result) {
        LOG4CPLUS_DEBUG(logger,
                        LOG4CPLUS_TEXT(fname << ": failed to setup address. addr=" << slaveAddress));
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

I2CModuleDriver::I2CModuleDriver()
    : m_data(NULL)
{
}

I2CModuleDriver::~I2CModuleDriver()
{
    delete m_data;
}

const std::string&
I2CModuleDriver::getName()
{
    return m_data->m_component->name;
}

const std::string&
I2CModuleDriver::getFullName()
{
    return m_data->m_component->fullName;
}

bool
I2CModuleDriver::describe(connector::Component* component,
                          std::string* errorMessage)
{
    return m_data->convertToProtocolBuf(component, errorMessage);
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
