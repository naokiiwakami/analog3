/*******************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:
*  This is source code for example project of the SPI Master component.
*  Parameters used:
*   Mode                CPHA == 0, CPOL == 0                
*   Data lines          MOSI+MISO
*   Shift direction     MSB First
*   DataBits:           8 
*   Bit Rate            1 Mbps
*   Clock source        External 
*
*  SPI communication test using DMA. 8 bytes are transmitted
*  between SPI Master and SPI Slave.
*  Received data are displayed on LCD. 
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>

void DmaTxConfiguration(void);
void DmaRxConfiguration(void);

/* DMA Configuration for DMA_TX */
#define DMA_TX_BYTES_PER_BURST      (1u)
#define DMA_TX_REQUEST_PER_BURST    (1u)
#define DMA_TX_SRC_BASE             (CYDEV_SRAM_BASE)
#define DMA_TX_DST_BASE             (CYDEV_PERIPH_BASE)

/* DMA Configuration for DMA_RX */
#define DMA_RX_BYTES_PER_BURST      (1u)
#define DMA_RX_REQUEST_PER_BURST    (1u)
#define DMA_RX_SRC_BASE             (CYDEV_PERIPH_BASE)
#define DMA_RX_DST_BASE             (CYDEV_SRAM_BASE)

#define BUFFER_SIZE                 (8u)
#define STORE_TD_CFG_ONCMPLT        (1u)

/* Variable declarations for DMA_TX*/
uint8 txChannel;
uint8 txTD;

/* Variable declarations for DMA_RX */
uint8 rxChannel;
uint8 rxTD;

/* Data buffers */
uint8 txBuffer [BUFFER_SIZE] = {0x0u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u};
uint8 rxBuffer[BUFFER_SIZE];

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  Main function performs following functions:
*   1. Starts Character LCD and print project info
*   2. Starts SPI Master component
*   3. Configures the DMA transfer for RX and TX directions
*   4. Displays the results on Character LCD
*   
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
int main()
{
    uint8 i;
    
    LCD_Start();
    LCD_Position(0u,0u);
    LCD_PrintString("SPI Master");
    LCD_Position(1u,0u);
    LCD_PrintString("example");
    CyDelay(2000u);
    
    DmaTxConfiguration();
    DmaRxConfiguration();
    
    SPIM_Start();
    
    CyDmaChEnable(rxChannel, STORE_TD_CFG_ONCMPLT);
    CyDmaChEnable(txChannel, STORE_TD_CFG_ONCMPLT);

    while (0u == (SPIM_ReadTxStatus() & SPIM_STS_SPI_DONE))
    {
    }
    
    LCD_ClearDisplay();
    LCD_PrintString("Master Rx data:");
    LCD_Position(1u,0u);
    
    for(i=0u; i<BUFFER_SIZE; i++)
    {
        LCD_PrintHexUint8(rxBuffer[i]);
    }

    for(;;)
    {
    }
}

    
/*******************************************************************************
* Function Name: DmaTxConfiguration
********************************************************************************
* Summary:
*  Configures the DMA transfer for TX direction
*   
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void DmaTxConfiguration()
{
    /* Init DMA, 1 byte bursts, each burst requires a request */ 
    txChannel = DMA_TX_DmaInitialize(DMA_TX_BYTES_PER_BURST, DMA_TX_REQUEST_PER_BURST, 
                                        HI16(DMA_TX_SRC_BASE), HI16(DMA_TX_DST_BASE));

    txTD = CyDmaTdAllocate();

    /* Configure this Td as follows:
    *  - Increment the source address, but not the destination address   
    */
    CyDmaTdSetConfiguration(txTD, BUFFER_SIZE, CY_DMA_DISABLE_TD, TD_INC_SRC_ADR);

    /* From the memory to the SPIM */
    CyDmaTdSetAddress(txTD, LO16((uint32)txBuffer), LO16((uint32) SPIM_TXDATA_PTR));
    
    /* Associate the TD with the channel */
    CyDmaChSetInitialTd(txChannel, txTD); 
}    


/*******************************************************************************
* Function Name: DmaRxConfiguration
********************************************************************************
* Summary:
*  Configures the DMA transfer for RX direction
*   
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void DmaRxConfiguration()
{ 
    /* Init DMA, 1 byte bursts, each burst requires a request */ 
    rxChannel = DMA_RX_DmaInitialize(DMA_RX_BYTES_PER_BURST, DMA_RX_REQUEST_PER_BURST,
                                     HI16(DMA_RX_SRC_BASE), HI16(DMA_RX_DST_BASE));

    rxTD = CyDmaTdAllocate();
    
    /* Configure this Td as follows:
    *  - Increment the destination address, but not the source address
    */
    CyDmaTdSetConfiguration(rxTD, BUFFER_SIZE, CY_DMA_DISABLE_TD, TD_INC_DST_ADR);

    /* From the SPIM to the memory */
    CyDmaTdSetAddress(rxTD, LO16((uint32)SPIM_RXDATA_PTR), LO16((uint32)rxBuffer));

    /* Associate the TD with the channel */
    CyDmaChSetInitialTd(rxChannel, rxTD);
}
   
	
/* [] END OF FILE */
