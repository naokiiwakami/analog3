#ifndef _filemoduledriver_h_
#define _filemoduledriver_h_

#include <string>

namespace connector {
    class Request;
    class Component;
}

class FileModuleDriverData;

class FileModuleDriver
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
