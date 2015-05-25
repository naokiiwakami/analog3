#include "Component.h"

#include "connector.pb.h"
#include "compact_descriptor.pb.h"

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
    "value",
    "scale",
    "choices",
    "wireId",
    "direction",
    "signal",
    "moduleType",
};
const int Component::NumAttributeTypes = 7;

const char* Component::directionInput = "INPUT";
const char* Component::directionOutput = "OUTPUT";
const char* Component::signalValue = "value";
const char* Component::signalGate = "note";

Component::Component()
    : id(0)
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
    compact_descriptor::Component_Type componentType = componentDesc.type();
    component->fullName = componentTypes[componentType];
    component->fullName += component->name;
    component->id = componentDesc.id();

    if (idTable != NULL && component->id > 0) {
        (*idTable)[component->id] = component;
    }

    int numAttributes = componentDesc.attribute_size();
    for (int iattr = 0; iattr < numAttributes; ++iattr) {
        const compact_descriptor::Attribute& attribute = componentDesc.attribute(iattr);
        if (attribute.type() < NumAttributeTypes) {
            std::string name = attrTypes[attribute.type()];
            AttributeValue value;
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
    case compact_descriptor::Component_Type_ValueInputPort:
        direction = directionInput;
        signal = signalValue;
        break;
    case compact_descriptor::Component_Type_ValueOutputPort:
        direction = directionOutput;
        signal = signalValue;
        break;
    case compact_descriptor::Component_Type_GateInputPort:
        direction = directionInput;
        signal = signalGate;
        break;
    case compact_descriptor::Component_Type_GateOutputPort:
        direction = directionOutput;
        signal = signalGate;
        break;
    default:
        // do nothing
        break;
    }

    if (direction != NULL) {
        component->attributes[attrTypes[compact_descriptor::Attribute_Type_Direction]].svalue.push_back(direction);
    }
    if (signal != NULL) {
        component->attributes[attrTypes[compact_descriptor::Attribute_Type_Signal]].svalue.push_back(signal);
    }

    const char* attrName = attrTypes[compact_descriptor::Attribute_Type_ModuleType];
    if (componentType == compact_descriptor::Component_Type_Module &&
        component->attributes.find(attrName) == component->attributes.end()) {
        component->attributes[attrName].svalue.push_back(component->name);
    }

    int numSubComponent = componentDesc.sub_component_size();
    for (int ic = 0; ic < numSubComponent; ++ic) {
        const compact_descriptor::Component& scDescriptor = componentDesc.sub_component(ic);
        Component* subComponent = Component::create(scDescriptor, idTable);
        component->subComponentsDict[subComponent->fullName] = subComponent;
        component->subComponents.push_back(subComponent);
    }

    return component;
}

bool
Component::convertToProtocolBuf(connector::Component* pbComponent, std::string* errorMessage)
{
    pbComponent->set_name(fullName);
    pbComponent->set_id(id);

    // attributes
    std::map<std::string, AttributeValue>::iterator it_attr = attributes.begin();
    std::map<std::string, AttributeValue>::iterator it_attrEnd = attributes.end();
    for (; it_attr != it_attrEnd; ++it_attr) {
        connector::Attribute* pbAttribute = pbComponent->add_attribute();
        pbAttribute->set_name(it_attr->first);
        connector::Value* pbValue = pbAttribute->mutable_value();
        AttributeValue& value = it_attr->second;
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
