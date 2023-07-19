/*
 * Button.h
 *
 *  Created on: 26.11.2020
 *      Author: cfehr
 */

#ifndef SRC_DRIVER_BUTTON_H_
#define SRC_DRIVER_BUTTON_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "Pie_ClearFlag.h"
#include "timer.h"
#include "LED.h"
#include <stdio.h>



#define BUTTON_ONE     13   // GPIO 13
//#define BUTTON_TWO    13   // GPIO 13
//#define BUTTON_THREE  13   // GPIO 13




    //*****************************************************************************
    void Button_SetGPIO(void);


    //*****************************************************************************
    uint32_t Button_readPIN(uint32_t ButtonNumber);


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
    void Button_setInterruptPin(uint32_t pin, GPIO_ExternalIntNum extIntNum);


    //*****************************************************************************
    void Button_EnableInterrupt(GPIO_ExternalIntNum extIntNum);


    //*****************************************************************************
    void Button_DisableInterrupt(GPIO_ExternalIntNum extIntNum);


    //*****************************************************************************
    uint16_t get_Status_Button_Interrupt(void);

    //*****************************************************************************
    void reset_Status_Button_Interrupt(void);



#endif /* SRC_DRIVER_BUTTON_H_ */
