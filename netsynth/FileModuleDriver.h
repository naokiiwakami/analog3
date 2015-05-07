#ifndef _filemoduledriver_h_
#define _filemoduledriver_h_

#include "ModuleDriver.h"

class FileRackDriver : public RackDriver
{
 public:
    FileRackDriver(const char* dirName);

    bool discover(std::list<ModuleDriver*>* modulesList);

 private:
    const char* m_dirName;
};

class FileModuleDriverData;

class FileModuleDriver : public ModuleDriver
{
public:
    FileModuleDriver(const std::string& fileName);
    virtual ~FileModuleDriver();

    const std::string& getName();

    const std::string& getFullName();

    void read(connector::Component* component);

    bool modifyAttribute(const connector::Request& request,
                         std::string* errorMessage);

    bool addSubComponent(const connector::Request& request,
                         std::string* errorMessage);

    bool removeSubComponent(const connector::Request& request,
                            std::string* errorMessage);

private:
    void makeDocument();
    bool flush(std::string* errorMessage);

    std::string m_fileName;
    FileModuleDriverData* m_data;
};

#endif
