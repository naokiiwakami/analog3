#ifndef _i2cmoduledriver_h_
#define _i2cmoduledriver_h_

#include "ModuleDriver.h"

class I2CRackDriver : public RackDriver
{
public:
    I2CRackDriver(const std::string& deviceName, const std::string& slaveAddress);

    bool discover(std::list<ModuleDriver*>* modulesList);

protected:
    bool setupAddress();
    bool sendCommand(const std::string& command, std::string* response = NULL);

protected:
    std::string m_device;
    int m_slaveAddress;
    int m_fd;
};

class I2CModuleDriverData;

class I2CModuleDriver : public ModuleDriver
{
public:
    I2CModuleDriver(const std::string& deviceName, const std::string& slaveAddress);
    virtual ~I2CModuleDriver();

    const std::string& getName();

    const std::string& getFullName();

    bool describe(connector::Component* component,
                  std::string* errorMessage);

    bool modifyAttribute(const connector::Request& request,
                         std::string* errorMessage);

    bool addSubComponent(const connector::Request& request,
                         std::string* errorMessage);

    bool removeSubComponent(const connector::Request& request,
                            std::string* errorMessage);

};

#endif
