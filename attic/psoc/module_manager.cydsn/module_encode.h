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

/*
 * Struct used for describing attributes.
 * Strategy:
 *   All attributes should be in a single memory block in static RAM area.
 *   This struct is used for describing each attribute in the block.
 *   All attribute descriptors should be in an AttributeInfo array.
 *   Each descriptor entry gives following information:
 *     - Relative position of the attribute value in the data block.
 *     - Attribute type that determines data type of the value.
 *     - Relative position from current AttributeInfo entry in the array to
 *       next attribute for the component. 0 terminates the link.
 *
 * The attribute description table (i.e., AttributeInfo array) is necessary only
 * during describing the modules implemented in the device. So the table may be
 * destroyed after the describe operation is completed.
 */
typedef struct _AttributeInfo {
    uint8_t valueOffset;  // offset of the value in the data block.
    int8_t attributeType; // compact descriptor attribute type
    uint8_t next;         // relative position to the next attribute. value of 0 terminates the link.
} AttributeInfo;

/*
 * Struct used for describing components.
 */
typedef struct _Component {
    const char* name;       // component name
    uint8_t type;           // component type represented by compact descriptor enum.
    uint8_t attributeIndex; // index of attribute info. assign NA if there is no attribute.
    uint8_t next;           // index for next component descriptor in the module.
    uint8_t sub;            // index for descriptor of sub component.
} Component;

#define NA 0xff

/*
 * The device descriptor
 */
typedef struct _DeviceDescriptor {
    Component* components; // component description table
    AttributeInfo* attrs;  // attribute description table
    uint8_t* data;         // pointer to the attribute data block
    uint8_t index;         // index for the component to describe
} DeviceDescriptor;

bool write_modules(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);
bool write_component(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

/* [] END OF FILE */
