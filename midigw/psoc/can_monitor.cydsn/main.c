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
#include <string.h>

#include "mcp2515.h"


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

uint8_t pnt = 0;
char command[128];

void process_command(/*char command[], uint8_t len*/)
{
    if (strncasecmp(command, "reqPerfId", 4) == 0) {
        SERIAL_UartPutString("0x04\r\n");
    }
    else {
        SERIAL_UartPutString("Unknown command\r\n");
    }
}

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    SPIM_CAN_Start();
    SERIAL_Start();
    
    init_can();
    
    // for (;;) {}
    
    // dump all register values
    {
        SERIAL_UartPutString("Done configuring CAN controller:\r\n");

        int i;
        uint8_t buf[18];
        uint8_t addr;

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

        for (addr = 0; addr < 0x80; addr += 0x10) {
            mcp2515_read(addr, buf, 16);
            // put_hex(addr);
            for (i = 0; i < 16; ++i)
                put_hex(buf[i+2]);
            SERIAL_UartPutString("\r\n");
        }
    }
    SERIAL_UartPutString("\r\nlistening...\r\n");    


    led = 0;
    Pin_LED_B_Write(led);

    // start listening
    isr_1_ClearPending();
    isr_1_StartEx(ISR_USER_SW);
    
    isr_RX0BF_ClearPending();
    isr_RX0BF_StartEx(ISR_RX0BF);
    
    for(;;)
    {
        uint32 ch = SERIAL_UartGetChar();
        if (ch != 0) {
            if (ch == '\r') {
                SERIAL_UartPutString("\r\n");
                command[pnt] = 0;
                process_command(/*str, pnt*/);
                pnt = 0;
            }
            else {
                SERIAL_UartPutChar(ch);
                command[pnt++] = ch;
            }
        }
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
            eid |= sid << 18;
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
                put_hex(eid >> 24);
                put_hex(eid >> 16);
                put_hex(eid >> 8);
                put_hex(eid);
            }
            if (is_remote) {
                SERIAL_UartPutString(" ]: REMOTE\r\n");                
            }
            else {
                SERIAL_UartPutString(" ]:");                                
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

/* [] END OF FILE */
