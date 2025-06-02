/*******************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:
*  This example project demonstrates the basic operation of the SCB component
*  in the UART mode. The polling method is used to wait for received character.
*  As soon as character is received it is transmitted back. The serial terminal
*  can be used on the PC to send characters and get them back.
*  The example will echo every received character.
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  The main function performs the following actions:
*   1. Sets up UART component.
*   2. UART sends text header into the serial terminal.
*   3. UART waits for the characters to send them back to the serial terminal.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    uint32 ch;

    /* Start SCB (UART mode) operation */
    UART_Start();

    UART_UartPutString("\r\n***********************************************************************************\r\n");
    UART_UartPutString("This is SCB_UartComm datasheet example project\r\n");
    UART_UartPutString("If you are able to read this text the terminal connection is configured correctly.\r\n");
    UART_UartPutString("Start transmitting the characters to see an echo in the terminal.\r\n");
    UART_UartPutString("\r\n");

    for (;;)
    {
        /* Get received character or zero if nothing has been received yet */
        ch = UART_UartGetChar();

        if (0u != ch)
        {
            /* Transmit the data through UART.
            * This functions is blocking and waits until there is a place in
            * the buffer.
            */
            UART_UartPutChar(ch);
        }
    }
}


/* [] END OF FILE */
