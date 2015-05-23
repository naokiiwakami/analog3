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

#define Knob compact_descriptor_Component_Type_Knob
#define Selector compact_descriptor_Component_Type_Selector
#define ValueInputPort compact_descriptor_Component_Type_ValueInputPort
#define ValueOutputPort compact_descriptor_Component_Type_ValueOutputPort
#define GateInputPort compact_descriptor_Component_Type_GateInputPort
#define GateOutputPort compact_descriptor_Component_Type_GateOutputPort

typedef struct _SelectorAttributes {
    uint8_t* index;
    const char** choices;
} SelectorAttributes;

typedef struct Component {
    const char* name;
    uint8_t type;
    void* attributes;
} Component;

typedef struct _ModuleInfo {
    const char* name;
    Component* subComponents;
} ModuleInfo;



bool write_modules(pb_ostream_t *stream, const pb_field_t *field, void * const *arg);

/* [] END OF FILE */
