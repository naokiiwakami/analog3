#ifndef _i2cmoduledriver_h_
#define _i2cmoduledriver_h_

#include "ModuleDriver.h"

class I2cRackDriver : public RackDriver
{
public:
    I2cRackDriver(const std::string& deviceName);

    bool discover(std::list<ModuleDriver*>* modulesList);

protected:
    bool setupAddress(int slaveAddress);
    bool sendCommand(const std::string& command, std::string* response = NULL);

protected:
    std::string m_device;
    int m_fd;
};

class I2cModuleDriverData;

class I2cModuleDriver : public ModuleDriver
{
public:
    I2cModuleDriver();
    virtual ~I2cModuleDriver();

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

    I2cModuleDriverData* m_data;
};

#endif
