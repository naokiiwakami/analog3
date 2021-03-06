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
#define CAN_CTL_RESET          0xc0 // 1100 0000
#define CAN_CTL_READ           0x03 // 0000 0011
#define CAN_CTL_READ_RX_BUFFER 0x90 // 1001 0nm0
#define CAN_CTL_WRITE          0x02 // 0000 0010
#define CAN_CTL_LOAD_TX_BUFFER 0x40 // 0100 0abc
#define CAN_CTL_RTS_TXB0       0x81 // 1000 0001
#define CAN_CTL_RTS_TXB1       0x82 // 1000 0010
#define CAN_CTL_RTS_TXB2       0x84 // 1000 0100
#define CAN_CTL_READ_STATUS    0xa0 // 1010 0000
#define CAN_CTL_RX_STATUS      0xb0 // 1011 0000
#define CAN_CTL_BIT_MODIFY     0x05 // 0000 0101

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

#define CANINTF_RX0IF_BIT 0
#define CANINTF_RX1IF_BIT 1
#define CANINTF_TX0IF_BIT 2
#define CANINTF_TX1IF_BIT 3
#define CANINTF_TX2IF_BIT 4
#define CANINTF_ERRIF_BIT 5
#define CANINTF_WAKIF_BIT 6
#define CANINTF_MERRF_BIT 7

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

void mcp2515_read_into_queue(uint8_t address, uint8_t length);
void mcp2515_read_rxb0_into_queue();

int32_t my_device_id = 100;

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
    // TXBnSIDL:  SID2  SID1  SID0   -   EXIDE   -   EID17 EID16
    id <<= 5;
    data[1] = (uint8_t) id;
    id >>= 8;
    data[0] = (uint8_t) id;
    mcp2515_write_array(register_address, data, 2);
}

void init_can()
{
    uint8_t temp;
    uint8_t buf[18];
    uint8_t addr;
    uint8_t i;
    uint8 state = CyEnterCriticalSection();
    
    mcp2515_reset();

    // configure baud rate
    // configured for 1 MHz baud rate with 20 MHz fosc
    //
    // |Sync(1TQ)| PropSeg | PS1 | PS2 |
    // | <-- NBT (normal bit time) --> |
    //
    //   TQ = 2 * (BRP + 1) / fosc
    //   TQ is set to 2 / fosc = 1 / 10M that makes NBT = 10 Tq
    //
    //  Requirements for programming of the time segments are:
    //
    //  PropSeg + PS1 >= PS2
    //  PropSeg + PS1 >= Tdelay
    //  PS2 > SJW
    //  Typically, the sampling of the bit should take place at
    //  about 60-70% of the bit time.
    //  Also, typically, the TDELAY is 1-2 TQ.
    //
    //   We set parameters as follows
    //   PropSeg = 2 Tq
    //   PS1 = 3Tq
    //   PS2 = 4Tq
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

    // CNF2
    #define BTLMODE 1 // bit7 BTLMODE: PS2 Bit Time Length bit
                      // 0 = Length of PS2 is the greater of PS1 and IPT (2 TQ)
                      // 1 = Length of PS2 determined by PHSEG22:PHSEG20 bits of CNF3
    #define SAM 1     // bit6 SAM: Sample Point Configuration bit
                      // 1 = Bus line is sampled three times at the sample point
                      // 0 = Bus line is sampled once at the sample point
    #define PHSEG1 2  // bit5-3 PHSEG1<2:0>: PS1 Length bits
                      // (PHSEG1 + 1) x TQ
    #define PRSEG 1   // bit2-0 PRSEG<2:0>: Propagation Segment Length bit
                      // (PRSEG + 1) x TQ
    temp = (BTLMODE << 7) + (SAM << 6) + (PHSEG1 << 3) + PRSEG;
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
    #define PHSEG2 3 // bit2-0 PHSEG2<2:0>: PS2 Length bits
                     // (PHSEG2 + 1) x TQ
                     // Minimum valid setting for PS2 is 2 TQ
    temp = (SOF << 7) + (WAKFIL << 6) + PHSEG2;
    // put_hex(temp);
    mcp2515_write(CNF3, temp);
    
    // Change operation mode from configuration to normal
    // CANCTRL
    #define REQOP 0b000 // bit7-5 REQOP<2:0>: Request Operation mode bits
                        // 000 = Set Normal Operation mode
                        // 001 = Set Sleep mode
                        // 010 = Set Loopback mode
                        // 011 = Set Listen-Only mode
                        // 100 = Set Configuration mod
    #define ABAT 0      // bit4 ABAT: Abort All Pending Transmissions bit
                        // 1 = Request abort of all pending transmit buffers
                        // 0 = Terminate request to abort all transmissions
    #define OSM 0       // bit3 OSM: One-Shot mode bit
                        // 1 = Enabled. Message will only attempt to transmit one time
                        // 0 = Disabled. Messages will reattempt transmission, if required
    #define CLKEN 1     // bit2 CLKEN: CLKOUT Pin Enable bit
                        // 1 = CLKOUT pin enabled
                        // 0 = CLKOUT pin disabled (Pin is in high-impedance state)
    #define CLKPRE 0b11 // bit1-0 CLKPRE<1:0>: CLKOUT Pin Prescaler bits
                        // 00 = FCLKOUT = System Clock/1
                        // 01 = FCLKOUT = System Clock/2
                        // 10 = FCLKOUT = System Clock/4
                        // 11 = FCLKOUT = System Clock/8
    temp = CLKPRE + (CLKEN << 2) + (OSM << 3) + (ABAT << 4) + (REQOP << 5);
    // put_hex(temp);
    mcp2515_write(CANCTRL, temp);
    
    // Configure Receive Buffer 0
    // RXB0CTRL
    #define RXB0CTRL_RXM 0b11 // catch 'em all
    #define RXB0CTRL_RXM_BIT 5     // bit 6-5 RXM<1:0>: R/W-00: Receive Buffer Operating mode bits
                                   //         11 = Turn mask/filters off; receive any message
                                   //         10 = Receive only valid messages with extended identifiers that meet filter criteria
                                   //         01 = Receive only valid messages with standard identifiers that meet filter criteria. Extended ID filter
                                   //           registers RXFnEID8:RXFnEID0 are ignored for the messages with standard IDs.
                                   //         00 = Receive all valid messages using either standard or extended identifiers that meet filter criteria.
                                   //           Extended ID filter registers RXFnEID8:RXFnEID0 are applied to first two bytes of data in the
                                   //           messages with standard IDs.
    #define RXB0CTRL_RXRTR_BIT 3   // bit 3   RXRTR: R-0: Received Remote Transfer Request bit
                                   //         1 = Remote Transfer Request Received
                                   //         0 = No Remote Transfer Request Received
    #define RXB0CTRL_BUKT 0
    #define RXB0CTRL_BUKT_BIT 2    // bit 2   BUKT: R/W-0: Rollover Enable bit
                                   //         1 = RXB0 message will rollover and be written to RXB1 if RXB0 is full
                                   //         0 = Rollover disabled
    #define RXB0CTRL_BUKT1_BIT 1   // bit 1   BUKT1: R-0: Read-only Copy of BUKT bit (used internally bu the MCP2515)
    #define RXB0CTRL_FILHIT0_BIT 0 // bit 0   FILHIT0: R-0: Filter Hit bit – indicates which acceptance filter enabled reception of message
                                   //         1 = Acceptance Filter 1 (RXF1)
                                   //         0 = Acceptance Filter 0 (RXF0)
    mcp2515_write(RXB0CTRL, (RXB0CTRL_RXM << RXB0CTRL_RXM_BIT) + (RXB0CTRL_BUKT << RXB0CTRL_BUKT_BIT));

    // Configure Receive Buffer 1
    // RXB1CTRL
    #define RXB1CTRL_RXM 0b10
    #define RXB1CTRL_RXM_BIT 5    // bit 6-5 RXM<1:0>: R/W-00: Receive Buffer Operating mode bits
                                  //         11 = Turn mask/filters off; receive any message
                                  //         10 = Receive only valid messages with extended identifiers that meet filter criteria
                                  //         01 = Receive only valid messages with standard identifiers that meet filter criteria
                                  //         00 = Receive all valid messages using either standard or extended identifiers that meet filter criteria
    #define RXB1CTRL_RXRTR_BIT 3  // bit 3   RXRTR: R-0: Received Remote Transfer Request bit
                                  //         1 = Remote Transfer Request Received
                                  //         0 = No Remote Transfer Request Received
    #define RXB1CTRL_FILHIT_BIT 0 // bit 2-0 FILHIT<2:0>: R-0: Filter Hit bits - indicates which acceptance filter enabled reception of message
                                  //         101 = Acceptance Filter 5 (RXF5)
                                  //         100 = Acceptance Filter 4 (RXF4)
                                  //         011 = Acceptance Filter 3 (RXF3)
                                  //         010 = Acceptance Filter 2 (RXF2)
                                  //         001 = Acceptance Filter 1 (RXF1) (Only if BUKT bit set in RXB0CTRL)
                                  //         000 = Acceptance Filter 0 (RXF0) (Only if BUKT bit set in RXB0CTRL)
    mcp2515_write(RXB1CTRL, (RXB1CTRL_RXM << RXB1CTRL_RXM_BIT));
    
    // Set RXnBF pin control and status
    #define BFPCTRL_B1BFS 0
    #define BFPCTRL_B1BFS_BIT 5 // bit 5 B1BFS: R/W-0: RX1BF Pin State bit (Digital Output mode only)
                                //       - Reads as ‘0’ when RX1BF is configured as interrupt pin
    #define BFPCTRL_B0BFS 0
    #define BFPCTRL_B0BFS_BIT 4 // bit 4 B0BFS: R/W-0: RX0BF Pin State bit (Digital Output mode only)
                                //       - Reads as ‘0’ when RX0BF is configured as interrupt pin
    #define BFPCTRL_B1BFE 0
    #define BFPCTRL_B1BFE_BIT 3 // bit 3 B1BFE: R/W-0: RX1BF Pin Function Enable bit
                                //       1 = Pin function enabled, operation mode determined by B1BFM bit
                                //       0 = Pin function disabled, pin goes to high-impedance state
    #define BFPCTRL_B0BFE 1
    #define BFPCTRL_B0BFE_BIT 2 // bit 2 B0BFE: R/W-0: RX0BF Pin Function Enable bit
                                //       1 = Pin function enabled, operation mode determined by B0BFM bit
                                //       0 = Pin function disabled, pin goes to high-impedance state
    #define BFPCTRL_B1BFM 0
    #define BFPCTRL_B1BFM_BIT 1 // bit 1 B1BFM: R/W-0: RX1BF Pin Operation mode bit
                                //       1 = Pin is used as interrupt when valid message loaded into RXB1
                                //       0 = Digital Output mode
    #define BFPCTRL_B0BFM 1
    #define BFPCTRL_B0BFM_BIT 0 // bit 0 B0BFM: R/W-0: RX0BF Pin Operation mode bit
                                //       1 = Pin is used as interrupt when valid message loaded into RXB0
                                //       0 = Digital Output mode
    temp = (BFPCTRL_B1BFS << BFPCTRL_B1BFS_BIT)
         + (BFPCTRL_B0BFS << BFPCTRL_B0BFS_BIT)
         + (BFPCTRL_B1BFE << BFPCTRL_B1BFE_BIT)
         + (BFPCTRL_B0BFE << BFPCTRL_B0BFE_BIT)
         + (BFPCTRL_B1BFM << BFPCTRL_B1BFM_BIT)
         + (BFPCTRL_B0BFM << BFPCTRL_B0BFM_BIT);
    mcp2515_write(BFPCTRL, temp);
    
    SERIAL_UartPutString("\r\nDone configuring CAN controller:\r\n");
    
    // DONE initialization
    
    // test writing data to TxB0 data registers
    for (i = 0; i < 8; ++i)
        buf[7-i] = i + 1;
    mcp2515_write_array(TXB0D0, buf, 8);
    set_can_id_std(TXB0SIDH, 0x555);
    mcp2515_write(TXB0DLC, 8);
    mcp2515_write(TXB0EID8, 0);
    mcp2515_write(TXB0EID0, 0);
    
    mcp2515_message_request_to_send_txb0();
    // done test
    
    // dump all register values
    {
        int i;
        for (addr = 0; addr < 0x80; addr += 0x10) {
            mcp2515_read(addr, buf, 16);
            // put_hex(addr);
            for (i = 0; i < 16; ++i)
                put_hex(buf[i+2]);
            SERIAL_UartPutString("\r\n");
        }
    }
    CyExitCriticalSection(state);
    
    SERIAL_UartPutString("\r\nlistening...\r\n");    
}


uint8 led = 1;
CY_ISR(ISR_USER_SW)
{
    Pin_LED_B_Write(0);
    init_can();
    Pin_LED_B_Write(1);    
}

#define QUEUE_SIZE 128
volatile uint8_t q_first = 0;
volatile uint8_t q_last = 0;
volatile uint8_t queue_array[QUEUE_SIZE];

inline void queue_add(uint8_t val)
{
    queue_array[q_last++] = val;
    if (q_last == QUEUE_SIZE)
        q_last = 0;
}

inline uint8_t queue_remove()
{
    uint8_t val = queue_array[q_first++];
    if (q_first == QUEUE_SIZE)
        q_first = 0;
    return val;
}

#define queue_empty() (q_first == q_last)

#define RXBnSIDL_SRR_BIT 4
#define RXBnSIDL_IDE_BIT 3

#define RXBnDLC_RFR_BIT 6

CY_ISR(ISR_RX0BF)
{
    mcp2515_read_into_queue(RXB0SIDH, 13);
    mcp2515_bit_modify(CANINTF, (1 << CANINTF_RX0IF_BIT), 0);
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    SPIM_CAN_Start();
    SERIAL_Start();
    
    init_can();

    led = 1;
    Pin_LED_B_Write(led);

    // start listening
    isr_1_ClearPending();
    isr_1_StartEx(ISR_USER_SW);
    
    isr_RX0BF_ClearPending();
    isr_RX0BF_StartEx(ISR_RX0BF);

    for(;;)
    {
        if (!queue_empty()) {
            //               7     6     5     4     3     2     1     0
            // RXBnSIDH: SID10  SID9  SID8  SID7  SID6  SID5  SID4  SID3
            // RXBnSIDL:  SID2  SID1  SID0   SRR   IDE   -   EID17 EID16
            uint8_t value = queue_remove(); // RXB0SIDH
            uint16_t sid = value;
            sid <<= 3;
            value = queue_remove(); // RXB0SIDL
            sid |= value >> 5;
            uint8_t extended = value & (1 << 3);
            uint8_t is_remote = value & (1 << 4);
            uint32_t eid = value & 0x3;
            eid <<= 8;
            //               7     6     5     4     3     2     1     0
            // RXBnEID8: EID15 EID14 EID13 EID12 EID11 EID10  EID9  EID8
            // RXBnEID0:  EID7  EID6  EID5  EID4  EID3  EID2  EID1  EID0
            value = queue_remove(); // RXB0EID8
            eid |= value;
            eid <<= 8;
            value = queue_remove(); // RXB0EID0
            eid |= value;
            //               7     6     5     4     3     2     1     0
            // RXBnDLC:    -     RTR   RB1   RB0  DLC3  DLC2  DLC1  DLC0
            value = queue_remove(); // RXB0DLC
            if (extended)
                is_remote = value & (1 << 6);
            uint8_t len = value & 0x0f;    
            
            if (!extended) {
                SERIAL_UartPutString("std[");
                put_hex(sid >> 8);
                put_hex(sid);
            }
            else {
                SERIAL_UartPutString("ext[");
                put_hex(eid >> 16);
                put_hex(eid >> 8);
                put_hex(eid);
            }
            if (is_remote) {
                SERIAL_UartPutString("]: REMOTE\r\n");                
            }
            else {
                SERIAL_UartPutString("]:");                                
                int i;
                for (i = 0; i < 8; ++i) {
                    value = queue_remove();
                    if (i < len)
                        put_hex(value);
                }
                SERIAL_UartPutString("\r\n");                                
            }
        }
    }
}

void mcp2515_reset()
{
    SPIM_CAN_WriteTxData(CAN_CTL_RESET);
}

void mcp2515_read(uint8_t address, uint8_t data[], uint8_t length)
{
    while (SPIM_CAN_GetRxBufferSize()) {
        SPIM_CAN_ReadRxData();
    }
    // SPIM_CAN_ClearRxBuffer();
    
    uint8_t toWrite = length;
    length += 2;

    while(0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_EMPTY))
    {
    }

    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_READ);
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

void mcp2515_read_into_queue(uint8_t address, uint8_t length)
{
    while (SPIM_CAN_GetRxBufferSize()) {
        SPIM_CAN_ReadRxData();
    }
    // SPIM_CAN_ClearRxBuffer();
    
    uint8_t toWrite = length;
    length += 2;

    while(0u == (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_EMPTY))
    {
    }

    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_READ);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);
        
    uint8_t read = 0;
    uint8_t data;
    while (read < length) {
        if (toWrite > 0 && (SPIM_CAN_TX_STATUS_REG & SPIM_CAN_STS_TX_FIFO_NOT_FULL)) {
            CY_SET_REG8(SPIM_CAN_TXDATA_PTR, 0);
            --toWrite;
        }
        if (SPIM_CAN_RX_STATUS_REG & SPIM_CAN_STS_RX_FIFO_NOT_EMPTY) {
            data = CY_GET_REG8(SPIM_CAN_RXDATA_PTR);
            if (read >= 2)
                queue_add(data);
            ++read;
        }
    }
}

void mcp2515_write(uint8_t address, uint8_t data)
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    
    /* Put data element into the TX FIFO */
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_WRITE);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data);
}

void mcp2515_write_array(uint8_t address, uint8_t data[], uint8_t length)
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_WRITE);
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
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_RTS_TXB0);
}

void mcp2515_message_request_to_send_txb1()
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_RTS_TXB1);
}

void mcp2515_message_request_to_send_txb2()
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_RTS_TXB2);
}

void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
    while ((SPIM_CAN_ReadTxStatus() & SPIM_CAN_STS_SPI_DONE) == 0) {}
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, CAN_CTL_BIT_MODIFY);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, address);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, mask);
    CY_SET_REG8(SPIM_CAN_TXDATA_PTR, data);
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
