/*
 * my_GPIO.h
 *
 *  Created on: 17.12.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_MY_GPIO_H_
#define SRC_DRIVER_MY_GPIO_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "Pie_ClearFlag.h"
#include "LED.h"
#include <stdint.h>


#define GPIO_A3   4    // GPIO 4       // output
#define GPIO_13   5    // GPIO 5       // output
#define GPIO_9    10   // GPIO 10      // output




    //*****************************************************************************
    void my_GPIO_INT(void);


    //*****************************************************************************
    uint32_t my_GPIO_readPIN(uint32_t ButtonNumber);


    //*****************************************************************************
    void  my_GPIO_writePIN(uint32_t ButtonNumber, uint32_t vlaue);


    //*****************************************************************************
    // Sets the pin for the specified external interrupt.
    // This function sets which pin triggers the selected external interrupt.
    // The following defines can be used to specify the external interrupt for the
    // \e extIntNum parameter:
    //
    // - \b GPIO_INT_XINT1     --> XBAR_INPUT4
    // - \b GPIO_INT_XINT2     --> XBAR_INPUT5
    // - \b GPIO_INT_XINT3     --> XBAR_INPUT6
    // - \b GPIO_INT_XINT4     --> XBAR_INPUT13
    // - \b GPIO_INT_XINT5     --> XBAR_INPUT14
    //
    // The pin is specified by its numerical value. For example, GPIO34 is
    // specified by passing 34 as \e pin.
    void my_GPIO_setInterruptPin(uint32_t pin, GPIO_ExternalIntNum extIntNum);


    //*****************************************************************************
    void my_GPIO_EnableInterrupt(GPIO_ExternalIntNum extIntNum);


    //*****************************************************************************
    void my_GPIO_DisableInterrupt(GPIO_ExternalIntNum extIntNum);


#endif /* SRC_DRIVER_MY_GPIO_H_ */
