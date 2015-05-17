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
#include "nano_module_descriptor.pb.h"

////////////////////
// I2C
////////////////////
#define ETX 0x03 // end of text
#define FF 0x0c // form feed
#define BUFFER_SIZE_READ      (0x10u)
#define BUFFER_SIZE_WRITE     (0x08u)
uint8 i2cSlaveReadBufNotReady[1] = { 0xff };
uint8 i2cSlaveReadBufDeviceName[] = { 10, 'P', 'S', 'o', 'C', '_', 'r', 'a', 'c', 'k', ETX };
uint8 i2cSlaveWriteBuf[BUFFER_SIZE_WRITE];
uint8 i2cSlaveReadBuf[BUFFER_SIZE_READ];
volatile uint8_t rb_ptr;
volatile uint8_t rb_in_use;

#define COMMAND_PING     'p'
#define COMMAND_NAME     'n'
#define COMMAND_DESCRIBE 'd'
#define COMMAND_SEND 's'
#define COMMAND_SET_WIREID 'w'
#define COMMAND_GET_WIREID 'W'

volatile int ledTimer = 0;

typedef struct _SizedText {
    const char* str;
    int size;
} SizedText;

void send(const uint8* data, int size)
{
    int idata;
    for (idata = 0; idata < size; ++idata) {
        if (rb_in_use) {
            while (I2C_S_I2CSlaveGetReadBufSize() < rb_ptr) {}
            I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufNotReady, 1);
            rb_ptr = 1;
            rb_in_use = 0;
        }
        i2cSlaveReadBuf[rb_ptr++] = data[idata];
        if (rb_ptr == BUFFER_SIZE_READ - 1) {
            i2cSlaveReadBuf[0] = rb_ptr;
            i2cSlaveReadBuf[rb_ptr++] = FF;
            rb_in_use = 1;
            I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBuf, rb_ptr);
        }
    }
}

void flush()
{
    // If the buffer is in use, wait for the transmission completed.
    if (rb_in_use) {
        while (I2C_S_I2CSlaveGetReadBufSize() < rb_ptr) {}
        I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufNotReady, 1);
        rb_ptr = 1;
        rb_in_use = 0;
    }

    i2cSlaveReadBuf[0] = rb_ptr;
    i2cSlaveReadBuf[rb_ptr++] = ETX;
    rb_in_use = 1;
    I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBuf, rb_ptr);
}

bool ostream_callback(pb_ostream_t *stream, const uint8_t *buf, size_t count)
{
    send(buf, count);
    return true;
}

bool write_cstring(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    const char* text = (const char*) *arg;
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }
    return pb_encode_string(stream, (uint8_t*) text, strlen(text));
}

bool write_string_array(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    char** string_list = (char**) *arg;
    
    int ii;
    for (ii = 0; string_list[ii] != NULL; ++ii) {
        connector_Value value = {};
        value.svalue.funcs.encode = &write_cstring;
        value.svalue.arg = string_list[ii];

        if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Value_fields, &value))
        {
            return false;
        }
    }
    
    return true;
}

// Values table
enum ValueIndex {
    IndexAttack,
    IndexDecay,
    IndexSustain,
    IndexRelease,
    IndexCurve,
    IndexGate,
    IndexOutput,
    NumIndexes
};

// TODO: load them from flash on startup
uint16_t eg_values[NumIndexes] = {
    72, 128, 720, 340, 0, 0, 0
};

// Selector names table
enum SelectorNameIndex {
    Linear,
    Exponential,
};

// schema.  better to have it on eeprom
typedef enum _Type {
    Knob,
    Selector,
    InputSignalPort,
    OutputSignalPort,
    InputGatePort,
    OutputGatePort,
} Type;

typedef struct Component {
    const char* name;
    Type type;
    int valueIndex;
} Component;

const char* curve_selector_names[] = {
    "linear",
    "exponential",
    NULL
};

Component eg_components[] = {
    { "attackTime", Knob, IndexAttack },
    { "decayTime", Knob, IndexDecay },
    { "sustainLevel", Knob, IndexSustain },
    { "releaseTime", Knob, IndexRelease },
    { "curve", Selector, IndexCurve },
    { "gate", InputGatePort, IndexGate },
    { "output", OutputSignalPort, IndexOutput },
    { NULL }
};

const char* attr_name_value = "value";
const char* attr_name_scale = "scale";
const char* attr_name_choices = "choices";
const char* attr_name_wireId = "wireId";
const char* attr_name_signal = "signal";
const char* attr_name_direction = "direction";

const char* signal_value = "value";
const char* signal_note = "note";

const char* direction_input = "INPUT";
const char* direction_output = "OUTPUT";

const uint16_t SCALE = 1023;

bool write_knob_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    int valueIndex = *(int*) *arg;

    connector_Attribute attr_value = {};
    attr_value.name.funcs.encode = &write_cstring;
    attr_value.name.arg = (void*) attr_name_value;
    connector_Value value = {};
    value.ivalue = eg_values[valueIndex];
    value.has_ivalue = true;
    attr_value.value = value;
    attr_value.has_value = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_value))
    {
        return false;
    }

    connector_Attribute attr_scale = {};
    attr_scale.name.funcs.encode = &write_cstring;
    attr_scale.name.arg = (void*) attr_name_scale;
    connector_Value scale = {};
    scale.ivalue = SCALE;
    scale.has_ivalue = true;
    attr_scale.value = scale;
    attr_scale.has_value = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_scale))
    {
        return false;
    }

    return true;
}

bool write_selector_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    int valueIndex = *(int*) *arg;
    
    connector_Attribute attr_value = {};
    attr_value.name.funcs.encode = &write_cstring;
    attr_value.name.arg = (void*) attr_name_value;
    connector_Value value = {};
    value.ivalue = eg_values[valueIndex];
    value.has_ivalue = true;
    attr_value.value = value;
    attr_value.has_value = true;
    
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_value))
    {
        return false;
    }
    
    connector_Attribute attr_choices = {};
    attr_choices.name.funcs.encode = &write_cstring;
    attr_choices.name.arg = (void*) attr_name_choices;
    connector_Value choices = {};
    choices.avalue.funcs.encode = &write_string_array;
    choices.avalue.arg = curve_selector_names;
    attr_choices.value = choices;
    attr_choices.has_value = true;

    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_choices))
    {
        return false;
    }

    return true;
}

typedef struct _TypeAndValue {
    Type type;
    int valueIndex;
} TypeAndValue;

bool write_port_attributes(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    TypeAndValue* tnv = (TypeAndValue*) *arg;
    
    // put wire Id
    int ivalue = eg_values[tnv->valueIndex];
    if (ivalue > 0) {
        connector_Attribute attr_wireId = {};
        attr_wireId.name.funcs.encode = &write_cstring;
        attr_wireId.name.arg = (void*) attr_name_wireId;
        connector_Value value = {};
        value.ivalue = ivalue;
        value.has_ivalue = true;
        attr_wireId.value = value;
        attr_wireId.has_value = true;
    
        if (!pb_encode_tag_for_field(stream, field) ||
            !pb_encode_submessage(stream, connector_Attribute_fields, &attr_wireId))
        {
            return false;
        }
    }

    const char* direction;
    if (tnv->type == InputSignalPort || tnv->type == InputGatePort) {
        direction = direction_input;
    }
    else {
        direction = direction_output;
    }
    connector_Attribute attr_direction = {};
    attr_direction.name.funcs.encode = &write_cstring;
    attr_direction.name.arg = (void*) attr_name_direction;
    connector_Value value = {};
    value.svalue.funcs.encode = &write_cstring;
    value.svalue.arg = (void*) direction;
    attr_direction.value = value;
    attr_direction.has_value = true;
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_direction))
    {
        return false;
    }
    
    const char* signal;
    if (tnv->type == InputSignalPort || tnv->type == OutputSignalPort) {
        signal = signal_value;
    }
    else {
        signal = signal_note;
    }
    connector_Attribute attr_signal = {};
    attr_signal.name.funcs.encode = &write_cstring;
    attr_signal.name.arg = (void*) attr_name_signal;
    connector_Value signalValue = {};
    signalValue.svalue.funcs.encode = &write_cstring;
    signalValue.svalue.arg = (void*) signal;
    attr_signal.value = signalValue;
    attr_signal.has_value = true;
    if (!pb_encode_tag_for_field(stream, field) ||
        !pb_encode_submessage(stream, connector_Attribute_fields, &attr_signal))
    {
        return false;
    }
    
    return true;
}

bool write_subcomponents(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    Component* components = (Component*) *arg;
    int ic;
    for (ic = 0; components[ic].name != NULL; ++ic) {
        connector_Component subComponent = {};
        subComponent.name.funcs.encode = &write_cstring;
        subComponent.name.arg = (void*) components[ic].name;
        subComponent.id = components[ic].valueIndex;
        subComponent.has_id = true;
        switch (components[ic].type) {
        case Knob:
            subComponent.type = connector_Component_Type_Knob;
            subComponent.attribute.funcs.encode = &write_knob_attributes;
            subComponent.attribute.arg = &components[ic].valueIndex;
            break;
        case Selector:
            subComponent.type = connector_Component_Type_Selector;
            subComponent.attribute.funcs.encode = &write_selector_attributes;
            subComponent.attribute.arg = &components[ic].valueIndex;
            break;
        case InputSignalPort:
        case OutputSignalPort:
        case InputGatePort:
        case OutputGatePort:
            subComponent.type = connector_Component_Type_Port;
            TypeAndValue tnv = { components[ic].type, components[ic].valueIndex };
            subComponent.attribute.funcs.encode = &write_port_attributes;
            subComponent.attribute.arg = &tnv;
            break;
        };
        subComponent.has_type = true;

        if (!pb_encode_tag_for_field(stream, field))
            return false;
    
        if (!pb_encode_submessage(stream, connector_Component_fields, &subComponent)) {
            return false;
        }
    }
    
    return true;
}

bool write_components(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    connector_Component component = {};
    component.name.funcs.encode = &write_cstring;
    component.name.arg = "eg_nano";
    component.type = connector_Component_Type_Module;
    component.has_type = true;
    component.id = 1;
    component.has_id = true;
    component.sub_component.funcs.encode = &write_subcomponents;
    component.sub_component.arg = eg_components;

    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    if (!pb_encode_submessage(stream, connector_Component_fields, &component)) {
        return false;
    }
    
    return true;
}

void handleI2CInput()
{
    uint32_t bytesReceived = I2C_S_I2CSlaveGetWriteBufSize();
    if (bytesReceived > 0) {
        uint32_t idata = 0;
        uint8_t command = i2cSlaveWriteBuf[idata++];
        switch(command) {
        case COMMAND_PING:
            Pin_LED_BLUE_Write(0);
            ledTimer = 0x1ffff;
            break;
        case COMMAND_NAME: {
            // I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufDeviceName, sizeof(i2cSlaveReadBufDeviceName));
            SizedText text = {"\"Long time ago, in a galaxy far, far away...\"", 45};
            send((uint8_t*)text.str, text.size);
            flush();
            break;
        }
        case COMMAND_DESCRIBE: {
            connector_Description description = {};
            description.component.funcs.encode = &write_components;
            description.component.arg = NULL;
            // fprintf(stderr, "arg=%p\n", nano_component.name.arg);

            pb_ostream_t stream = { &ostream_callback, NULL, 65536, 0 };
            pb_encode(&stream, connector_Description_fields, &description);
            flush();

            break;
        }
            /*
        case COMMAND_SEND:
            tx_numBytes = 0;
            while (idata < bytesReceived) {
                tx_dataArray[tx_numBytes++] = i2cSlaveWriteBuf[idata++];
            }
            initiateSend();
            break;
        case COMMAND_SET_WIREID:
            wireId = i2cSlaveWriteBuf[idata++];
            break;
        case COMMAND_GET_WIREID:
            i2cSlaveReadBuf[0] = wireId;
            break;
            */
        }
    }
}

int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    CyGlobalIntEnable; /* Uncomment this line to enable global interrupts. */
   
    rb_ptr = 1;
    rb_in_use = 0;
    I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufNotReady, 1);
    I2C_S_I2CSlaveInitWriteBuf(i2cSlaveWriteBuf, BUFFER_SIZE_WRITE);
    I2C_S_Start();
    
    Pin_LED_BLUE_Write(1);
    
    for(;;)
    {
        if (I2C_S_I2CSlaveStatus() & I2C_S_I2C_SSTAT_WR_CMPLT)
        {
            handleI2CInput();
            I2C_S_I2CSlaveClearWriteBuf();
            I2C_S_I2CSlaveClearWriteStatus();
        }
        
        /*
        if (I2C_S_I2CSlaveStatus() & I2C_S_I2C_SSTAT_RD_CMPLT)
        {
            if (I2C_S_I2CSlaveGetReadBufSize() == rx_numBytes) {
                // All available bytes are read by master.
                // rx_numBytes = 0;
                I2C_S_I2CSlaveClearReadBuf();
                I2C_S_I2CSlaveClearReadStatus();
            }
            // Pin_ReadReady_Write(0);
        }
        */
        if (ledTimer > 0) {
            if (--ledTimer == 0) {
                Pin_LED_BLUE_Write(1);
            }
        }
        
/*        
        if (rx_byteReady) {
            rx_byteReady = 0;
            uint8_t data = ShiftReg_In_ReadData();
            rx_dataArray[++rx_numBytes] = data;
            if ((Status_Reg_1_Read() & 0x1) == 0) {
                rx_dataArray[0] = rx_numBytes;
                ++rx_numBytes;
                memcpy(i2cSlaveReadBuf, rx_dataArray, rx_numBytes);
                Pin_ReadReady_Write(1);
            }
        }
        */
    }
}

/* [] END OF FILE */

