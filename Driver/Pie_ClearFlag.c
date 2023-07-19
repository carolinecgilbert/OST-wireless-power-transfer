/*
 * Pie_ClearFlag.c
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */

#include "Pie_ClearFlag.h"


void Pie_clearInterruptFlag(uint32_t interruptNumber)
{
    bool intsDisabled;
    uint16_t intGroup;
    uint16_t groupMask;
    uint16_t vectID;

    vectID = (uint16_t)(interruptNumber >> 16U);

    // Globally disable interrupts but save status
    intsDisabled = Interrupt_disableMaster();


    if(vectID >= 0x20U)
    {
        intGroup = ((uint16_t)(interruptNumber & 0xFF00U) >> 8U) - 1U;
        groupMask = 1U << intGroup;

        HWREGH(PIECTRL_BASE + PIE_O_IER1 + (intGroup * 2U)) |=
            1U << ((uint16_t)(interruptNumber & 0xFFU) - 1U);

    }

    //
    // INT13, INT14, DLOGINT, & RTOSINT
    //
    else if((vectID >= 0x0DU) && (vectID <= 0x10U))
    {
        // do nothing, because no flag has to be set here!!!
    }
    else
    {
        //
        // Other interrupts
        //
    }

    //
    // Re-enable interrupts if they were enabled
    //
    if(!intsDisabled)
    {
        (void)Interrupt_enableMaster();
    }
}


