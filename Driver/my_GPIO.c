/*
 * my_GPIO.c
 *
 *  Created on: 17.12.2020
 *      Author: Christoph Fehr
 */


#include "my_GPIO.h"


// define ISR
__interrupt void gpio_xint1ISR(void);    // object declaration
__interrupt void gpio_xint2ISR(void);
__interrupt void gpio_xint3ISR(void);
__interrupt void gpio_xint4ISR(void);
__interrupt void gpio_xint5ISR(void);



//***********************************************************************************************************************
void my_GPIO_INT(void){

    // GPIO_A3 --> GPIO_4
    GPIO_setPadConfig(GPIO_A3, GPIO_PIN_TYPE_STD);         // Pin is floating
    GPIO_setDirectionMode(GPIO_A3, GPIO_DIR_MODE_OUT);     // Output-Pin
    GPIO_writePin(GPIO_A3,0);                              // set to low for startup

    // GPIO_13 --> GPIO_5
    GPIO_setPadConfig(GPIO_13, GPIO_PIN_TYPE_STD);         // Pin is floating
    GPIO_setDirectionMode(GPIO_13, GPIO_DIR_MODE_OUT);     // Output-Pin
    GPIO_writePin(GPIO_13,0);                              // set to low for startup

    // GPIO_9 --> GPIO_10
    GPIO_setPadConfig(GPIO_9, GPIO_PIN_TYPE_STD);         // Pin is floating
    GPIO_setDirectionMode(GPIO_9, GPIO_DIR_MODE_OUT);     // Output-Pin
    GPIO_writePin(GPIO_9,0);                              // set to low for startup

}



//***********************************************************************************************************************
void  my_GPIO_writePIN(uint32_t GPIONumber, uint32_t value){
    switch (GPIONumber)
            {
            case GPIO_A3:
                if (0 < value){
                    GPIO_writePin(GPIO_A3, 1);              // write value to GPIO-Pin --> 3.3V
                } else{
                    GPIO_writePin(GPIO_A3, 0);              // write value to GPIO-Pin  --> 0V
                }
            /*
            case GPIO_13:
                if (0 < value){
                    GPIO_writePin(GPIO_13, 1);              // write value to GPIO-Pin --> 3.3V
                } else{
                    GPIO_writePin(GPIO_13, 0);              // write value to GPIO-Pin  --> 0V
                }
            */
            case GPIO_9:
                if (0 < value){
                    GPIO_writePin(GPIO_9, 1);              // write value to GPIO-Pin --> 3.3V
                } else{
                    GPIO_writePin(GPIO_9, 0);              // write value to GPIO-Pin  --> 0V
                }
            default:
                return;                                            // do nothing
            }
}




//************************************************************************************************************************
void my_GPIO_setInterruptPin(uint32_t pin, GPIO_ExternalIntNum extIntNum){

    XBAR_InputNum input;

        // Check the arguments.
        ASSERT(GPIO_isPinValid(pin));

        // Pick the X-BAR input that corresponds to the requested XINT.
        switch(extIntNum)
        {
            case GPIO_INT_XINT1:
                input = XBAR_INPUT4;
                Interrupt_register(INT_XINT1, &gpio_xint1ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT1);
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_BOTH_EDGES);
                //GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_FALLING_EDGE);
                break;

            case GPIO_INT_XINT2:
                input = XBAR_INPUT5;
                Interrupt_register(INT_XINT2, &gpio_xint2ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT2);
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_BOTH_EDGES);
                break;

            case GPIO_INT_XINT3:
                input = XBAR_INPUT6;
                Interrupt_register(INT_XINT3, &gpio_xint3ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT3);
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_BOTH_EDGES);
                break;

            case GPIO_INT_XINT4:
                input = XBAR_INPUT13;
                Interrupt_register(INT_XINT4, &gpio_xint4ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT4);
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_BOTH_EDGES);
                break;

            case GPIO_INT_XINT5:
                input = XBAR_INPUT14;
                Interrupt_register(INT_XINT5, &gpio_xint5ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT5);
                GPIO_setInterruptType(GPIO_INT_XINT5, GPIO_INT_TYPE_BOTH_EDGES);
                break;

            default:
                // Invalid interrupt. Shouldn't happen if enum value is used.
                // XBAR_INPUT1 isn't tied to an XINT, so we'll use it to check for a bad value.
                input = XBAR_INPUT1;
                break;
        }

        if(input != XBAR_INPUT1)
        {
            XBAR_setInputPin(input, (uint16_t) pin);
            // For the signal transfer to the PIE only the input of the XBAR must be set.
        }
}



//***************************************************************************************************************************
void my_GPIO_EnableInterrupt(GPIO_ExternalIntNum extIntNum){
    // Check the arguments.
    ASSERT(GPIO_isPinValid(pin));

    switch(extIntNum)
        {
            case GPIO_INT_XINT1:
                Interrupt_enable(INT_XINT1);
                Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
                break;

            case GPIO_INT_XINT2:
                Interrupt_enable(INT_XINT2);
                Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
                break;

            case GPIO_INT_XINT3:
                Interrupt_enable(INT_XINT3);
                Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
                break;

            case GPIO_INT_XINT4:
                Interrupt_enable(INT_XINT4);
                Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
                break;

            case GPIO_INT_XINT5:
                Interrupt_enable(INT_XINT5);
                Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
                break;

            default:
                // Invalid interrupt
                break;
        }
}



//***************************************************************************************************************************
void my_GPIO_DisableInterrupt(GPIO_ExternalIntNum extIntNum){
    // Check the arguments.
    ASSERT(GPIO_isPinValid(pin));

    switch(extIntNum)
        {
            case GPIO_INT_XINT1:
                Interrupt_disable(INT_XINT1);
                break;

            case GPIO_INT_XINT2:
                Interrupt_disable(INT_XINT2);
                break;

            case GPIO_INT_XINT3:
                Interrupt_disable(INT_XINT3);
                break;

            case GPIO_INT_XINT4:
                Interrupt_disable(INT_XINT4);
                break;

            case GPIO_INT_XINT5:
                Interrupt_disable(INT_XINT5);
                break;

            default:
                // Invalid interrupt
                break;
        }
}



//***************************************************************************************************************************
__interrupt void gpio_xint1ISR(void)         // xint1ISR --> XINT_1 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);


    // clear flag for INT1.4
    Pie_clearInterruptFlag(INT_XINT1);                       //HWREG(PIECTRL_BASE  + PIE_O_IFR1) |= PIE_IER1_INTX4;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT1.4 is in group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP1;
}



//***************************************************************************************************************************
__interrupt void gpio_xint2ISR(void)         // xint1ISR --> XINT_2 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);



    // clear flag for INT1.5
    Pie_clearInterruptFlag(INT_XINT2);                //HWREG(PIECTRL_BASE  + PIE_O_IFR1) |= PIE_IER1_INTX5;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT1.5 is in group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP1;
}



//***************************************************************************************************************************
__interrupt void gpio_xint3ISR(void)         // xint1ISR --> XINT_3 Interrupt Service Routine
{
    // do something
    //  LED_toggle(LED_ONE);



    // clear flag for INT12.1
    Pie_clearInterruptFlag(INT_XINT3);                 //HWREG(PIECTRL_BASE  + PIE_O_IFR12) = PIE_IFR12_INTX1;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT12.1 is in group 12
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP12;
}



//***************************************************************************************************************************
__interrupt void gpio_xint4ISR(void)         // xint1ISR --> XINT_4 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);




    // clear flag for INT12.2
    Pie_clearInterruptFlag(INT_XINT4);                 // HWREG(PIECTRL_BASE  + PIE_O_IFR12) = PIE_IFR12_INTX2;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT12.2 is in group 12
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP12;
}



//***************************************************************************************************************************
__interrupt void gpio_xint5ISR(void)         // xint1ISR --> XINT_5 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);



    // clear flag for INT12.3
    Pie_clearInterruptFlag(INT_XINT5);                 // HWREG(PIECTRL_BASE  + PIE_O_IFR12) = PIE_IFR12_INTX3;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT12.3 is in group 12
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP12;
}
