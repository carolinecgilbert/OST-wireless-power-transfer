/*
 * my_sci.h
 *
 *  Created on: 22.02.2021
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_MY_SCI_H_
#define SRC_DRIVER_MY_SCI_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "../TIDriverLib/device.h"
#include "../ThyoneDriver/thyoneI.h"
#include "Pie_ClearFlag.h"
#include "LED.h"
#include "gpio_PortB.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define DEVICE_GPIO_PIN11_SCIB_RX   11U                 // GPIO 11 number for SCIB RX
#define DEVICE_GPIO_CFG_SCIB_RX     GPIO_11_SCIRXDB     // GPIO 11 used for SCIB
#define DEVICE_GPIO_PIN09_SCIB_TX   9U                  // GPIO 9 number for SCIB TX
#define DEVICE_GPIO_CFG_SCIB_TX     GPIO_9_SCITXDB      // GPIO 9 used for SCIB
#define MY_SCIFFTXENA               0x4000              // Bit 14 of STXFF To Enable FIFO
#define MESSAGE_LENGTH              8                   // Bytes in thyoneI command
#define SENDBUFFER_LENGTH           3                   // Bytes in  message within command

//typedef unsigned char uint8_t;

void init_SCI_A(void);
void my_init_SCI_A(void);
void init_SCI_B(void);
void SCI_A_sent_Byte(uint16_t byte);
void SCI_B_sent_Byte(uint16_t byte);



#endif /* SRC_DRIVER_MY_SCI_H_ */
