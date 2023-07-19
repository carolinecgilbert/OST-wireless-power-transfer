/*
 * LED.c
 *
 *  Created on: 24.11.2020
 *      Author: cfehr
 */

#include "LED.h"

void LED_SetGPIO(void)
{
    // LED_ONE --> GPIO_1
    GPIO_setPadConfig(LED_ONE, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(LED_ONE, GPIO_DIR_MODE_OUT);
    GPIO_writePin(LED_ONE, 0);

//    // LED_TWO --> GPIO_33
//    PortB_GPIO_setPadConfig(LED_TWO, GPIO_PIN_TYPE_STD);         // Pin is flaoting
//    PortB_GPIO_setDirectionMode(LED_TWO, GPIO_DIR_MODE_OUT);     // Output-Pin
//    PortB_GPIO_writePin(LED_TWO,0);                              // set to low for startup

}

void LED_Init(void)
{
    LED_turnOff(LED_ONE);
}


void LED_turnOn(uint16_t ledNumber)
{
    switch (ledNumber)
    {
    case LED_ONE:
        GPIO_writePin(LED_ONE, 1);
        break;
//    case LED_TWO:
//        PortB_GPIO_writePin(LED_TWO, 1);
//        break;
//    case LED_THREE:
//        GPIO_writePin(LED_THREE, 1);
//        break;
    default:
        break;
    }
}

void LED_turnOff(uint16_t ledNumber)
{
    switch (ledNumber)
    {
    case LED_ONE:
        GPIO_writePin(LED_ONE, 0);
        break;
//    case LED_TWO:
//        PortB_GPIO_writePin(LED_TWO, 0);
//        break;
//    case LED_THREE:
//        GPIO_writePin(LED_THREE, 0);
//        break;
    default:
        break;
    }
}

void LED_toggle(uint16_t ledNumber)
{
    switch (ledNumber)
    {
    case LED_ONE:
        GPIO_writePin(LED_ONE, !GPIO_readPin(LED_ONE) );
        break;
//    case LED_TWO:
//        PortB_GPIO_writePin(LED_TWO, !GPIO_readPin(LED_TWO) );
//        break;
//    case LED_THREE:
//        GPIO_writePin(LED_THREE, !GPIO_readPin(LED_THREE) );
//        break;
    default:
        break;
    }
}
