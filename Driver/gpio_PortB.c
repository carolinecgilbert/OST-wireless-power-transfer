/*
 * gpio_PortB.c
 *
 *  Created on: 17.12.2020
 *      Author: Christoph Fehr
 */

#define GPBDIR 0x4AU
#define GPBPUD 0x4CU
#define GPBINV 0x50U
#define GPBODR 0x52U
#include "gpio_PortB.h"


//*****************************************************************************
void PortB_GPIO_setDirectionMode(uint32_t pin, GPIO_Direction pinIO){
    uint32_t pinMask = (uint32_t)1U << (pin % 32U);

    EALLOW;
    //
    // Set the data direction
    //
    if(pinIO == GPIO_DIR_MODE_OUT)
    {
        //
        // Output
        //
        HWREG(GPIOCTRL_BASE + GPBDIR) |= pinMask;
    }
    else
    {
        //
        // Input
        //
        HWREG(GPIOCTRL_BASE + GPBDIR) &= ~pinMask;
    }

    EDIS;
}



//*****************************************************************************
void PortB_GPIO_setPadConfig(uint32_t pin, uint32_t pinType){
    uint32_t pinMask = (uint32_t)1U << (pin % 32U);

    EALLOW;

    //
    // Enable open drain if necessary
    //
    if((pinType & GPIO_PIN_TYPE_OD) != 0U)
    {
        HWREG(GPIOCTRL_BASE + GPBODR) |= pinMask;
    }
    else
    {
        HWREG(GPIOCTRL_BASE + GPBODR) &= ~pinMask;
    }

    //
    // Enable pull-up if necessary
    //
    if((pinType & GPIO_PIN_TYPE_PULLUP) != 0U)
    {
        HWREG(GPIOCTRL_BASE + GPBPUD) &= ~pinMask;
    }
    else
    {
        HWREG(GPIOCTRL_BASE + GPBPUD) |= pinMask;
    }

    //
    // Invert polarity if necessary
    //
    if((pinType & GPIO_PIN_TYPE_INVERT) != 0U)
    {
        HWREG(GPIOCTRL_BASE + GPBINV) |= pinMask;
    }
    else
    {
        HWREG(GPIOCTRL_BASE + GPBINV) &= ~pinMask;
    }

    EDIS;
}



//*****************************************************************************
void PortB_GPIO_setPinConfig(uint32_t pinConfig){

    /*
    uint32_t muxRegAddr;
    uint32_t pinMask, shiftAmt;

    muxRegAddr = (uint32_t)GPIOCTRL_BASE + (pinConfig >> 16);
    shiftAmt = ((pinConfig >> 8) & (uint32_t)0xFFU);
    pinMask = (uint32_t)0x3U << shiftAmt;

    EALLOW;

    //
    // Clear fields in MUX register first to avoid glitches
    //
    HWREG(muxRegAddr) &= ~pinMask;

    //
    // Write value into GMUX register
    //
    HWREG(muxRegAddr + GPIO_MUX_TO_GMUX) =
        (HWREG(muxRegAddr + GPIO_MUX_TO_GMUX) & ~pinMask) |
        (((pinConfig >> 2) & (uint32_t)0x3U) << shiftAmt);

    //
    // Write value into MUX register
    //
    HWREG(muxRegAddr) |= ((pinConfig & (uint32_t)0x3U) << shiftAmt);
    EDIS;
    */
}



//*****************************************************************************
void PortB_GPIO_writePin(uint32_t pin, uint32_t value){                    // 32 < pin < 63
    uint32_t pinMask = (uint32_t)1U << (pin % 32U);
    uint32_t pinvalue = GPIO_readPortData(GPIO_PORT_B);

    if (0 < value){
        GPIO_writePortData(GPIO_PORT_B, (pinvalue | pinMask));             // write value to GPIO-Pin --> 3.3V
    } else{
        GPIO_writePortData(GPIO_PORT_B, (pinvalue & ~pinMask));            // write value to GPIO-Pin  --> 0V
    }
}
