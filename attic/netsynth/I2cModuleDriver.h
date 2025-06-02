#ifndef _i2cmoduledriver_h_
#define _i2cmoduledriver_h_

#include "ModuleDriver.h"

class I2cRackDriver : public RackDriver
{
public:
    I2cRackDriver(const std::string& deviceName);

    bool discover(std::list<ModuleDriver*>* modulesList);

    /**
     * Set slave address to the opened I2C session.
     *
     * @param slaveAddress Slave address.  The range should be greater than 0 and smaller than 128.
     *
     * @return True on successful execution.  False otherwise.
     */
    bool setupAddress(int slaveAddress);

    /**
     * Send command to the device via I2C and receives the response if necessary.
     * The target slave address must be set before this method is called using
     * setupAddress().
     * @param command Command byte array.
     * @param response Ponter to a byte array where you receive the response.
     *                 The method does not read response from the slave when the
     *                 response pointer is NULL.
     *
     * @return Tru on successful execution.  False otherwise.
     */
    bool sendCommand(const std::string& command, std::string* response = NULL);

protected:
    std::string m_device;
    int m_fd;
};

class I2cModuleDriverData;
class Component;

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

protected:
    Component* resolveComponent(const connector::Request& request,
                                std::string* errorMessage);

};

#endif
