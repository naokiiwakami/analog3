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

#include <project.h>

#include <pb_encode.h>
#include <pb_decode.h>
#include "nano_compact_descriptor.pb.h"

#define Rack compact_descriptor_Component_Type_Rack
#define Module compact_descriptor_Component_Type_Module
#define Knob compact_descriptor_Component_Type_Knob
#define Selector compact_descriptor_Component_Type_Selector
#define ValueInputPort compact_descriptor_Component_Type_ValueInputPort
#define ValueOutputPort compact_descriptor_Component_Type_ValueOutputPort
#define NoteInputPort compact_descriptor_Component_Type_NoteInputPort
#define NoteOutputPort compact_descriptor_Component_Type_NoteOutputPort

#define AttributeTypeValue         compact_descriptor_Attribute_Type_Value
#define AttributeTypeScale         compact_descriptor_Attribute_Type_Scale
#define AttributeTypeChoices       compact_descriptor_Attribute_Type_Choices
#define AttributeTypeWireId        compact_descriptor_Attribute_Type_WireId
#define AttributeTypeDirection     compact_descriptor_Attribute_Type_Direction
#define AttributeTypeSignal        compact_descriptor_Attribute_Type_Signal
#define AttributeTypeModuleType    compact_descriptor_Attribute_Type_ModuleType
#define AttributeTypeSelectorIndex compact_descriptor_Attribute_Type_SelectorIndex

typedef struct _AttributeInfo {
    uint8_t valueOffset;
    int8_t attributeType /*: 6*/;
    uint8_t hasNext /*: 1*/;
} AttributeInfo;

typedef struct _SelectorAttributes {
    uint8_t* index;
    const char** choices;
} SelectorAttributes;

typedef struct _Component {
    const char* name;
    uint8_t type;
    // void* attributes;
    uint8_t offset;
} Component;

typedef struct _ComponentNode {
    uint8_t next;
    uint8_t sub;
} ComponentNode;
            
typedef struct _ComponentDef {
    Component* components;
    ComponentNode* nodes;
    AttributeInfo* attrs;
    uint8_t* data;
    uint8_t index;
} ComponentDef;

bool write_modules(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
bool write_component(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

/* [] END OF FILE */
