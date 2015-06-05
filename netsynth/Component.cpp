#include "Component.h"

#include "connector.pb.h"

const char* Component::componentTypes[] = {
    "Rack.",
    "Module.",
    "Knob.",
    "Selector.",
    "Port.",
    "Port.",
    "Port.",
    "Port.",
};
const int Component::NumComponentTypes = 8;

const char* Component::attrTypes[] = {
    "value",      // Attribute::Value
    "scale",      // Attribute::Scale
    "choices",    // Attribute::Choices
    "wireId",     // Attribute::WireId
    "direction",  // Attribute::Direction
    "signal",     // Attribute::Signal
    "moduleType", // Attribute::ModuleType
    "value",      // Attribute::SelectorIndex
};
const int Component::NumAttributeTypes = 8;

const char* Component::directionInput = "INPUT";
const char* Component::directionOutput = "OUTPUT";
const char* Component::signalValue = "value";
const char* Component::signalGate = "note";

Component::Component()
    : id(0), parent(NULL)
{}

Component::~Component()
{
    std::vector<Component*>::iterator it = subComponents.begin();
    std::vector<Component*>::iterator end = subComponents.end();
    for (; it != end; ++it) {
        delete *it;
    }
}

const std::string&
Component::getName()
{
    return name;
}

const std::string&
Component::getFullName() {
    return fullName;
}

bool
Component::hasAttribute(const std::string& name)
{
    std::map<std::string, AttributeValue>::iterator it = attributes.find(name);
    return (it != attributes.end());
}

const AttributeValue*
Component::getAttribute(const std::string& name)
{
    std::map<std::string, AttributeValue>::iterator it = attributes.find(name);
    if (it != attributes.end()) {
        return &it->second;
    }
    else {
        return NULL;
    }
}

bool
Component::setAttribute(const std::string& name, int value, bool force)
{
    if (force) {
        attributes[name].setInt(value);
    }
    else {
        std::map<std::string, AttributeValue>::iterator it = attributes.find(name);
        if (it == attributes.end()) {
            return false;
        }
        it->second.setInt(value);
    }
    return true;
}

Component*
Component::findSubComponent(const std::string& name)
{
    std::map<std::string, Component*>::iterator it = subComponentsDict.find(name);
    return (it != subComponentsDict.end()) ? it->second : NULL;
}

Component*
Component::create(const compact_descriptor::Component& componentDesc,
                  std::map<int, Component*>* idTable)
{
    Component* component = new Component();
    component->name = componentDesc.name();
    compact_descriptor::Component::Type componentType = componentDesc.type();
    component->fullName = componentTypes[componentType];
    component->fullName += component->name;
    component->id = componentDesc.id();

    if (idTable != NULL && component->id > 0) {
        (*idTable)[component->id] = component;
    }

    int numAttributes = componentDesc.attribute_size();
    for (int iattr = 0; iattr < numAttributes; ++iattr) {
        const compact_descriptor::Attribute& attribute = componentDesc.attribute(iattr);
        compact_descriptor::Attribute::Type type = attribute.type();
        if (type < NumAttributeTypes) {
            AttributeValue value;

            std::string name = attrTypes[type];
            value.attributeType = type;
            value.id = attribute.id();

            if (attribute.has_ivalue()) {
                value.setInt(attribute.ivalue());
            }

            int nstr = attribute.svalue_size();
            for (int istr = 0; istr < nstr; ++istr) {
                value.addString(attribute.svalue(istr));
            }

            component->attributes[name] = value;
        }
    }

    // Special attributes
    const char* direction = NULL;
    const char* signal = NULL;
    switch (componentType) {
    case compact_descriptor::Component::ValueInputPort:
        direction = directionInput;
        signal = signalValue;
        break;
    case compact_descriptor::Component::ValueOutputPort:
        direction = directionOutput;
        signal = signalValue;
        break;
    case compact_descriptor::Component::NoteInputPort:
        direction = directionInput;
        signal = signalGate;
        break;
    case compact_descriptor::Component::NoteOutputPort:
        direction = directionOutput;
        signal = signalGate;
        break;
    default:
        // do nothing
        break;
    }

    if (direction != NULL) {
        component->attributes[attrTypes[compact_descriptor::Attribute::Direction]].svalue.push_back(direction);
    }
    if (signal != NULL) {
        component->attributes[attrTypes[compact_descriptor::Attribute::Signal]].svalue.push_back(signal);
    }

    const char* attrName = attrTypes[compact_descriptor::Attribute::ModuleType];
    if (componentType == compact_descriptor::Component::Module &&
        component->attributes.find(attrName) == component->attributes.end()) {
        component->attributes[attrName].svalue.push_back(component->name);
    }

    int numSubComponent = componentDesc.sub_component_size();
    for (int ic = 0; ic < numSubComponent; ++ic) {
        const compact_descriptor::Component& scDescriptor = componentDesc.sub_component(ic);
        Component* subComponent = Component::create(scDescriptor, idTable);
        component->subComponentsDict[subComponent->fullName] = subComponent;
        component->subComponents.push_back(subComponent);
        subComponent->parent = component;
    }

    return component;
}

void
Component::remove()
{
    if (parent != NULL) {
        parent->subComponentsDict.erase(getFullName());
        std::vector<Component*>::iterator it = parent->subComponents.begin();
        std::vector<Component*>::iterator end = parent->subComponents.end();
        for (; it != end; ++it) {
            if (*it == this) {
                parent->subComponents.erase(it);
                break;
            }
        }
    }
}

bool
Component::convertToProtocolBuf(connector::Component* pbComponent, std::string* errorMessage)
{
    pbComponent->set_name(fullName);
    // pbComponent->set_id(id);

    // attributes
    std::map<std::string, AttributeValue>::iterator it_attr = attributes.begin();
    std::map<std::string, AttributeValue>::iterator it_attrEnd = attributes.end();

    for (; it_attr != it_attrEnd; ++it_attr) {
        
        AttributeValue& value = it_attr->second;

        // make NIL wireId invisible.
        if (value.attributeType == compact_descriptor::Attribute::WireId && value.ivalue == 0) {
            continue;
        }

        // Convert attribute to protocol buffers
        connector::Attribute* pbAttribute = pbComponent->add_attribute();
        pbAttribute->set_name(it_attr->first);
        connector::Value* pbValue = pbAttribute->mutable_value();
        if (value.has_ivalue) {
            pbValue->set_ivalue(value.ivalue);
        }
        else if (value.svalue.size() == 1) {
            pbValue->set_svalue(value.svalue[0]);
        }
        else if (value.svalue.size() > 1) {
            for (size_t iv = 0; iv < value.svalue.size(); ++iv) {
                connector::Value* avalue = pbValue->add_avalue();
                avalue->set_svalue(value.svalue[iv]);
            }
        }
    }

    // sub components
    for (size_t i = 0; i < subComponents.size(); ++i) {
        if (!subComponents[i]->convertToProtocolBuf(pbComponent->add_sub_component(), errorMessage)) {
            return false;
        }
    }

    return true;
}
