/*
 * timer.h
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_TIMER_H_
#define SRC_DRIVER_TIMER_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/device.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "Button.h"
#include "Pie_ClearFlag.h"
#include "LED.h"
#include "my_sci.h"
#include "../ThyoneDriver/thyoneI.h"


// Added from updated timer.h code
#define BYTE_1      0
#define BYTE_2      1
#define BYTE_3      2
#define BYTE_4      3
#define BYTE_5      4
#define BYTE_6      5
#define BYTE_7      6




    void InitCPUTimers(void);

    void Init_Timer0(uint32_t period);   // period in uSeconds

    void Init_Timer1(uint32_t period);   // period in uSeconds

    void Init_Timer2(uint32_t period);   // period in uSeconds

    void set_ADC_AResult(uint16_t value);
    void set_ADC_BResult(uint16_t value);



#endif /* SRC_DRIVER_TIMER_H_ */
