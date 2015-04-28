/**
 * @file     main.c
 * @version  1.0
 * @date     2015-Feb-26
 *
 * Copyright(C) Redpine Signals 2013
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief main: Beginning of the code
 *
 * @section Description : main function invokes the  system initialization.
 *
 *
 * @section Improvements :
 *
 */
/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#include <WyzBee_kit.h>
#include <WyzBee.h>
#include <WyzBee_gpio.h>

/*
*********************************************************************************************************
*                                         GLOBAL VARIABLES
*********************************************************************************************************
*/

uint32 cnt;
int DELAY_INTERVAL = 200;
#define       EXT_INTR         4E	 				//PORT  P4E

/*
*********************************************************************************************************
*                                             GLOBAL FUNCTIONS
*********************************************************************************************************
*/


void nextColor();

int main()
{
    sys_ticks_init();																				/*Initialize Systick timer*/
    
    nextColor();
    for(;;) {
        delay(500);
        if( !WyzBeeGpio_Get(4E))
            nextColor();        
    }
}

//enum {BLUE, GREEN, RED} COLOR_T;
void nextColor()
{
    static int color = 2;
    
    switch(color) {
        case 2:
            /*Toggling Red LED*/
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_HIGH);
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            color = 3;
            break;

        case 3:
            /*Toggling Green LED*/
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_HIGH);
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            color = 4;
            break;
        
        case 4:
            /*Toggling Blue LED*/
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_HIGH);
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
            color = 2;
            break;
        
        // SHOULD NEVER HAPPEN
        default:
            WyzBeeGpio_Init(GPIO_LED2,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED3,GPIO_OUTPUT,GPIO_LOW);
            WyzBeeGpio_Init(GPIO_LED4,GPIO_OUTPUT,GPIO_LOW);
            delay(2000);
    }
}

