#ifndef _component_h_
#define _component_h_

#include <string>
#include <vector>
#include <map>

#include "compact_descriptor.pb.h"

struct AttributeValue
{
    compact_descriptor::Attribute::Type attributeType;
    int ivalue;
    bool has_ivalue;
    std::vector<std::string> svalue;
    int id;

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

    static Component* create(const compact_descriptor::Component& componentDesc,
                             std::map<int, Component*>* idTable = NULL);

    const std::string& getName();

    const std::string& getFullName();

    int getId() {
        return id;
    }

    bool hasAttribute(const std::string& name);

    const AttributeValue* getAttribute(const std::string& name);

    bool setAttribute(const std::string& name, int value, bool force = true);

    Component* findSubComponent(const std::string& name);

    bool hasSubComponent() {
        return !subComponents.empty();
    }

    void addSubComponent(Component* sub);

    void remove();

    bool convertToProtocolBuf(connector::Component* pbComponent, std::string* errorMessage);

private:
    std::string name;
    std::string fullName;
    int id;
    std::map<std::string, AttributeValue> attributes;

    Component* parent;
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
