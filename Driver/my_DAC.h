/*
 * my_DAC.h
 *
 *  Created on: 21.12.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_MY_DAC_H_
#define SRC_DRIVER_MY_DAC_H_


#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "../TIDriverLib/inc/hw_adc.h"
#include "LED.h"
#include "timer.h"
#include "Pie_ClearFlag.h"
#include <stdint.h>
#include "float.h"



//
// global Variable Declaration
//
// Gain OPV = 8k6/3k3 = 2.606
extern float gain_OPV;   // Gain OPV (scaled for easier calculation)

    void INIT_my_DAC(void);

    // base is DACA_BASE or DACB_BASE
    void my_DAC_change_Value(uint32_t base, float voltage);

    // base is DACA_BASE or DACB_BASE
    void my_DAC_set_Value(uint32_t base, uint32_t value);   // 0 < value < 4069 Bit


#endif /* SRC_DRIVER_MY_DAC_H_ */
