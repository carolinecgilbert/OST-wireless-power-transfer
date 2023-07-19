/*
 * my_ADC.h
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_MY_ADC_H_
#define SRC_DRIVER_MY_ADC_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "../TIDriverLib/inc/hw_adc.h"
#include "LED.h"
#include "timer.h"
#include "Pie_ClearFlag.h"
#include "my_GPIO.h"
#include "my_DAC.h"
#include <stdint.h>
#include "float.h"


#define ADC_A2  1    // V_Primary


    //
    // global Variable Declaration
    //
    extern uint16_t adcAResult_A2;     // ADC Result A-2
    extern uint16_t adcBResult_B15;    // ADC Result B-15


    //
    // Methodes
    //
    void InitADCs(void);
    void InitADCSOC_A(void);
    void InitADCSOC_B(void);
    float ADC_getResult(uint32_t ADCChannel);
    float get_V_Primary(void);
    float get_I_Primary(void);



#endif /* SRC_DRIVER_MY_ADC_H_ */
