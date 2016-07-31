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

void SPIM_CAN_WriteTxData24(uint8 data1, uint8 data2, uint8 data3);
void SPIM_CAN_WriteTxData2plusN(uint8 data1, uint8 data2, uint8 n);

// TODO: move this to separate mcp2515.h file
// MCP2515 SPI instructions
//
#define CAN_CTRL_RESET          0xc0 // 1100 0000
#define CAN_CTRL_READ           0x03 // 0000 0011
#define CAN_CTRL_READ_RX_BUFFER 0x90 // 1001 0nm0
#define CAN_CTRL_WRITE          0x02 // 0000 0010
#define CAN_CTRL_LOAD_TX_BUFFER 0x40 // 0100 0abc
#define CAN_CTRL_RTS_TXB0       0x81 // 1000 0001
#define CAN_CTRL_RTS_TXB1       0x82 // 1000 0010
#define CAN_CTRL_RTS_TXB2       0x84 // 1000 0100
#define CAN_CTRL_READ_STATUS    0xa0 // 1010 0000
#define CAN_CTRL_RX_STATUS      0xb0 // 1011 0000
#define CAN_CTRL_BIT_MODIFY     0x05 // 0000 0101

// MCP2515 register map
//
// higher-order address = 0
#define RXF0SIDH  0x00
#define RXF0SIDL  0x01
#define RXF0EID8  0x02
#define RXF0EID0  0x03
#define RXF1SIDH  0x04
#define RXF1SIDL  0x05
#define RXF1EID8  0x06
#define RXF1EID0  0x07
#define RXF2SIDH  0x08
#define RXF2SIDL  0x09
#define RXF2EID8  0x0a
#define RXF2EID0  0x0b
#define BFPCTRL   0x0c
#define TXRTSCTRL 0x0d
#define CANSTAT   0x0e
#define CANCTRL   0x0f

// higher-order address = 1
#define RXF3SIDH 0x10
#define RXF3SIDL 0x11
#define RXF3EID8 0x12
#define RXF3EID0 0x13
#define RXF4SIDH 0x14
#define RXF4SIDL 0x15
#define RXF4EID8 0x16
#define RXF4EID0 0x17
#define RXF5SIDH 0x18
#define RXF5SIDL 0x19
#define RXF5EID8 0x1a
#define RXF5EID0 0x1b
#define TEC      0x1c
#define REC      0x1d
#define CANSTAT1 0x1e
#define CANCTRL1 0x1f

// higher-order address = 2
#define RXM0SIDH 0x20
#define RXM0SIDL 0x21
#define RXM0EID8 0x22
#define RXM0EID0 0x23
#define RXM1SIDH 0x24
#define RXM1SIDL 0x25
#define RXM1EID8 0x26
#define RXM1EID0 0x27
#define CNF3     0x28
#define CNF2     0x29
#define CNF1     0x2a
#define CANINTE  0x2b
#define CANINTF  0x2c
#define EFLG     0x2d
#define CANSTAT2 0x2e
#define CANCTRL2 0x2f

// register addresses (TXB0)
#define TXB0CTRL 0x30
#define TXB0SIDH 0x31
#define TXB0SIDL 0x32
#define TXB0EID8 0x33
#define TXB0EID0 0x34
#define TXB0DLC  0x35
#define TXB0D0   0x36
#define TXB0D1   0x37
#define TXB0D2   0x38
#define TXB0D3   0x39
#define TXB0D4   0x3a
#define TXB0D5   0x3b
#define TXB0D6   0x3c
#define TXB0D7   0x3d
#define CANSTAT3 0x3e
#define CANCTRL3 0x3f

// register addresses (TXB1)
#define TXB1CTRL 0x40
#define TXB1SIDH 0x41
#define TXB1SIDL 0x42
#define TXB1EID8 0x43
#define TXB1EID0 0x44
#define TXB1DLC  0x45
#define TXB1D0   0x46
#define TXB1D1   0x47
#define TXB1D2   0x48
#define TXB1D3   0x49
#define TXB1D4   0x4a
#define TXB1D5   0x4b
#define TXB1D6   0x4c
#define TXB1D7   0x4d
#define CANSTAT4 0x4e
#define CANCTRL4 0x4f

// register addresses (TXB2)
#define TXB2CTRL 0x50
#define TXB2SIDH 0x51
#define TXB2SIDL 0x52
#define TXB2EID8 0x53
#define TXB2EID0 0x54
#define TXB2DLC  0x55
#define TXB2D0   0x56
#define TXB2D1   0x57
#define TXB2D2   0x58
#define TXB2D3   0x59
#define TXB2D4   0x5a
#define TXB2D5   0x5b
#define TXB2D6   0x5c
#define TXB2D7   0x5d
#define CANSTAT5 0x5e
#define CANCTRL5 0x5f

// register addresses (RXB0)
#define RXB0CTRL 0x60
#define RXB0SIDH 0x61
#define RXB0SIDL 0x62
#define RXB0EID8 0x63
#define RXB0EID0 0x64
#define RXB0DLC  0x65
#define RXB0D0   0x66
#define RXB0D1   0x67
#define RXB0D2   0x68
#define RXB0D3   0x69
#define RXB0D4   0x6a
#define RXB0D5   0x6b
#define RXB0D6   0x6c
#define RXB0D7   0x6d
#define CANSTAT6 0x6e
#define CANCTRL6 0x6f

// register addresses (RXB1)
#define RXB1CTRL 0x70
#define RXB1SIDH 0x71
#define RXB1SIDL 0x72
#define RXB1EID8 0x73
#define RXB1EID0 0x74
#define RXB1DLC  0x75
#define RXB1D0   0x76
#define RXB1D1   0x77
#define RXB1D2   0x78
#define RXB1D3   0x79
#define RXB1D4   0x7a
#define RXB1D5   0x7b
#define RXB1D6   0x7c
#define RXB1D7   0x7d
#define CANSTAT7 0x7e
#define CANCTRL7 0x7f

void mcp2515_reset();
void mcp2515_read(uint8_t address, uint8_t data[], uint8_t length);
void mcp2515_read_rx_buffer(uint8_t location, uint8_t data[], uint8_t length);
void mcp2515_write(uint8_t address, uint8_t data);
void mcp2515_write_array(uint8_t address, uint8_t data[], uint8_t length);
void mcp2515_load_tx_buffer(uint8_t location, uint8_t data);
void mcp2515_message_request_to_send_txb0();
void mcp2515_message_request_to_send_txb1();
void mcp2515_message_request_to_send_txb2();
void mcp2515_read_status(uint8_t *status);
void mcp2515_rx_status(uint8_t *status);
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);

int32_t my_device_id = 100;

void note_on(uint8_t key, uint8_t velocity);
void note_off(uint8_t key, uint8_t velocity);

static void put_hex(uint8_t data)
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

void set_can_id_std(uint8_t register_address, uint16_t id)
{
    uint8_t data[2];
    //               7     6     5     4     3     2     1     0
    // TXBnSIDH: SID10  SID9  SID8  SID7  SID6  SID5  SID4  SID3
    // TXBnSIDL:  SID2  SID1   -     -   EXIDE   -   EID17 EID16
    id <<= 5;
    data[1] = (uint8_t) id;
    id >>= 8;
    data[0] = (uint8_t) id;
    mcp2515_write_array(register_address, data, 2);
}

void mcp2515_init()
{
    uint8_t temp;

    mcp2515_reset();

    // configure baud rate
    // configured for 500 kHz baud rate with 16 MHz fosc
    //
    // |Sync(1TQ)| PropSeg | PS1 | PS2 |
    // | <-- NBT (normal bit time) --> |
    //
    //   TQ = 2 * (BRP + 1) / fosc
    //   TQ is set to 2 / fosc = 1 / 8M that makes NBT = 16 Tq
    //
    // CNF1
    #define SJW 0b00 // bit7-6 SJW<1:0>: Synchronization Jump Width Length bits
                     // 11 = Length = 4 x TQ
                     // 10 = Length = 3 x TQ
                     // 01 = Length = 2 x TQ
                     // 00 = Length = 1 x TQ
    #define BRP 0    // bit5-0 BRP<5:0>: Baud Rate Prescaler bits
                     // TQ = 2 x (BRP + 1)/FOSC
    temp = (SJW << 6) + BRP;
    // put_hex(temp);
    mcp2515_write(CNF1, temp);

    //  PropSeg + PS1 >= PS2
    //  PropSeg + PS1 >= Tdelay
    //  PS2 > SJW
    //  Typically, the sampling of the bit should take place at
    //  about 60-70% of the bit time.
    //  Also, typically, the TDELAY is 1-2 TQ.
    //  We set here PS1 = 5 and PS2 = 6 that make ProSeg = 4
    // CNF2
    #define BTLMODE 1 // bit7 BTLMODE: PS2 Bit Time Length bit
                      // 0 = Length of PS2 is the greater of PS1 and IPT (2 TQ)
                      // 1 = Length of PS2 determined by PHSEG22:PHSEG20 bits of CNF3
    #define SAM 1     // bit6 SAM: Sample Point Configuration bit
                      // 1 = Bus line is sampled three times at the sample point
                      // 0 = Bus line is sampled once at the sample point
    #define PHSEG1 4  // bit5-3 PHSEG1<2:0>: PS1 Length bits
                      // PHSEG1 + 1) x TQ
    #define PRESEG 3  // bit2-0 PRSEG<2:0>: Propagation Segment Length bit
                      // (PRSEG + 1) x TQ
    temp = (BTLMODE << 7) + (SAM << 6) + (PHSEG1 << 3) + PRESEG;
    // put_hex(temp);
    mcp2515_write(CNF2, temp);

    // CNF3
    #define SOF 0    // bit7 SOF: Start-of-Frame signal bit
                     // If CANCTRL.CLKEN = 1:
                     // 1 = CLKOUT pin enabled for SOF signal
                     // 0 = CLKOUT pin enabled for clockout function
                     // If CANCTRL.CLKEN = 0, Bit is don’t care.
    #define WAKFIL 0 // bit6 WAKFIL: Wake-up Filter bit
                     // 1 = Wake-up filter enabled
                     // 0 = Wake-up filter disabled
    #define PHSEG2 5 // bit2-0 PHSEG2<2:0>: PS2 Length bits
                     // (PHSEG2 + 1) x TQ
                     // Minimum valid setting for PS2 is 2 TQ
    temp = (SOF << 7) + (WAKFIL << 6) + PHSEG2;
    // put_hex(temp);
    mcp2515_write(CNF3, temp);
    
    // Change operation mode from configuration to normal
    // CANCTRL
    #define REQOP 0b000 // bit7-5 REQOP<2:0>: R/W-100: Request Operation mode bits
                        // 000 = Set Normal Operation mode
                        // 001 = Set Sleep mode
                        // 010 = Set Loopback mode
                        // 011 = Set Listen-Only mode
                        // 100 = Set Configuration mod
    #define ABAT 0      // bit4 ABAT: R/W-0: Abort All Pending Transmissions bit
                        // 1 = Request abort of all pending transmit buffers
                        // 0 = Terminate request to abort all transmissions
    #define OSM 0       // bit3 OSM: R/W-0: One-Shot mode bit
                        // 1 = Enabled. Message will only attempt to transmit one time
                        // 0 = Disabled. Messages will reattempt transmission, if required
    #define CLKEN 1     // bit2 CLKEN: R/W-1: CLKOUT Pin Enable bit
                        // 1 = CLKOUT pin enabled
                        // 0 = CLKOUT pin disabled (Pin is in high-impedance state)
    #define CLKPRE 0b11 // bit1-0 CLKPRE<1:0>: R/W-11: CLKOUT Pin Prescaler bits
                        // 00 = FCLKOUT = System Clock/1
                        // 01 = FCLKOUT = System Clock/2
                        // 10 = FCLKOUT = System Clock/4
                        // 11 = FCLKOUT = System Clock/8
    temp = CLKPRE + (CLKEN << 2) + (OSM << 3) + (ABAT << 4) + (REQOP << 5);
    // put_hex(temp);
    mcp2515_write(CANCTRL, temp);
    
    // Configure Receive Buffer 0
    // RXB0CTRL
    #define RXB0CTRL_RXM_BIT 5     // bit 6-5 RXM<1:0>: Receive Buffer Operating mode bits
                                   //         11 = Turn mask/filters off; receive any message
                                   //         10 = Receive only valid messages with extended identifiers that meet filter criteria
                                   //         01 = Receive only valid messages with standard identifiers that meet filter criteria. Extended ID filter
                                   //           registers RXFnEID8:RXFnEID0 are ignored for the messages with standard IDs.
                                   //         00 = Receive all valid messages using either standard or extended identifiers that meet filter criteria.
                                   //           Extended ID filter registers RXFnEID8:RXFnEID0 are applied to first two bytes of data in the
                                   //           messages with standard IDs.
    #define RXB0CTRL_RXM 0b11 // catch 'em all
    #define RXB0CTRL_RXRTR_BIT 3   // bit 3   RXRTR: R: Received Remote Transfer Request bit
                                   //         1 = Remote Transfer Request Received
                                   //         0 = No Remote Transfer Request Received
    #define RXB0CTRL_BUKT_BIT 2    // bit 2   BUKT: R/W: Rollover Enable bit
                                   //         1 = RXB0 message will rollover and be written to RXB1 if RXB0 is full
                                   //         0 = Rollover disabled
    #define RXB0CTRL_BUKT 0
    #define RXB0CTRL_BUKT1_BIT 1   // bit 1   BUKT1: R: Read-only Copy of BUKT bit (used internally bu the MCP2515)
    #define RXB0CTRL_FILHIT0_BIT 0 // bit 0   FILHIT0: R: Filter Hit bit – indicates which acceptance filter enabled reception of message
                                   //         1 = Acceptance Filter 1 (RXF1)
                                   //         0 = Acceptance Filter 0 (RXF0)
    mcp2515_write(RXB0CTRL, (RXB0CTRL_RXM << RXB0CTRL_RXM_BIT) + (RXB0CTRL_BUKT << RXB0CTRL_BUKT_BIT));
    
    // Configure Receive Buffer 1
    // RXB1CTRL
    #define RXB1CTRL_RXM_BIT 5    // bit 6-5 RXM<1:0>: R/W: Receive Buffer Operating mode bits
                                  //         11 = Turn mask/filters off; receive any message
                                  //         10 = Receive only valid messages with extended identifiers that meet filter criteria
                                  //         01 = Receive only valid messages with standard identifiers that meet filter criteria
                                  //         00 = Receive all valid messages using either standard or extended identifiers that meet filter criteria
    #define RXB1CTRL_RXM 0b10
    #define RXB1CTRL_RXRTR_BIT 3  // bit 3   RXRTR: R: Received Remote Transfer Request bit
                                  //         1 = Remote Transfer Request Received
                                  //         0 = No Remote Transfer Request Received
    #define RXB1CTRL_FILHIT_BIT 0 // bit 2-0 FILHIT<2:0>: R: Filter Hit bits - indicates which acceptance filter enabled reception of message
                                  //         101 = Acceptance Filter 5 (RXF5)
                                  //         100 = Acceptance Filter 4 (RXF4)
                                  //         011 = Acceptance Filter 3 (RXF3)
                                  //         010 = Acceptance Filter 2 (RXF2)
                                  //         001 = Acceptance Filter 1 (RXF1) (Only if BUKT bit set in RXB0CTRL)
                                  //         000 = Acceptance Filter 0 (RXF0) (Only if BUKT bit set in RXB0CTRL)
    mcp2515_write(RXB1CTRL, (RXB1CTRL_RXM << RXB1CTRL_RXM_BIT));
    
    // Set RXnBF pin control and status
    #define BFPCTRL_B1BFS_BIT 5 // bit 5 B1BFS: R/W-0: RX1BF Pin State bit (Digital Output mode only)
                                //       - Reads as ‘0’ when RX1BF is configured as interrupt pin
    #define BFPCTRL_B1BFS 0
    #define BFPCTRL_B0BFS_BIT 4 // bit 4 B0BFS: R/W-0: RX0BF Pin State bit (Digital Output mode only)
                                //       - Reads as ‘0’ when RX0BF is configured as interrupt pin
    #define BFPCTRL_B0BFS 0
    #define BFPCTRL_B1BFE_BIT 3 // bit 3 B1BFE: R/W-0: RX1BF Pin Function Enable bit
                                //       1 = Pin function enabled, operation mode determined by B1BFM bit
                                //       0 = Pin function disabled, pin goes to high-impedance state
    #define BFPCTRL_B1BFE 0
    #define BFPCTRL_B0BFE_BIT 2 // bit 2 B0BFE: R/W-0: RX0BF Pin Function Enable bit
                                //       1 = Pin function enabled, operation mode determined by B0BFM bit
                                //       0 = Pin function disabled, pin goes to high-impedance state
    #define BFPCTRL_B0BFE 0
    #define BFPCTRL_B1BFM_BIT 1 // bit 1 B1BFM: R/W-0: RX1BF Pin Operation mode bit
                                //       1 = Pin is used as interrupt when valid message loaded into RXB1
                                //       0 = Digital Output mode
    #define BFPCTRL_B1BFM 0
    #define BFPCTRL_B0BFM_BIT 0 // bit 0 B0BFM: R/W-0: RX0BF Pin Operation mode bit
                                //       1 = Pin is used as interrupt when valid message loaded into RXB0
                                //       0 = Digital Output mode
    #define BFPCTRL_B0BFM 1
    temp = (BFPCTRL_B1BFS << BFPCTRL_B1BFS_BIT)
         + (BFPCTRL_B0BFS << BFPCTRL_B0BFS_BIT)
         + (BFPCTRL_B1BFE << BFPCTRL_B1BFE_BIT)
         + (BFPCTRL_B0BFE << BFPCTRL_B0BFE_BIT)
         + (BFPCTRL_B1BFM << BFPCTRL_B1BFM_BIT)
         + (BFPCTRL_B0BFM << BFPCTRL_B0BFM_BIT);
    mcp2515_write(BFPCTRL, temp);

    // DONE initialization

    // dump all register values
    {
        int i;
        uint8_t addr;
        uint8_t buf[18];
        for (addr = 0; addr < 0x80; addr += 0x10) {
            mcp2515_read(addr, buf, 16);
            // put_hex(addr);
            for (i = 0; i < 16; ++i)
                put_hex(buf[i+2]);
            SERIAL_UartPutString("\r\n");
        }
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

uint8_t midi_target_channel = 0; // channel 1 -- TODO: make this configurable
uint8_t voices = 1;  // TODO: make this configurable
    
int main()
{
    uint32 ch;
    uint8 led;
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_MIDI_Start();
    SPIM_CAN_Start();
    SERIAL_Start();
    // UART_MIDI_SetCustomInterruptHandler(ISR_MIDI);
    
    // Pin_CAN_Enable_Write(1);
    // Pin_CAN_Reset_Write(1);
    
    mcp2515_init();

    
    // test writing data to TxB0 data registers
    /*
    for (i = 0; i < 8; ++i)
        buf[7-i] = i + 1;
    mcp2515_write_array(TXB0D0, buf, 8);
    set_can_id_std(TXB0SIDH, 0x555);
    mcp2515_write(TXB0DLC, 1); // length, bit6=RTR
    mcp2515_write(TXB0EID8, 0);
    mcp2515_write(TXB0EID0, 0);
    */
    
    // mcp2515_message_request_to_send_txb0();
    // done test
    
    SERIAL_UartPutString("\r\nHello World!\r\n");
    
    led = 0;
    Pin_LED_R_Write(led);
    Pin_LED_G_Write(1);
    Pin_LED_B_Write(~led);
    
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
                                note_on(midi_data[0], midi_data[1]);
                                break;
                            }
                            // else fall down to note off
                        case MIDI_NOTE_OFF:
                            note_off(midi_data[0], midi_data[1]);
                            break;
                        }
                    }
                }
            }
            
            /*
            mcp2515_write(TXB0D0, ch);
            mcp2515_message_request_to_send_txb0();
            // toggle LED
            led ^= 1;
            Pin_LED_R_Write(led);
            Pin_LED_B_Write(~led);
            put_hex(ch);
            */
        }
    }
}

void note_on(uint8_t key, uint8_t velocity)
{
}

void note_off(uint8_t key, uint8_t velocity)
{
}

void mcp2515_reset()
{
    SPIM_CAN_WriteTxData(CAN_CTRL_RESET);
}

void mcp2515_read(uint8_t address, uint8_t data[], uint8_t length)
{
    while (SPIM_CAN_GetRxBufferSize()) {
        SPIM_CAN_ReadRxData();
    }
    
    uint8_t toWrite = length;
    length += 2;

    while(0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_EMPTY))
    {
    }

    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_READ);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);
        
    while (length > 0) {
        if (toWrite > 0 && (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_NOT_FULL)) {
            CY_SET_REG8(SPIM_CAN_TXDATA_PTR, 0);
            --toWrite;
        }
        if (SPIM_CAN_RX_STATUS_REG & SPIM_CAN_STS_RX_FIFO_NOT_EMPTY) {
            *data++ = CY_GET_REG8(SPIM_CAN_RXDATA_PTR);
            --length;
        }
    }
}

void mcp2515_write(uint8_t address, uint8_t data)
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    
    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_WRITE);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data);
}

void mcp2515_write_array(uint8_t address, uint8_t data[], uint8_t length)
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_WRITE);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);

    while (1) {
        if ((SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_NOT_FULL) != 0) {
            CY_SET_REG8(SPIM_CAN_TXDATA_PTR, *data++);
            if (--length == 0)
                break;
        }
    }
}

void mcp2515_message_request_to_send_txb0()
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_RTS_TXB0);
}

void mcp2515_message_request_to_send_txb1()
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_RTS_TXB1);
}

void mcp2515_message_request_to_send_txb2()
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTRL_RTS_TXB2);
}

void SPIM_CAN_WriteTxData24(uint8 data1, uint8 data2, uint8_t data3) 
{
    /* Wait until TX FIFO has a place */
    while(0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_EMPTY))
    {
    }

    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data1);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data2);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data3);
}

void SPIM_CAN_WriteTxData2plusN(uint8 data1, uint8 data2, uint8 n) 
{
    /* Wait until TX FIFO has a place */
    while(0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_EMPTY)) {}

    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data1);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data2);
    
    while (--n >= 0) {
        while (0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_NOT_FULL)) {}
        CY_SET_REG8(SPIM_CAN_TXDATA_PTR, 0);    
    }
}

/* [] END OF FILE */
