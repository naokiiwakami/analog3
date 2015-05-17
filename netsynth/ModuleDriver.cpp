#include "ModuleDriver.h"
#include "FileModuleDriver.h"

#ifdef __linux
#include "I2CModuleDriver.h"
#endif

#include "connector.pb.h"

#include <pb_encode.h>
#include <pb_decode.h>

#include "compact_descriptor.pb.h"
#include "nano_compact_descriptor.pb.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <stdio.h>
#include <string>
#include <string.h>

using namespace google::protobuf;


static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ModuleDriver"));

class StubModuleDriver : public ModuleDriver
{
public:
    StubModuleDriver()
    {
        m_name = "eg";
        m_fullName = "Module.eg";
    }
    const std::string& getName() { return m_name; }
    const std::string& getFullName() { return m_fullName; }

    virtual bool describe(connector::Component* component,
                          std::string* errorMessage)
    {
        /*
        fprintf(stderr, "HEY!\n");

        connector_Component nano_component = {};
        std::string name = "Module.eg_nano";
        nano_component.name.funcs.encode = &write_string;
        nano_component.name.arg = &name;

        std::string data;
        pb_ostream_t stream = { &ostream_callback, &data, 65536, 0 };
        pb_encode(&stream, connector_Component_fields, &nano_component);

        component->ParseFromString(data);
        */

        return true;
    }

    virtual bool modifyAttribute(const connector::Request& request,
                                 std::string* errorMessage)
    {        
        return true;
    }

    virtual bool addSubComponent(const connector::Request& request,
                                 std::string* errorMessage)
    {
        return true;
    }

    virtual bool removeSubComponent(const connector::Request& request,
                                    std::string* errorMessage)
    {
        return true;
    }

protected:
    std::string m_name;
    std::string m_fullName;
};

class NanopbStubModuleDriver : public StubModuleDriver
{
public:
    NanopbStubModuleDriver()
        : StubModuleDriver()
    {}

    static bool ostream_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count)
    {
        std::string* bytearray = (std::string*) stream->state;
        bytearray->append((const char*) buf, count);
        return true;
    }

    static bool write_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
    {
        fprintf(stderr, "arg=%p\n", *arg);
        std::string* value = (std::string*) *arg;
        fprintf(stderr, "HEY HEY! %s\n", value->c_str());

        if (!pb_encode_tag_for_field(stream, field))
            return false;

        return pb_encode_string(stream, (uint8_t*) value->c_str(), value->size());
    }

    virtual bool describe(connector::Component* component,
                          std::string* errorMessage)
    {
        fprintf(stderr, "HEY!\n");

        compact_descriptor_Component nano_component = {};
        std::string name = "Module.eg_nano";
        nano_component.name.funcs.encode = &write_string;
        nano_component.name.arg = &name;

        std::string data;
        pb_ostream_t stream = { &ostream_callback, &data, 65536, 0 };
        pb_encode(&stream, compact_descriptor_Component_fields, &nano_component);

        compact_descriptor::Component comp;
        comp.ParseFromString(data);
        
        std::string dump;
        io::StringOutputStream output(&dump);
        TextFormat::Print(comp, &output);
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("component=" << dump));

        // component->ParseFromString(data);

        return true;
    }
};

class StubRackDriver : public RackDriver
{
public:
    StubRackDriver(const char* subtype)
    {
        m_subtype = subtype;
    }
    
    bool discover(std::list<ModuleDriver*>* modulesList)
    {
        const static std::string fname = "StubRackDriver::discover()";

        ModuleDriver* driver;
        if (m_subtype == "simple") {
            driver = new StubModuleDriver();
        }
        else if (m_subtype == "nanopb") {
            driver = new NanopbStubModuleDriver();
        }
        else {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << m_subtype << ": unknown subtype"));
            return false;
        }
        modulesList->push_back(driver);
        return true;
    }

private:
    std::string m_subtype;
};

RackDriver* RackDriver::create(const char* rackURL)
{
    const static std::string fname = "RackDriver::create()";

    RackDriver* rackDriver = NULL;
    if (strncasecmp(rackURL, "file:", 5) == 0) {
        // url = file:/<rackDir>
        const char* ptr = rackURL + 5;
        if (*ptr == '/') {
            ++ptr;
        }
        rackDriver = new FileRackDriver(ptr);
    }
#ifdef __linux
    else if (strncasecmp(rackURL, "i2c://", 6) == 0) {
        // url = i2c://<deviceName>/<i2c_address>
        char* ptr = strdup(rackURL + 6);
        const char* tmp = strtok(ptr, "/");
        if (tmp == NULL) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << rackURL << ": device name is missing"));
            return NULL;
        }
        std::string deviceName = tmp;
        tmp = strtok(NULL, "/");
        if (tmp == NULL) {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << rackURL << ": i2c address is missing"));
            return NULL;
        }
        std::string address(tmp);
        free(ptr);
        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": " << deviceName << ", " << address));
        rackDriver = new I2CRackDriver(deviceName, address);
    }
#endif
    else if (strncasecmp(rackURL, "stub:", 5) == 0) {
        // url = stub:<type>
        rackDriver = new StubRackDriver(rackURL + 5);
    }
    else {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << rackURL << ": unknown protocol"));
    }

    return rackDriver;
}
