/*
 * gpio_PortB.h
 *
 *  Created on: 17.12.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_GPIO_PORTB_H_
#define SRC_DRIVER_GPIO_PORTB_H_


#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "Pie_ClearFlag.h"
#include "LED.h"
#include <stdio.h>


void PortB_GPIO_setDirectionMode(uint32_t pin, GPIO_Direction pinIO);
void PortB_GPIO_setPadConfig(uint32_t pin, uint32_t pinType);
void PortB_GPIO_setPinConfig(uint32_t pinConfig);
void PortB_GPIO_writePin(uint32_t pin, uint32_t value);                  // 32 < pin < 63



#endif /* SRC_DRIVER_GPIO_PORTB_H_ */
