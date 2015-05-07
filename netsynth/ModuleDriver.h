#ifndef _moduledriver_h_
#define _moduledriver_h_

#include <string>
#include <list>

namespace connector {
    class Request;
    class Component;
}

class ModuleDriver;

class RackDriver
{
 public:
    static RackDriver* create(const char* rackURL);
    
    virtual bool discover(std::list<ModuleDriver*>* modulesList) = 0;
};

class ModuleDriver
{
public:
    virtual const std::string& getName() = 0;

    virtual const std::string& getFullName() = 0;

    virtual void read(connector::Component* component) = 0;

    virtual bool modifyAttribute(const connector::Request& request,
                                 std::string* errorMessage) = 0;

    virtual bool addSubComponent(const connector::Request& request,
                                 std::string* errorMessage) = 0;

    virtual bool removeSubComponent(const connector::Request& request,
                                    std::string* errorMessage) = 0;
};

#endif
