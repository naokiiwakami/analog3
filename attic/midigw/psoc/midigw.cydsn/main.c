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
#include "mcp2515.h"

int32_t my_device_id = 100;

void put_hex(uint8_t data)
{
    uint8_t elem = data >> 4;
    int i;
    SERIAL_UartPutChar(' ');
    for (i = 0; i < 2; ++i) {
        elem = (elem < 10) ? elem + '0' : elem + 'a' - 10;
        SERIAL_UartPutChar(elem);
        elem = data & 0x0f;
    }
}

void ISR_MIDI()
{
    
}

enum MIDI_STATUS {
    MIDI_NONE = 0,
    // channel voice message
    MIDI_NOTE_OFF = 0x80,
    MIDI_NOTE_ON = 0x90,
    MIDI_POLY_KEY_PRESSURE = 0xa0,
    MIDI_CONTROL_CHANGE = 0xb0,
    MIDI_PROGRAM_CHANGE = 0xc0,
    MIDI_CHANNEL_PRESSURE = 0xd0,
    MIDI_PITCH_BEND = 0xe0,
    // system control message
    MIDI_SYSEX = 0xf0,
    MIDI_TIME_CODE_QUARTER_FRAME = 0xf1,
    MIDI_SONG_POSITION_POINTER = 0xf2,
    MIDI_SONG_SELECT = 0xf3,
    MIDI_TUNE_REQUEST = 0xf6,
    MIDI_SYSEX_END = 0xf7,
    // system real-time messages
    MIDI_TIMING_CLOCK = 0xf8,
    MIDI_START = 0xfa,
    MIDI_CONTINUE = 0xfb,
    MIDI_STOP = 0xfc,
    MIDI_ACTIVE_SENSING = 0xfe,
    MIDI_RESET = 0xff,
};

// configuration
uint8_t midi_target_channel = 0; // channel 1
uint8_t num_voices = 1;
uint8_t retrigger = 0;
enum {
    ASSIGN_LOWEST,
    ASSIGN_HIGHEST,
    ASSIGN_OLDEST
};
uint8_t assignment_policy = ASSIGN_OLDEST;

// voices
typedef struct voice_t {
    uint8_t key;
    uint8_t level;
    uint16_t voice_id;
    struct voice_t *next;
    struct voice_t *prev;
} voice_t;

typedef struct voice_list_t {
    voice_t first;
    voice_t last;
}
voice_list_t;

void voice_list_init(voice_list_t *list)
{
    list->first.prev = NULL;
    list->first.next = &list->last;
    list->last.prev = &list->first;
    list->last.next = NULL;
    list->first.key = 0xff;
    list->last.key = 0xff;
}

inline void voice_list_push_back(voice_list_t *list, voice_t *voice)
{
    voice->prev = list->last.prev;
    voice->next = &list->last;
    voice->prev->next = voice;
    voice->next->prev = voice;
}

inline void voice_list_push_front(voice_list_t *list, voice_t *voice)
{
    voice->prev = &list->first;
    voice->next = list->first.next;
    voice->prev->next = voice;
    voice->next->prev = voice;
}

inline voice_t *voice_list_pop_back(voice_list_t *list)
{
    voice_t *voice = list->last.prev;
    if (voice == &list->first)
        return NULL;

    voice->prev->next = voice->next;
    voice->next->prev = voice->prev;
    
    return voice;
}

inline voice_t *voice_list_pop_front(voice_list_t *list)
{
    voice_t *voice = list->first.next;
    if (voice == &list->last)
        return NULL;

    voice->prev->next = voice->next;
    voice->next->prev = voice->prev;
    
    return voice;
}

inline void voice_erase(voice_t *voice)
{
    voice->prev->next = voice->next;
    voice->next->prev = voice->prev;
}

inline void voice_insert(voice_t *pos, voice_t *to_insert)
{
    to_insert->next = pos;
    to_insert->prev = pos->prev;
    to_insert->next->prev = to_insert;
    to_insert->prev->next = to_insert;
}

#define MAX_VOICES 24
voice_t voices[MAX_VOICES];
voice_list_t voices_active;
voice_list_t voices_standby;

void init_voices()
{
    uint8_t voice_array_length = (num_voices == 1 && assignment_policy != ASSIGN_OLDEST) ? MAX_VOICES : num_voices;
    uint8_t i;

    voice_list_init(&voices_active);
    voice_list_init(&voices_standby);

    for (i = 0; i < voice_array_length; ++i) {
        voices[i].key = 0xff;
        voices[i].level = 0;
        if (assignment_policy == ASSIGN_OLDEST)
            voices[i].voice_id = 0x100 + i;
        else
            voices[i].voice_id = 0x100;
        voice_list_push_back(&voices_standby, &voices[i]);
    }
}

inline void send_note_on(voice_t *voice)
{
    uint8_t buf[8];
    buf[0] = 3;
    buf[1] = 0x09; // note on
    buf[2] = voice->key;
    buf[3] = voice->level;
    mcp2515_write_array(TXB0DLC, buf, 4);
    set_can_id_std(TXB0SIDH, voice->voice_id);
    mcp2515_message_request_to_send_txb0();

    SERIAL_UartPutString("note on  [");
    put_hex(0xff & (voice->voice_id >> 8));
    put_hex(0xff & voice->voice_id);
    SERIAL_UartPutString("]");
    put_hex(voice->key);
    SERIAL_UartPutString("\r\n");
}

inline void send_note_off(voice_t *voice)
{
    uint8_t buf[8];
    buf[0] = 3;
    buf[1] = 0x08; // note off
    buf[2] = voice->key;
    buf[3] = voice->level;
    mcp2515_write_array(TXB0DLC, buf, 4);
    set_can_id_std(TXB0SIDH, voice->voice_id);
    mcp2515_message_request_to_send_txb0();

    SERIAL_UartPutString("note off [");
    put_hex(0xff & (voice->voice_id >> 8));
    put_hex(0xff & voice->voice_id);
    SERIAL_UartPutString("]");
    put_hex(voice->key);
    SERIAL_UartPutString("\r\n");
}

/**
 * midi_data[0] : key
 * midi_data[1] : velocity
 */
void note_on(uint8_t midi_data[])
{
    uint8_t key = midi_data[0];
    uint8_t velocity = midi_data[1];
    
    voice_t *voice;
    for (voice = voices_active.first.next; voice != &voices_active.last; voice = voice->next) {
        if (voice->key == key) {
            voice->level = velocity;
            send_note_on(voice);
            voice_erase(voice);
            voice_list_push_front(&voices_active, voice);
            return;
        }
    }
    voice = voice_list_pop_back(&voices_standby);
    if (voice == NULL) {
        voice = voice_list_pop_back(&voices_active);
        send_note_off(voice);
    }
    voice->key = key;
    voice->level = velocity;
    send_note_on(voice);
    voice_list_push_front(&voices_active, voice);
}

void note_on_mono(uint8_t midi_data[])
{
    uint8_t key = midi_data[0];
    uint8_t velocity = midi_data[1];
    
    voice_t *pos;
    if (assignment_policy == ASSIGN_LOWEST)
       for (pos = voices_active.first.next; pos != &voices_active.last && pos->key < key; pos = pos->next) {}
    else
       for (pos = voices_active.first.next; pos != &voices_active.last && pos->key > key; pos = pos->next) {}

    if (pos->key == key) {
        pos->level = velocity;
        send_note_on(pos);
        return;
    }
    voice_t *voice = voice_list_pop_back(&voices_standby);
    voice->key = key;
    voice->level = velocity;
    voice_insert(pos, voice);
    if (voice->prev == &voices_active.first)
        send_note_on(voice);
}

/**
 * midi_data[0] : key
 * midi_data[1] : velocity
 */
void note_off(uint8_t midi_data[])
{
    uint8_t key = midi_data[0];
    uint8_t velocity = midi_data[1];
    
    voice_t *voice;
    for (voice = voices_active.first.next; voice != &voices_active.last; voice = voice->next) {
        if (voice->key == key) {
            voice->level = velocity;
            send_note_off(voice);
            voice_erase(voice);
            voice_list_push_front(&voices_standby, voice);
            return;
        }
    }
}

void note_off_mono(uint8_t midi_data[])
{
    uint8_t key = midi_data[0];
    uint8_t velocity = midi_data[1];
    
    voice_t *voice;
    for (voice = voices_active.first.next; voice != &voices_active.last; voice = voice->next) {
        if (voice->key == key) {
            voice->level = velocity;
            if (voice->prev == &voices_active.first) {
                if (voice->next == &voices_active.last)
                    send_note_off(voice);
                else
                    send_note_on(voice->next);
            }
            voice_erase(voice);
            voice_list_push_front(&voices_standby, voice);
            return;
        }
    }
}

CY_ISR(ISR_USER_SW)
{
    Pin_LED_B_Write(0);
    mcp2515_init();
    SERIAL_UartPutString("\r\nHello World!\r\n");    
    Pin_LED_B_Write(1);    
}

int main()
{
    uint32 ch;
    uint8 led;
    
    init_voices();
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_MIDI_Start();
    SPIM_CAN_Start();
    SERIAL_Start();
    // UART_MIDI_SetCustomInterruptHandler(ISR_MIDI);
    
    // Pin_CAN_Enable_Write(1);
    // Pin_CAN_Reset_Write(1);
    
    mcp2515_init();

    SERIAL_UartPutString("\r\nHello World!\r\n");
    
    led = 1;
    Pin_LED_B_Write(led);
    
    isr_1_ClearPending();
    isr_1_StartEx(ISR_USER_SW);
    
    uint8_t midi_status = MIDI_NONE;
    uint8_t midi_event;
    uint8_t midi_data[2];
    uint8_t midi_data_ptr = 0;

    for(;;)
    {
        if (UART_MIDI_SpiUartGetRxBufferSize() > 0) {
            ch = UART_MIDI_UartGetChar();
            if (midi_status == MIDI_SYSEX) {
                if (ch == MIDI_SYSEX_END) {
                    midi_status = MIDI_NONE;
                }
                continue;
            }
            midi_event = MIDI_NONE;
            if (ch & 0x80) {
                midi_status = ch;
                midi_data_ptr = 0;
                switch (midi_status) {
                case MIDI_SYSEX:
                    continue;
                case MIDI_TIME_CODE_QUARTER_FRAME:
                case MIDI_SONG_SELECT:
                    midi_data_ptr = 1;
                    break;
                case MIDI_TUNE_REQUEST:
                case MIDI_TIMING_CLOCK:
                case MIDI_START:
                case MIDI_CONTINUE:
                case MIDI_STOP:
                case MIDI_ACTIVE_SENSING:
                case MIDI_RESET:
                    midi_event = midi_status;
                    break;
                case MIDI_SYSEX_END:
                    midi_status = MIDI_NONE;
                    continue;
                default:
                    if (midi_status < MIDI_SYSEX) {
                        ch &= 0xf0;
                        switch (ch) {
                        case MIDI_PROGRAM_CHANGE:
                        case MIDI_CHANNEL_PRESSURE:
                            midi_data_ptr = 1;
                            break;
                        }
                    }
                }
            }
            else {
                midi_data[midi_data_ptr++] = ch;
                if (midi_data_ptr == 2) {
                    midi_event = midi_status;
                    midi_data_ptr = 0;
                }
            }
            
            if (midi_event != MIDI_NONE) {
                if (midi_event < MIDI_SYSEX) {
                    // channel message
                    if ((midi_event & 0x0f) == midi_target_channel) {
                        midi_event &= 0xf0;
                        switch (midi_event) {
                        case MIDI_NOTE_ON:
                            if (midi_data[1] > 0) {
                                note_on(midi_data);
                                break;
                            }
                            // else fall down to note off
                        case MIDI_NOTE_OFF:
                            note_off(midi_data);
                            break;
                        }
                    }
                }
            }
        }
    }
}

/* [] END OF FILE */
