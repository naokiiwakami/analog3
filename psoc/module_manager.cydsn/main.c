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
#define BUFFER_SIZE_WRITE     (0x08u)
uint8 i2cSlaveReadBufNotReady[1] = { 0xff };
uint8 i2cSlaveWriteBuf[BUFFER_SIZE_WRITE];
uint8 i2cSlaveReadBuf[BUFFER_SIZE_READ];
volatile uint8_t rb_ptr;
volatile uint8_t rb_in_use;

#define COMMAND_PING     'p'
#define COMMAND_NAME     'n'
#define COMMAND_DESCRIBE 'd'
#define COMMAND_MODIFY   'm'
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

typedef struct _Variables {
    uint16_t attack;
    uint16_t decay;
    uint16_t sustain;
    uint16_t release;
    uint8_t curveIndex;
    const char** curveChoices;
    uint8_t egGateWireId;
    uint8_t egOutputWireId;

    uint8_t midiChannel;
    const char** midiChannels;
    uint8_t midiOutputWireId;
    
    uint16_t scale;
} Variables;

// TODO: load them from flash on startup
Variables data = { 72, 128, 720, 340, 1, NULL, 0, 0, 0, NULL, 0, 1023 };

void describe()
{
    AttributeInfo attrs[] = {
        /*  0 */ { offsetof(Variables, attack), AttributeTypeValue, 11 },
        /*  1 */ { offsetof(Variables, decay), AttributeTypeValue, 10 },
        /*  2 */ { offsetof(Variables, sustain), AttributeTypeValue, 9 },
        /*  3 */ { offsetof(Variables, release), AttributeTypeValue, 8 },
        /*  4 */ { offsetof(Variables, curveIndex), AttributeTypeSelectorIndex, 1 },
        /*  5 */ { offsetof(Variables, curveChoices), AttributeTypeChoices, 0 },
        /*  6 */ { offsetof(Variables, egGateWireId), AttributeTypeWireId, 0 },
        /*  7 */ { offsetof(Variables, egOutputWireId), AttributeTypeWireId, 0 },
        /*  8 */ { offsetof(Variables, midiChannel), AttributeTypeSelectorIndex, 1 },
        /*  9 */ { offsetof(Variables, midiChannels), AttributeTypeChoices, 0 },
        /* 10 */ { offsetof(Variables, midiOutputWireId), AttributeTypeWireId, 0 },
        /* 11 */ { offsetof(Variables, scale), AttributeTypeScale, 0 }
    };

    enum {
        IndexComponentEG,
        IndexComponentAttack,
        IndexComponentDecay,
        IndexComponentSustain,
        IndexComponentRelease,
        IndexComponentCurve,
        IndexComponentGate,
        IndexComponentOutput,

        IndexComponentMidi,
        IndexComponentMidiChannel,
        IndexComponentMidiOutput,

        IndexComponentNull
    };

    Component components[] = {
        { "EG",           Module,         NA, IndexComponentMidi,    IndexComponentAttack },
        { "attackTime",   Knob,            0, IndexComponentDecay,   IndexComponentNull },
        { "decayTime",    Knob,            1, IndexComponentSustain, IndexComponentNull },
        { "sustainLevel", Knob,            2, IndexComponentRelease, IndexComponentNull },
        { "releaseTime",  Knob,            3, IndexComponentCurve,   IndexComponentNull },
        { "curve",        Selector,        4, IndexComponentGate,    IndexComponentNull },
        { "gate",         NoteInputPort,   6, IndexComponentOutput,  IndexComponentNull },
        { "output",       ValueOutputPort, 7, IndexComponentNull,    IndexComponentNull },

        { "midi-cv",      Module,         NA, IndexComponentNull,    IndexComponentMidiChannel },
        { "channel",      Selector,        8, IndexComponentMidiOutput, IndexComponentNull },
        { "output",       NoteOutputPort, 10, IndexComponentNull,    IndexComponentNull },

        { NULL }
    };

    const char* curve_selector_names[] = {
        "linear",
        "exponential",
        NULL
    };
    data.curveChoices = curve_selector_names;
    
    const char* midiChannel_choices[] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
        NULL
    };
    data.midiChannels = midiChannel_choices;

    DeviceDescriptor desc = { components, attrs, (uint8_t*) &data, IndexComponentEG };
    
    compact_descriptor_Description deviceDesc = {};
    deviceDesc.component.funcs.encode = &write_component;
    deviceDesc.component.arg = &desc;
    // fprintf(stderr, "arg=%p\n", nano_component.name.arg);

    pb_ostream_t stream = { &ostream_callback, NULL, 65536, 0 };
    pb_encode(&stream, compact_descriptor_Description_fields, &deviceDesc);
    flush();

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
        case COMMAND_MODIFY: {
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
                        
            PWM_1_WriteCompare1(data.attack);
            PWM_1_WriteCompare2(data.decay);
        
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
    
    PWM_1_Start();
    PWM_1_WriteCompare1(data.attack);
    PWM_1_WriteCompare2(data.decay);
   
    rb_ptr = 1;
    rb_in_use = 0;
    I2C_S_I2CSlaveInitReadBuf(i2cSlaveReadBufNotReady, 1);
    I2C_S_I2CSlaveInitWriteBuf(i2cSlaveWriteBuf, BUFFER_SIZE_WRITE);
    I2C_S_Start();
    
    Pin_LED_Blue_Write(1);
    
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

