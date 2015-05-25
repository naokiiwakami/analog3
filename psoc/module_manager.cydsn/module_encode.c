/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "module_encode.h"

static const uint16_t SCALE = 1023;

static bool write_cstring(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    const char* text = (const char*) *arg;
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }
    return pb_encode_string(stream, (uint8_t*) text, strlen(text));
}

static bool write_string_array(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    char** string_list = (char**) *arg;
    
    int ii;
    for (ii = 0; string_list[ii] != NULL; ++ii) {
        char* text = string_list[ii];
        if (!pb_encode_tag_for_field(stream, field) ||
            !pb_encode_string(stream, (uint8_t*) text, strlen(text)))
        {
            return false;
        }
    }
    
    return true;
}

static bool write_knob_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint16_t ivalue = *(uint16_t*) *arg;

    compact_descriptor_Attribute attr_value = {};
    attr_value.type = compact_descriptor_Attribute_Type_Value;
    attr_value.ivalue = ivalue;
    attr_value.has_ivalue = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attr_value))
    {
        return false;
    }

    compact_descriptor_Attribute attr_scale = {};
    attr_scale.type = compact_descriptor_Attribute_Type_Scale;
    attr_scale.ivalue = SCALE;
    attr_scale.has_ivalue = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attr_scale))
    {
        return false;
    }

    return true;
}

static bool write_selector_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    SelectorAttributes* attr = (SelectorAttributes*) *arg;

    // encode index
    compact_descriptor_Attribute attr_value = {};
    attr_value.type = compact_descriptor_Attribute_Type_Value;
    attr_value.ivalue = *attr->index;
    attr_value.has_ivalue = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attr_value))
    {
        return false;
    }
    
    // encode choices
    compact_descriptor_Attribute attr_choices = {};
    attr_choices.type = compact_descriptor_Attribute_Type_Choices;
    attr_choices.svalue.funcs.encode = &write_string_array;
    attr_choices.svalue.arg = attr->choices;

    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attr_choices))
    {
        return false;
    }

    return true;
}

static bool write_port_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    uint8_t ivalue = *(uint8_t*) *arg;
    
    // put wire Id
    if (ivalue > 0) {
        compact_descriptor_Attribute attr_wireId = {};
        attr_wireId.type = compact_descriptor_Attribute_Type_WireId;
        attr_wireId.ivalue = ivalue;
        attr_wireId.has_ivalue = true;
    
        if (!pb_encode_tag_for_field(stream, field) ||
            !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attr_wireId))
        {
            return false;
        }
    }

    return true;
}

bool write_component(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    ComponentDef* def = (ComponentDef*) *arg;
    
    Component* components = def->components;
    ComponentNode* nodes = def->nodes;
    uint8_t index = def->index;
    
    ComponentDef subDef;
    subDef.components = components;
    subDef.nodes = nodes;

    while (components[index].name != NULL) {
        compact_descriptor_Component component = {};
        component.name.funcs.encode = &write_cstring;
        component.name.arg = (void*) components[index].name;
        component.type = components[index].type;
        component.id = index + 1;
        
        component.attribute.arg = components[index].attributes;
        if (component.attribute.arg != NULL) {
            switch (components[index].type) {
            case Knob:
                component.attribute.funcs.encode = &write_knob_attributes;
                break;
            case Selector:
                component.attribute.funcs.encode = &write_selector_attributes;
                break;
            case ValueInputPort:
            case ValueOutputPort:
            case GateInputPort:
            case GateOutputPort:
                component.attribute.funcs.encode = &write_port_attributes;
                break;
            };
        }

        if (components[nodes[index].sub].name != NULL) {
            subDef.index = nodes[index].sub;
            component.sub_component.funcs.encode = &write_component;
            component.sub_component.arg = &subDef;
        }

        if (!pb_encode_tag_for_field(stream, field))
            return false;
    
        if (!pb_encode_submessage(stream, compact_descriptor_Component_fields, &component)) {
            return false;
        }
        
        index = nodes[index].next;
    }

    return true;
}

/* [] END OF FILE */
