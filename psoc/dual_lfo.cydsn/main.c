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

#include "module_encode.h"

////////////////////
// I2C
////////////////////
#define ETX 0x03 // end of text
#define FF 0x0c // form feed
#define BUFFER_SIZE_READ      (0x10u)
#define BUFFER_SIZE_WRITE     (0x10u)
uint8 i2cSlaveReadBufNotReady[1] = { 0xff };
uint8 i2cSlaveWriteBuf[BUFFER_SIZE_WRITE];
uint8 i2cSlaveReadBuf[BUFFER_SIZE_READ];
volatile uint8_t rb_ptr;
volatile uint8_t rb_in_use;

#define COMMAND_PING             'p'
#define COMMAND_NAME             'n'
#define COMMAND_DESCRIBE         'd'
#define COMMAND_MODIFY_ATTRIBUTE 'm'
#define COMMAND_ADD_PORT         'a'
#define COMMAND_REMOVE_PORT      'r'
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

typedef struct _FreePort {
    uint8_t wireId;
    uint16_t value;
    uint8_t listener;
    char name[16];
} FreePort;

typedef struct _Variables {
    uint16_t scale;

    // LFO1
    uint16_t lfo1_frequency;
    uint16_t lfo1_delay;
    uint8_t lfo1_waveFormIndex;
    const char** lfo1_waveForms;
    uint8_t lfo1_gateWireId;
    uint8_t lfo1_outputWireId;
    // LFO2
    uint16_t lfo2_frequency;
    uint16_t lfo2_delay;
    uint8_t lfo2_waveFormIndex;
    const char** lfo2_waveForms;
    uint8_t lfo2_gateWireId;
    uint8_t lfo2_outputWireId;
    // midi-cv
    uint8_t midiChannel;
    const char** midiChannels;
    uint8_t midiOutputWireId;

    // freePorts
    FreePort freePort1;
    FreePort freePort2;
} Variables;

// TODO: load them from flash on startup
Variables data = { 
    1023, // scale
    72, 10, 0, NULL, 0, 0, // lfo1
    320, 1, 1, NULL, 0, 138, // lfo2
    0, NULL, 0, // midi-cv
    { 138, 256,  1, {'0'}, }, // freePort1
    { 0, 256, NA, {'0'}, }, // freePort2
};

typedef struct _Component {
    uint8_t componentType;
    void* data;
} Component;

enum {
    ComponentIdLfo1Freq = 1,
    ComponentIdLfo1Delay,
    ComponentIdLfo2Freq,
    ComponentIdLfo2Delay,
    ComponentIdFreePort1,
    ComponentIdFreePort2,
};
#define ComponentsResolverSize ComponentIdFreePort2
Component components[ComponentsResolverSize] = {
    { Knob, NULL },
    { Knob, NULL },
    { Knob, NULL },
    { Knob, NULL },
    { ValueInputPort, &data.freePort1 },
    { ValueInputPort, &data.freePort2 },
};

/*
 * This function makes a port name 'w<n>' from given wireId n.
 */
void makePortName(char* name, uint8_t wireId)
{
    name[0] = 'w';
    
    // Strategy:
    //  - Convert a decimal to a numeric string in reverse order.
    //  - Reverse the numeric string.
    int nch = 0;
    while (wireId > 0) {
        name[1 + nch++] = wireId % 10 + '0';
        wireId /= 10;
    }
    name[1 + nch] = '\0';
    
    int ich;
    char temp;
    for (ich = 0; ich < nch / 2; ++ich) {
        temp = name[nch - ich];
        name[nch - ich] = name[1 + ich];
        name[1 + ich] = temp;
    }
}

void describe()
{
    AttributeInfo attrs[] = {
        /*  0 */ { offsetof(Variables, scale), AttributeTypeScale, 0 },

        /*  1 */ { offsetof(Variables, lfo1_frequency), AttributeTypeValue, -1 },
        /*  2 */ { offsetof(Variables, lfo1_delay), AttributeTypeValue, -2 },
        /*  3 */ { offsetof(Variables, lfo1_waveFormIndex), AttributeTypeSelectorIndex, 1 },
        /*  4 */ { offsetof(Variables, lfo1_waveForms), AttributeTypeChoices, 0 },
        /*  5 */ { offsetof(Variables, lfo1_gateWireId), AttributeTypeWireId, 0 },
        /*  6 */ { offsetof(Variables, lfo1_outputWireId), AttributeTypeWireId, 0 },

        /*  7 */ { offsetof(Variables, lfo2_frequency), AttributeTypeValue, -7 },
        /*  8 */ { offsetof(Variables, lfo2_delay), AttributeTypeValue, -8 },
        /*  9 */ { offsetof(Variables, lfo2_waveFormIndex), AttributeTypeSelectorIndex, 1 },
        /* 10 */ { offsetof(Variables, lfo2_waveForms), AttributeTypeChoices, 0 },
        /* 11 */ { offsetof(Variables, lfo2_gateWireId), AttributeTypeWireId, 0 },
        /* 12 */ { offsetof(Variables, lfo2_outputWireId), AttributeTypeWireId, 0 },

        /* 13 */ { offsetof(Variables, midiChannel), AttributeTypeSelectorIndex, 1 },
        /* 14 */ { offsetof(Variables, midiChannels), AttributeTypeChoices, 0 },
        /* 15 */ { offsetof(Variables, midiOutputWireId), AttributeTypeWireId, 0 },
        
        /* 16 */ { offsetof(Variables, freePort1) + offsetof(FreePort, wireId), AttributeTypeWireId, 1 },
        /* 17 */ { offsetof(Variables, freePort1) + offsetof(FreePort, value), AttributeTypeValue, -17},
        
        /* 18 */ { offsetof(Variables, freePort2) + offsetof(FreePort, wireId), AttributeTypeWireId, 1 },
        /* 19 */ { offsetof(Variables, freePort2) + offsetof(FreePort, value), AttributeTypeValue, -19},
    };

    enum {
        IndexComponentLfo1,
        IndexComponentLfo1Freq,
        IndexComponentLfo1Delay,
        IndexComponentLfo1WaveForm,
        IndexComponentLfo1Gate,
        IndexComponentLfo1Output,

        IndexComponentLfo2,
        IndexComponentLfo2Freq,
        IndexComponentLfo2Delay,
        IndexComponentLfo2WaveForm,
        IndexComponentLfo2Gate,
        IndexComponentLfo2Output,

        IndexComponentMidi,
        IndexComponentMidiChannel,
        IndexComponentMidiOutput,
        
        IndexComponentFreePort1,
        IndexComponentFreePort2,

        IndexComponentNull,
    };

    ComponentInfo components[] = {
        { "LFO1",         Module,                 NA, NA, IndexComponentLfo2,         IndexComponentLfo1Freq },
        { "frequency",    Knob,  ComponentIdLfo1Freq,  1, IndexComponentLfo1Delay,    IndexComponentNull },
        { "delay",        Knob, ComponentIdLfo1Delay,  2, IndexComponentLfo1WaveForm, IndexComponentNull },
        { "waveForm",     Selector,               NA,  3, IndexComponentLfo1Gate,     IndexComponentNull },
        { "gate",         NoteInputPort,          NA,  5, IndexComponentLfo1Output,   IndexComponentNull },
        { "output",       ValueOutputPort,        NA,  6, IndexComponentNull,         IndexComponentNull },

        { "LFO2",         Module,                 NA, NA, IndexComponentMidi,         IndexComponentLfo2Freq },
        { "frequency",    Knob,  ComponentIdLfo2Freq,  7, IndexComponentLfo2Delay,    IndexComponentNull },
        { "delay",        Knob, ComponentIdLfo2Delay,  8, IndexComponentLfo2WaveForm, IndexComponentNull },
        { "waveForm",     Selector,               NA,  9, IndexComponentLfo2Gate,     IndexComponentNull },
        { "gate",         NoteInputPort,          NA, 11, IndexComponentLfo2Output,   IndexComponentNull },
        { "output",       ValueOutputPort,        NA, 12, IndexComponentNull,         IndexComponentNull },

        { "midi-cv",      Module,          NA, NA, IndexComponentNull,         IndexComponentMidiChannel },
        { "channel",      Selector,        NA, 13, IndexComponentMidiOutput,   IndexComponentNull },
        { "output",       NoteOutputPort,  NA, 15, IndexComponentNull,         IndexComponentNull },

        { NULL,           ValueInputPort, ComponentIdFreePort1, 16, IndexComponentNull,         IndexComponentNull },
        { NULL,           ValueInputPort, ComponentIdFreePort2, 18, IndexComponentNull,         IndexComponentNull },

        { NULL }
    };

    const char* waveForm_choices[] = {
        "triangle",
        "rectangle",
        "sine",
        "random",
        NULL
    };
    data.lfo1_waveForms = waveForm_choices;
    data.lfo2_waveForms = waveForm_choices;
    
    const char* midiChannel_choices[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
        NULL
    };
    data.midiChannels = midiChannel_choices;
    
    if (data.freePort1.listener != NA) {
        if (data.freePort1.name[0] == '\0') {
            makePortName(data.freePort1.name, data.freePort1.wireId);
        }
        components[IndexComponentFreePort1].name = data.freePort1.name;
    }
    
    if (data.freePort2.listener != NA) {
        if (data.freePort2.name[0] == '\0') {
            makePortName(data.freePort2.name, data.freePort2.wireId);
        }
        components[IndexComponentFreePort2].name = data.freePort2.name;
    }
    
    int i;
    for (i = 0; i < IndexComponentNull; ++i) {
        uint8_t id = components[i].id;
        if (id == NA) {
            continue;
        }
        if (id == data.freePort1.listener) {
            components[i].sub = IndexComponentFreePort1;
        }
        if (id == data.freePort2.listener) {
            components[i].sub = IndexComponentFreePort2;
        }
    }

    DeviceDescriptor desc = { components, attrs, (uint8_t*) &data, IndexComponentLfo1 };
    
    compact_descriptor_Description deviceDesc = {};
    deviceDesc.component.funcs.encode = &write_component;
    deviceDesc.component.arg = &desc;
    // fprintf(stderr, "arg=%p\n", nano_component.name.arg);

    pb_ostream_t stream = { &ostream_callback, NULL, 65536, 0 };
    pb_encode(&stream, compact_descriptor_Description_fields, &deviceDesc);
    flush();

}

int16_t getPeriod(int16_t freq)
{
    float k = (1023 - data.lfo1_frequency) / 1023.0;
    k = k * 0.85 + 0.15;
    k = k * k;
    k = k * k;
    uint16_t period = (uint16_t) (65535 * k);
    if (period < 32) {
        period = 32;
    }
    return period;
}

void handleI2CInput()
{
    uint32_t bytesReceived = I2C_S_I2CSlaveGetWriteBufSize();
    if (bytesReceived > 0) {
        uint32_t idata = 0;
        uint8_t command = i2cSlaveWriteBuf[idata++];
        switch(command) {
        case COMMAND_PING:
            Pin_LED_Blue_Write(0);
            ledTimer = 0x1ffff;
            break;
        case COMMAND_NAME: {
            // I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufDeviceName, sizeof(i2cSlaveReadBufDeviceName));
            SizedText text = {"\"Long time ago, in a galaxy far, far away...\"", 45};
            send((uint8_t*)text.str, text.size);
            flush();
            break;
        }
        case COMMAND_DESCRIBE:
            describe(); 
            break;
        case COMMAND_MODIFY_ATTRIBUTE: {
            // usage: 'm' <uint8_t:moduleId> <uint8_t:componentId> <uint16_t:value>
            uint8_t attributeType = i2cSlaveWriteBuf[idata++];
            uint8_t attributeId = i2cSlaveWriteBuf[idata++];
            uint16_t value = i2cSlaveWriteBuf[idata++];
            value <<= 8;
            value += i2cSlaveWriteBuf[idata++];
            
            uint8_t* dataBlock = (uint8_t*) &data;
            
            switch (attributeType) {
            case AttributeTypeValue:
                * (int16_t*) (dataBlock + attributeId) = value;
                break;
            case AttributeTypeSelectorIndex:
            case AttributeTypeWireId:
                dataBlock[attributeId] = value;
                break;
            }

            Timer_1_WritePeriod(getPeriod(data.lfo1_frequency));
            // PWM_1_WriteCompare1(data.lfo1_frequency);
            // PWM_1_WriteCompare2(data.lfo2_frequency);
        
            break;
        }
        case COMMAND_ADD_PORT: {
            uint8_t parentComponentId = i2cSlaveWriteBuf[idata++];
            if (parentComponentId < ComponentsResolverSize && components[parentComponentId].componentType == Knob) {
                FreePort* port = NULL;
                uint8_t freePortOffset;
                uint8_t componentId;
                if (data.freePort1.listener == NA) {
                    port = &data.freePort1;
                    componentId = ComponentIdFreePort1;
                    freePortOffset = offsetof(Variables, freePort1);
                }
                else if (data.freePort2.listener == NA) {
                    port = &data.freePort2;
                    componentId = ComponentIdFreePort2;
                    freePortOffset = offsetof(Variables, freePort2);
                }
                if (port != NULL) {
                    uint8_t nameLen = i2cSlaveWriteBuf[idata++];
                    int i;
                    for (i = 0; i < nameLen; ++i) {
                        port->name[i] = i2cSlaveWriteBuf[idata++];
                    }
                    port->name[i] = '\0';
                    uint8_t wireId = i2cSlaveWriteBuf[idata++];
                    uint16_t value = i2cSlaveWriteBuf[idata++];
                    value <<= 8;
                    value += i2cSlaveWriteBuf[idata++];

                    port->wireId = wireId;
                    port->value = value;
                    port->listener = parentComponentId;
                    
                    AttributeInfo attrs[] = {
                        /* 0 */ { freePortOffset + offsetof(FreePort, wireId), AttributeTypeWireId, 1},
                        /* 1 */ { freePortOffset + offsetof(FreePort, value), AttributeTypeValue, 1 },
                        /* 2 */ { offsetof(Variables, scale), AttributeTypeScale, 0 },
                    };
                    
                    ComponentInfo components[] = {
                        { port->name, ValueInputPort, componentId, 0, 1, 1 },
                        { NULL }
                    };
                    
                    DeviceDescriptor desc = { components, attrs, (uint8_t*) &data, 0 };
    
                    compact_descriptor_Description deviceDesc = {};
                    deviceDesc.component.funcs.encode = &write_component;
                    deviceDesc.component.arg = &desc;
                    // fprintf(stderr, "arg=%p\n", nano_component.name.arg);

                    pb_ostream_t stream = { &ostream_callback, NULL, 65536, 0 };
                    pb_encode(&stream, compact_descriptor_Description_fields, &deviceDesc);
                    flush();
                }
                // TODO: else return error.
                
                // TODO: add the port from the knob.
            }
            // TODO: else return error
            break;
        }
        case COMMAND_REMOVE_PORT: {
            uint8_t componentId = i2cSlaveWriteBuf[idata++];
            if (componentId < ComponentsResolverSize && components[componentId].componentType == ValueInputPort) {
                FreePort* port = (FreePort*) components[componentId].data;
                port->wireId = 0;
                port->listener = NA;
                // TODO: remove the port from the knob.
            }
            // TODO: else return error
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

CY_ISR(ISR_Timer_1_Overflow)
{
    static struct _osc {
        uint8_t direction;
        uint16_t value;
    }
    osc = { 1, 0 };
    
    // Pin_LED_Blue_Write(!Pin_LED_Blue_Read());
    PWM_1_WriteCompare1(osc.value);
    if (osc.direction) {
        if (osc.value == 1023) {
            osc.direction = 0;
            --osc.value;
        }
        else {
            ++osc.value;
        }
    }
    else {
        if (osc.value == 0) {
            ++osc.value;
            osc.direction = 1;
        }
        else {
            --osc.value;
        }
    }
}

int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    Timer_1_Start();
    Timer_1_WritePeriod(getPeriod(data.lfo1_frequency));
    
    PWM_1_Start();
    PWM_1_WriteCompare1(data.lfo1_frequency);
    PWM_1_WriteCompare2(data.lfo2_frequency);
   
    rb_ptr = 1;
    rb_in_use = 0;
    I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufNotReady, 1);
    I2C_S_I2CSlaveInitWriteBuf(i2cSlaveWriteBuf, BUFFER_SIZE_WRITE);
    I2C_S_Start();
    
    Pin_LED_Blue_Write(1);
    
    ISR_Timer_1_Overflow_ClearPending();
    ISR_Timer_1_Overflow_StartEx(ISR_Timer_1_Overflow);
    
    CyGlobalIntEnable; /* Uncomment this line to enable global interrupts. */    
    
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
                Pin_LED_Blue_Write(1);
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

