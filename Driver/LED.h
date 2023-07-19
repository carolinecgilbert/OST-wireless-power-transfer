/*
 * LED.h
 *
 *  Created on: 24.11.2020
 *      Author: cfehr
 */

#ifndef SRC_DRIVER_LED_H_
#define SRC_DRIVER_LED_H_

#include "../TIDriverLib/driverlib.h"
#include "gpio_PortB.h"

/* we use only one LED in this project */
#define LED_ONE     8   // GPIO 8
//#define LED_TWO     8   // GPIO 8
//#define LED_THREE     8   // GPIO 8

void LED_SetGPIO(void);
void LED_Init(void);
void LED_turnOn(uint16_t ledNumber);
void LED_turnOff(uint16_t ledNumber);
void LED_toggle(uint16_t ledNumber);



#endif /* SRC_DRIVER_LED_H_ */
