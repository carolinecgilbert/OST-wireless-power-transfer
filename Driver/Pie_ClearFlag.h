/*
 * Pie_ClearFlag.h
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */

#ifndef SRC_DRIVER_PIE_CLEARFLAG_H_
#define SRC_DRIVER_PIE_CLEARFLAG_H_

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include <stdbool.h>
#include <stdint.h>

    // PIE clear Interrupts Flag
    void Pie_clearInterruptFlag(uint32_t interruptNumber);


#endif /* SRC_DRIVER_PIE_CLEARFLAG_H_ */
