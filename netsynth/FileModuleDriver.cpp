#include "FileModuleDriver.h"
#include "ModuleRecognitionException.h"

#include "connector.pb.h"

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include <dirent.h>
#include <fcntl.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <stack>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

using namespace rapidjson;

static log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("FileModuleDriver"));

FileRackDriver::FileRackDriver(const char* dirName)
    : m_dirName(dirName)
{
}

bool FileRackDriver::discover(std::list<ModuleDriver*>* modulesList)
{
    static const std::string fname = "FileRackDriver::discover()";
    DIR *dp = opendir(m_dirName);
    if (dp == NULL) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << m_dirName << ": no such directory"));
        return false;
    }

    bool result = false;
    try {
        struct dirent *ep;
        std::map<std::string, ModuleDriver*> sortMap;
        while ((ep = readdir(dp)) != NULL) {
            char* ptr = strcasestr(ep->d_name, ".json");
            if (ptr == NULL || *(ptr + 5) != '\0') {
                // not a target file
                continue;
            }
            if (ep->d_type != DT_REG) {
                continue;
            }
            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("fileName=" << ep->d_name));
            std::string fileName = m_dirName;
            fileName += "/";
            fileName += ep->d_name;
            ModuleDriver* moduleDriver = new FileModuleDriver(fileName);
            sortMap[fileName] = moduleDriver;
        }

        for (std::map<std::string, ModuleDriver*>::iterator it = sortMap.begin();
             it != sortMap.end(); ++it) {
            modulesList->push_back(it->second);
        }
        result = true;
    }
    catch (ModuleRecognitionException& ex) {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(fname << ": " << ex.what()));
    }
    closedir(dp);
    return result;
}

class FileModuleDriverData
{
public:
    FileModuleDriverData()
        : m_document(NULL)
    {}

    ~FileModuleDriverData() {
        delete m_document;
    }

    Document* m_document;
    std::string m_name;
    std::string m_fullName;
};

class SaxEventToProtocolBuf
{
public:
    SaxEventToProtocolBuf(connector::Component* rootComponent) {
        this->rootComponent = rootComponent;
    }

    bool Null() {
        // Do nothing
        return true;
    }
    bool Bool(bool b) {
        // This won't come?
        return true;
    }
    bool Int(int i) {
        return setInt(i);
    }
    bool Uint(unsigned u) {
        return setInt(u);
    }
    bool Int64(int64_t i) {
        return setInt(i);
    }

    bool Uint64(uint64_t u) {
        return setInt(u);
    }
    bool Double(double d) {
        connector::Value* value = currentValue();
        if (value != NULL) {
            value->set_dvalue(d);
            return true;
        }
        else {
            return false;
        }
    }
    bool String(const char* str, SizeType length, bool copy) {
        connector::Value* value = currentValue();
        if (value != NULL) {
            value->set_svalue(std::string(str, length));
            return true;
        }
        else {
            return false;
        }
    }
    bool StartObject() {
        if (componentStack.empty()) {
            // push dummy
            componentStack.push(NULL);
        }
        else {
            if (savedKey.empty()) {
                return false;
            }
            if (componentStack.top() == NULL) {
                componentStack.push(rootComponent);
            }
            else {
                componentStack.push(componentStack.top()->add_sub_component());
            }
            componentStack.top()->set_name(savedKey);
            savedKey.clear();
        }
        return true;
    }
    bool Key(const char* str, SizeType length, bool copy) {
        if (!savedKey.empty()) {
            return false;
        }
        savedKey.assign(str, length);
        return true;
    }
    bool EndObject(SizeType memberCount) {
        componentStack.pop();
        return true;
    }
    bool StartArray() {
        if (savedKey.empty()) {
            return false;
        }
        if (componentStack.top() == NULL) {
            // object nest level mismatch
            return false;
        }
        connector::Attribute* attribute = componentStack.top()->add_attribute();
        attribute->set_name(savedKey);
        savedKey.clear();
        arrayStack.push(attribute->mutable_value());
        return true;
    }
    bool EndArray(SizeType elementCount) {
        arrayStack.pop();
        return true;
    }

private:
    connector::Value* currentValue() {
        if (arrayStack.empty()) {
            if (savedKey.empty()) {
                return NULL;
            }
            if (componentStack.top() == NULL) {
                // object nest level mismatch
                return NULL;
            }
            connector::Attribute* attribute = componentStack.top()->add_attribute();
            attribute->set_name(savedKey);
            savedKey.clear();
            return attribute->mutable_value();
        }
        else {
            return arrayStack.top()->add_avalue();
        }
    }

    bool setInt(long i) {
        connector::Value* value = currentValue();
        if (value != NULL) {
            value->set_ivalue(i);
            return true;
        }
        else {
            return false;
        }
    }

    connector::Component* rootComponent;
    std::stack<connector::Component*> componentStack;
    std::stack<connector::Value*> arrayStack;

    std::string savedKey;
};

FileModuleDriver::FileModuleDriver(const std::string& fileName)
{
    m_fileName= fileName;
    m_data = new FileModuleDriverData();

    makeDocument();

    Document* doc = m_data->m_document;

    if (!doc->IsObject()) {
        ModuleRecognitionException ex("NOT AN OBJECT!");
        throw ex;
    }

    // Find module name
    Value::MemberIterator itr = doc->MemberBegin();
    Value::MemberIterator end = doc->MemberEnd();
    for (; itr != end; ++itr) {
        const char* szName = itr->name.GetString();
        const char* ptr = strstr(szName, "Module.");
        if (ptr == szName && *(ptr + 7) != '\0') {
            m_data->m_name = ptr + 7;
            m_data->m_fullName = ptr;
            break;
        }
    }

    if (m_data->m_name.empty()) {
        ModuleRecognitionException ex("mandatory attribute \"name\" is missing or invalid.");
        throw ex;
    }
}

FileModuleDriver::~FileModuleDriver()
{
    delete m_data;
}

const std::string&
FileModuleDriver::getName()
{
    return m_data->m_name;
}

const std::string&
FileModuleDriver::getFullName()
{
    return m_data->m_fullName;
}

void
FileModuleDriver::makeDocument()
{
    int fd = open(m_fileName.c_str(), O_RDONLY);
    if (fd < 0) {
        throw ModuleRecognitionException("File open error");
    }
    struct stat fileInfo;
    fstat(fd, &fileInfo);

    // Read file and make Json data byte array
    char* buf = new char[fileInfo.st_size + 1];
    if (::read(fd, buf, fileInfo.st_size) < 0) {
        close(fd);
        delete[] buf;
        std::string message = "File read error: ";
        message += m_fileName;
        throw ModuleRecognitionException(message);
    }
    buf[fileInfo.st_size] = '\0';
    close(fd);

    // Make the Json DOM document
    Document* document = new Document();
    document->Parse(buf);
    if (document->HasParseError()) {
        char errText[4096];
        unsigned int offset = document->GetErrorOffset();
        std::string message;
        snprintf(errText, sizeof(errText), "Error (offset %u): %s\n",
                 offset,
                 GetParseError_En(document->GetParseError()));
        message = errText;
        size_t start = offset;
        while (start != 0 && buf[start] != '\n') {
            --start;
        }
        snprintf(errText, 256, "%s\n", buf + start + 1);
        message += errText;
        ModuleRecognitionException ex(message);
        delete[] buf;
        delete document;
        throw ex;
    }
    delete[] buf;

    m_data->m_document = document;
}

bool
FileModuleDriver::describe(connector::Component* component,
                           std::string* errorMessage)
{
    // Convert the Json document to protocol buffer
    SaxEventToProtocolBuf handler(component);
    if (!m_data->m_document->Accept(handler)) {
        char errText[4096];
        snprintf(errText, sizeof(errText), "Error: %s\n",
                 GetParseError_En(m_data->m_document->GetParseError()));
        *errorMessage = errText;
        return false;
    }

    return true;
}

bool
FileModuleDriver::flush(std::string* errorMessage)
{
    static const std::string fname = "FileModuleDriver::flush()";

    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    writer.SetIndent(' ', 2);
    if (!m_data->m_document->Accept(writer)) {
        *errorMessage = fname + ": serialization error";
        return false;
    }

    // write to file
    int fd = open(m_fileName.c_str(), O_WRONLY | O_TRUNC);
    if (fd < 0) {
        *errorMessage = fname + ": " + m_fileName + ": file open error.";
        return false;
    }

    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": new JSON=" << buffer.GetString()));

    if (::write(fd, buffer.GetString(), buffer.GetSize()) < 0) {
        *errorMessage = fname + ": write error: ";
        *errorMessage += strerror(errno);
        close(fd);
        return false;
    }

    close(fd);

    return true;
}

bool
FileModuleDriver::modifyAttribute(const connector::Request& request,
                                  std::string* errorMessage)
{
    static const std::string fname = "FileModuleDriver::modifyAttribute()";
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": module " << m_data->m_name << " enter"));

    int num_paths = request.path_size();
    if (num_paths < 2) {
        *errorMessage = fname + ": Invalid request: target path is incomplete";
        return false;
    }

    // Find target component
    Value* component = m_data->m_document;
    Value::MemberIterator it;
    const std::string* componentName = NULL;
    for (int index = 0; index < num_paths; ++index) {
        componentName = &request.path(index);
        if (!component->IsObject()) {
            *errorMessage = fname + ": " + *componentName + ": invalid component";
            return false;
        }
        it = component->FindMember(componentName->c_str());
        if (it == component->MemberEnd()) {
            *errorMessage = fname + ": target component " + *componentName + " not found";
            return false;
        }
        component = &it->value;
    }
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": component=" << *componentName));

    // Set or unset attribute
    const std::string& attributeName = request.attribute().name();
    
    switch (request.command()) {
    case connector::Request_Command_SET_ATTRIBUTE: {
        // get new value
        const connector::Value* inputValue = request.attribute().has_value() ? &request.attribute().value() : NULL;
        Value newValue;
        if (inputValue->has_ivalue()) {
            newValue.SetInt(inputValue->ivalue());
        }
        else if (inputValue->has_svalue()) {
            const std::string& svalue = inputValue->svalue();
            newValue.SetString(svalue.c_str(), svalue.size(), m_data->m_document->GetAllocator());
        }

        // Determine if this is addition or update.
        Value::MemberIterator it_attr = component->FindMember(attributeName.c_str());
        bool isUpdate = (it_attr != component->MemberEnd());

        // Update or add new value
        if (isUpdate) {
            Value& value = it_attr->value;
            if (value.IsInt()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": old value=" << value.GetInt()));
            }
            else if (value.IsString()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": old value=" << value.GetString()));
            }

            value = newValue;

            if (value.IsInt()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": new value=" << value.GetInt()));
            }
            else if (value.IsString()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": new value=" << value.GetString()));
            }
        }
        else {
            Value name(attributeName.c_str(), attributeName.size(), m_data->m_document->GetAllocator());
            component->AddMember(name, newValue, m_data->m_document->GetAllocator());
            if (newValue.IsInt()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": new value=" << newValue.GetInt()));
            }
            else if (newValue.IsString()) {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": new value=" << newValue.GetString()));
            }
        }
        break;
    }
    case connector::Request_Command_UNSET_ATTRIBUTE: {
        Value::MemberIterator it_attr = component->FindMember(attributeName.c_str());
        if (it_attr == component->MemberEnd()) {
            *errorMessage = fname + ": " + *componentName + ": " + attributeName + ": attribute not found";
            return false;
        }
        component->EraseMember(it_attr);
    }
    default:
        // Do nothing
        ;
    }

    return flush(errorMessage);
}

bool
FileModuleDriver::addSubComponent(const connector::Request& request,
                                  std::string* errorMessage)
{
    static const std::string fname = "FileModuleDriver::addSubComponent()";
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": module " << m_data->m_name << " enter"));

    // Resolve target component
    int pathLevels = request.path_size();
    if (pathLevels < 2) {
        *errorMessage = fname + ": Invalid request: target path is incomplete";
        return false;
    }
    int index = 1;
    int num_paths = request.path_size();
    Value* component = &(*m_data->m_document)[m_data->m_fullName.c_str()];
    while (index < num_paths) {
        const std::string& componentName = request.path(index);
        Value::MemberIterator it = component->FindMember(componentName.c_str());
        if (it == component->MemberEnd()) {
            *errorMessage = fname + ": target component " + componentName + " not found";
            return false;
        }
        component = &it->value;
        if (!component->IsObject()) {
            *errorMessage = fname + ": " + componentName + ": invalid component";
            return false;
        }
        ++index;
    }

    // Convert the component in the request to Json document.
    Value newComponent;
    newComponent.SetObject();
    const std::string& componentName = request.component().name();
    int num_attributes = request.component().attribute_size();
    for (index = 0; index < num_attributes; ++index) {
        const connector::Attribute& attribute = request.component().attribute(index);
        if (!attribute.has_value()) {
            *errorMessage = fname + ": component=" + componentName + ": attribute=" + attribute.name() + ": has no value";
            return false;
        }
        Value attributeName(attribute.name().c_str(), attribute.name().size(), m_data->m_document->GetAllocator());
        if (attribute.value().has_ivalue()) {
            newComponent.AddMember(attributeName, attribute.value().ivalue(), m_data->m_document->GetAllocator());
        }
        else if (attribute.value().has_svalue()) {
            const std::string& svalue = attribute.value().svalue();
            Value attributeValue(svalue.c_str(), svalue.size(), m_data->m_document->GetAllocator());
            newComponent.AddMember(attributeName, attributeValue, m_data->m_document->GetAllocator());
        }
    }
    Value newComponentName(componentName.c_str(), componentName.size(), m_data->m_document->GetAllocator());
    component->AddMember(newComponentName, newComponent, m_data->m_document->GetAllocator());

    return flush(errorMessage);
}

bool
FileModuleDriver::removeSubComponent(const connector::Request& request,
                                     std::string* errorMessage)
{
    static const std::string fname = "FileModuleDriver::removeSubComponent()";
    LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(fname << ": module " << m_data->m_name << " enter"));

    // Resolve target component
    int num_paths = request.path_size();
    Value* component = m_data->m_document;
    const std::string* componentName = NULL;
    for (int index = 0; index < num_paths; ++index) {
        componentName = &request.path(index);
        if (!component->IsObject()) {
            *errorMessage = fname + ": " + *componentName + ": invalid component";
            return false;
        }
        Value::MemberIterator it = component->FindMember(componentName->c_str());
        if (it == component->MemberEnd()) {
            *errorMessage = fname + ": target component " + *componentName + " not found";
            return false;
        }
        component = &it->value;
    }

    // Find the module to be removed
    const std::string& targetName = request.component().name();
    Value::MemberIterator it = component->FindMember(targetName.c_str());
    if (it == component->MemberEnd()) {
        *errorMessage = fname + ": " + targetName + ": subcomponent to be deleted not found";
        return false;
    }

    component->EraseMember(it);

    return flush(errorMessage);
}
