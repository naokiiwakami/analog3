/*******************************************************************************
* File Name: main.c
*
* Version: 2.1
*
* Description:
*  This project demonstrates the UART reception mechanism. 
*  Data typed on the hyperterminal is sent through serial port and displayed
*  on the LCD.
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <device.h>

#define LCD_NUM_COLUMNS (16u)


int main()
{   
    char8 ch;       /* Data received from the Serial port */
    uint8 count = 0u;
    uint8 pos = 0u;

    CyGlobalIntEnable; /* Enable all interrupts by the processor. */

    LCD_Char_1_Start();
    UART_1_Start();

    while(1)
    {
        /* Check the UART status */
        ch = UART_1_GetChar();

        /* If byte received */
        if(ch > 0u)
        {
            count++;        
            /* If the count value reaches the count 16 start from first location */
            if(count % LCD_NUM_COLUMNS == 0u) 
            {
                pos = 0u; /* resets the count value */
                /* Display will be cleared when reached count value 16 */
                LCD_Char_1_WriteControl(LCD_Char_1_CLEAR_DISPLAY); 
            }

            LCD_Char_1_Position(0u, pos++);
            LCD_Char_1_PutChar(ch);         /* Print the received character */

            LCD_Char_1_Position(1u, 0u);
            LCD_Char_1_PrintInt8(count);    /* Prints the count in the LCD */
        }
    }
}


/* [] END OF FILE */
