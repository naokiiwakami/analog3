/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is the source code for the datasheet example of the Count7 component.
*
* Note:
*
********************************************************************************
* Copyright 2013-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include <project.h>

#define LCD_REFRESH_PERIOD      100u

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  The main function performs the following tasks:
*   1. Starts all the components
*   2. Prints out the count value onto the LCD with a user defined refresh rate
*
* Parameters:
*  None
*
* Return:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
int main()
{
    /* Initialize the display refresh rate */
    uint8 refreshRate = LCD_REFRESH_PERIOD - 1u;
    uint8 count;
    uint8 status;
    
    /* Start components */
    LCD_Start();
    Count7_Start();
    
    LCD_PrintString("STATUS = 0x");
    LCD_Position(1u,0u);
    LCD_PrintString("COUNT =  0x");
    
    for(;;)
    {
        if(0u != refreshRate)
        {
            refreshRate--;
        }
        else
        {
            /* Update count value on LCD */
            refreshRate = LCD_REFRESH_PERIOD - 1u;
            
            /* Stop Count7 prior reading count value */
            Count7_Stop();
            count = Count7_ReadCounter();
            status = Status_Read();
            Count7_Enable();
            
            /* Print out status register value */ 
            LCD_Position(0u, 11u);
            LCD_PrintInt8(status);
            
            /* Print out count value */ 
            LCD_Position(1u, 11u);
            LCD_PrintInt8(count);
        }
        CyDelay(1u);
    }
}

/* [] END OF FILE */

