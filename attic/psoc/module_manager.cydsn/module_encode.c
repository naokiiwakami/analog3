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
    char** string_list = *(char***) *arg;
    
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

typedef struct _AttributeDef {
    uint8_t* data;
    AttributeInfo* info;
} AttributeDef;

static bool write_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    AttributeDef* attrDef = (AttributeDef*) *arg;
    
    int i = 0;
    while (1) {
        compact_descriptor_Attribute attribute = {};

        uint8_t offset = attrDef->info[i].valueOffset;
        attribute.id = offset;
        attribute.has_id = true;

        uint8_t attributeType = attrDef->info[i].attributeType;
        attribute.type = attributeType;

        switch (attributeType) {
        case AttributeTypeValue:
        case AttributeTypeScale: {
            uint16_t ivalue = *(uint16_t*) &attrDef->data[offset];
            attribute.ivalue = ivalue;
            attribute.has_ivalue = true;
            break;
        }
        case AttributeTypeSelectorIndex:
        case AttributeTypeWireId: {
            uint8_t ivalue = *(uint8_t*) &attrDef->data[offset];
            attribute.ivalue = ivalue;
            attribute.has_ivalue = true;
            break;
        }
        case AttributeTypeChoices: {
            attribute.svalue.funcs.encode = &write_string_array;
            const char*** svalue = (const char ***) &attrDef->data[offset];
            attribute.svalue.arg = svalue;
        }
        }

        if (!pb_encode_tag_for_field(stream, field) ||
            !pb_encode_submessage(stream, compact_descriptor_Attribute_fields, &attribute))
        {
            return false;
        }
        
        if (attrDef->info[i].next == 0) {
            break;
        }
        i += attrDef->info[i].next;

    }
    
    return true;
}

bool write_component(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    DeviceDescriptor* desc = (DeviceDescriptor*) *arg;
    
    Component* components = desc->components;

    uint8_t index = desc->index;
    
    DeviceDescriptor subDesc;
    subDesc.components = components;
    subDesc.attrs = desc->attrs;
    subDesc.data = desc->data;

    while (components[index].name != NULL) {
        compact_descriptor_Component component = {};
        component.name.funcs.encode = &write_cstring;
        component.name.arg = (void*) components[index].name;
        component.type = components[index].type;
        
        AttributeDef attributeDef;
        uint8_t attributeIndex = components[index].attributeIndex;
        if (attributeIndex != NA) {
            attributeDef.data = desc->data;
            attributeDef.info = &desc->attrs[attributeIndex];
        
            component.attribute.arg = &attributeDef;
            component.attribute.funcs.encode = &write_attributes;
        }

        if (components[components[index].sub].name != NULL) {
            subDesc.index = components[index].sub;
            component.sub_component.funcs.encode = &write_component;
            component.sub_component.arg = &subDesc;
        }

        if (!pb_encode_tag_for_field(stream, field))
            return false;
    
        if (!pb_encode_submessage(stream, compact_descriptor_Component_fields, &component)) {
            return false;
        }
        
        index = components[index].next;
    }

    return true;
}

/* [] END OF FILE */
