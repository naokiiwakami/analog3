#ifndef _component_h_
#define _component_h_

#include <string>
#include <vector>
#include <map>

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

namespace compact_descriptor {
    class Component;
}

namespace connector {
    class Component;
}

class Component
{
public:
    Component();

    virtual ~Component();

    const std::string& getName();

    const std::string& getFullName();

    static Component* create(const compact_descriptor::Component& componentDesc);

    bool convertToProtocolBuf(connector::Component* pbComponent, std::string* errorMessage);

private:
    std::string name;
    std::string fullName;
    int id;
    std::map<std::string, AttributeValue> attributes;
    std::map<std::string, Component*> subComponentsDict;
    std::vector<Component*> subComponents;

    static const char* componentTypes[];
    static const int NumComponentTypes;
    static const char* attrTypes[];
    static const int NumAttributeTypes;
    static const char* directionInput;
    static const char* directionOutput;
    static const char* signalValue;
    static const char* signalGate;
};

#endif // _component_h_
