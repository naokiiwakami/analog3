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

// TODO: load them from flash on startup
uint16_t attack = 72;
uint16_t decay = 128;
uint16_t sustain = 720;
uint16_t release = 340;
uint8_t indexCurve = 1;
uint8_t wireIdGate = 0;
uint8_t wireIdOutput = 0;

uint8_t midiChannel = 1;

// Selector names table
enum SelectorNameIndex {
    Linear,
    Exponential,
};

SelectorAttributes curveAttr = {
    &indexCurve,
    NULL
    // curve_selector_names,
};

SelectorAttributes midiChannelAttr = {
    &indexCurve,
    NULL
    // midiChannel_choices,
};

enum {
    IndexEG,
    IndexAttack,
    IndexDecay,
    IndexSustain,
    IndexRelease,
    IndexCurve,
    IndexGate,
    IndexOutput,
    //
    IndexMidi,
    IndexMidiChannel,
    IndexMidiOutput,
    //
    IndexNull
};

Component components[] = {
    { "EG", Module, NULL },
    { "attackTime", Knob, &attack },
    { "decayTime", Knob, &decay },
    { "sustainLevel", Knob, &sustain },
    { "releaseTime", Knob, &release },
    { "curve", Selector, &curveAttr },
    { "gate", NoteInputPort, &wireIdGate },
    { "output", ValueOutputPort, &wireIdOutput },
    //
    { "midi/cv", Module, NULL },
    { "channel", Selector, &midiChannelAttr },
    { "output", NoteOutputPort, &wireIdOutput },
    { NULL }
};

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
        case COMMAND_DESCRIBE: {
            ComponentNode nodes[] = {
                { IndexMidi, IndexAttack },  // IndexEG
                { IndexDecay, IndexNull },   // IndexAttack
                { IndexSustain, IndexNull }, // IndexDecay
                { IndexRelease, IndexNull }, // IndexSustain
                { IndexCurve, IndexNull },   // IndexRelease
                { IndexGate, IndexNull },    // IndexCurve
                { IndexOutput, IndexNull },  // IndexGate
                { IndexNull, IndexNull },    // IndexOutput
                { IndexNull, IndexMidiChannel }, // IndexMidi
                { IndexMidiOutput, IndexNull },  // IndexMidiChannel
                { IndexNull, IndexNull }         // IndexMidiOutput
            };
            
            const char* curve_selector_names[] = {
                "linear",
                "exponential",
                NULL
            };
            curveAttr.choices = curve_selector_names;
            
            const char* midiChannel_choices[] = {
                "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16",
                NULL
            };
            midiChannelAttr.choices = midiChannel_choices;

            ComponentDef def = { components, nodes, IndexEG };
            
            compact_descriptor_Description deviceDesc = {};
            deviceDesc.component.funcs.encode = &write_component;
            deviceDesc.component.arg = &def;
            // fprintf(stderr, "arg=%p\n", nano_component.name.arg);

            pb_ostream_t stream = { &ostream_callback, NULL, 65536, 0 };
            pb_encode(&stream, compact_descriptor_Description_fields, &deviceDesc);
            flush();

            break;
        }
        case COMMAND_MODIFY: {
            // usage: 'm' <uint8_t:moduleId> <uint8_t:componentId> <uint16_t:value>
            uint8_t componentId = i2cSlaveWriteBuf[idata++];
            uint16_t value = i2cSlaveWriteBuf[idata++];
            value <<= 8;
            value += i2cSlaveWriteBuf[idata++];
            Component* subComponent = &components[componentId - 1];
            switch (subComponent->type) {
            case Knob:
                * (uint16_t*) subComponent->attributes = value;
                break;
            case Selector:
                *((SelectorAttributes*) subComponent->attributes)->index = value;
                break;
            }
            
            PWM_1_WriteCompare1(attack);
            PWM_1_WriteCompare2(decay);
        
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
    PWM_1_WriteCompare1(attack);
    PWM_1_WriteCompare2(decay);
   
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

