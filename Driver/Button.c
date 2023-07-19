/*
 * Button.c
 *
 *  Created on: 26.11.2020
 *      Author: Christoph Fehr
 */

#include "Button.h"

uint16_t Status_Button_Interrupt = 0;


// define ISR
__interrupt void xint1ISR(void);    // object decleration
__interrupt void xint2ISR(void);
__interrupt void xint3ISR(void);
__interrupt void xint4ISR(void);
__interrupt void xint5ISR(void);



//***********************************************************************************************************************
void Button_SetGPIO(void){

    // Button_ONE --> GPIO_13
    GPIO_setPadConfig(BUTTON_ONE, GPIO_PIN_TYPE_STD);               // floating
    GPIO_setPinConfig(GPIO_13_GPIO13);                              // MUx als GPIO  --> in pin_map.h
    GPIO_setQualificationMode(BUTTON_ONE, GPIO_QUAL_6SAMPLE);       // switch only if value is equal for 6 cycles
    GPIO_setDirectionMode(BUTTON_ONE, GPIO_DIR_MODE_IN);            // Input-Pin


//    // Button_TWO --> GPIO_13
//    GPIO_setPadConfig(BUTTON_TWO, GPIO_PIN_TYPE_STD);               // floating
//    GPIO_setPinConfig(GPIO_13_GPIO13);                              // MUx als GPIO --> in pin_map.h
//    GPIO_setQualificationMode(BUTTON_TWO, GPIO_QUAL_6SAMPLE);       // switch only if value is equal for 6 cycles
//    GPIO_setDirectionMode(BUTTON_TWO, GPIO_DIR_MODE_IN);            // Input-Pin


//    // Button_THREE --> GPIO_13
//    GPIO_setPadConfig(BUTTON_THREE, GPIO_PIN_TYPE_STD);             // flaoting
//    GPIO_setPinConfig(GPIO_13_GPIO13);                              // MUx als GPIO --> in pin_map.h
//    GPIO_setQualificationMode(BUTTON_THREE, GPIO_QUAL_6SAMPLE);     // switch only if value is equal for 6 cycles
//    GPIO_setDirectionMode(BUTTON_THREE, GPIO_DIR_MODE_IN);          // Input-Pin

}



//***********************************************************************************************************************
uint32_t Button_readPIN(uint32_t ButtonNumber){
        uint32_t data1;
//        uint32_t data2;
//        uint32_t data3;

        switch (ButtonNumber)
        {
        case BUTTON_ONE:
            data1 = GPIO_readPin(BUTTON_ONE);       // 0 = switch is pressed;   (value > 0) = switch is released
            return (~data1 & 0x1U) ;                // 0 = switch is released;      1 = switch is pressed
//        case BUTTON_TWO:
//            data2 = GPIO_readPin(BUTTON_TWO);       // 0 = switch is depressed;   (value > 0) = switch is released
//            return (~data2 & 0x1U);                 // 0 = switch is released;      1 = switch is pressed
//        case BUTTON_THREE:
//            data3 = GPIO_readPin(BUTTON_THREE);     // 0 = switch is depressed;   (value > 0) = switch is released
//            return (~data3 & 0x1U);                 // 0 = switch is released;      1 = switch is pressed
        default:
            return 0x0U;
        }

        /*
        uint32_t PortValue = GPIO_readPortData(GPIO_PORT_A);
        switch (ButtonNumber)
        {
        case BUTTON_ONE: // Button_1
            return PortValue &= (uint32_t)(1 << BUTTON_ONE);        // 0 = switch is depressed;   (value > 0) = switch is released
        case BUTTON_TWO: // Button_2
            return PortValue &= (uint32_t)(1 << BUTTON_TWO);        // 0 = switch is depressed;   (value > 0) = switch is released
        case BUTTON_THREE: // Button_3
            return PortValue &= (uint32_t)(1 << BUTTON_THREE);      // 0 = switch is depressed;   (value > 0) = switch is released
        default:
            return 0xffffffffU;
        }
        */
}



//************************************************************************************************************************
void Button_setInterruptPin(uint32_t pin, GPIO_ExternalIntNum extIntNum){

    XBAR_InputNum input;

        // Check the arguments.
        ASSERT(GPIO_isPinValid(pin));

        // Pick the X-BAR input that corresponds to the requested XINT.
        switch(extIntNum)
        {
            case GPIO_INT_XINT1:
                input = XBAR_INPUT4;
                Interrupt_register(INT_XINT1, &xint1ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT1);
                GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
                break;

            case GPIO_INT_XINT2:
                input = XBAR_INPUT5;
                Interrupt_register(INT_XINT2, &xint2ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT2);
                GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);
                break;

            case GPIO_INT_XINT3:
                input = XBAR_INPUT6;
                Interrupt_register(INT_XINT3, &xint3ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT3);
                GPIO_setInterruptType(GPIO_INT_XINT3, GPIO_INT_TYPE_FALLING_EDGE);
                break;

            case GPIO_INT_XINT4:
                input = XBAR_INPUT13;
                Interrupt_register(INT_XINT4, &xint4ISR);
                GPIO_enableInterrupt(GPIO_INT_XINT4);
                GPIO_setInterruptType(GPIO_INT_XINT4, GPIO_INT_TYPE_BOTH_EDGES);
                break;

            case GPIO_INT_XINT5:
                input = XBAR_INPUT14;
                Interrupt_register(INT_XINT5, &xint5ISR);
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
void Button_EnableInterrupt(GPIO_ExternalIntNum extIntNum){
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
void Button_DisableInterrupt(GPIO_ExternalIntNum extIntNum){
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
uint16_t get_Status_Button_Interrupt(void){
    return Status_Button_Interrupt;
}


//***************************************************************************************************************************
void reset_Status_Button_Interrupt(void){
    Status_Button_Interrupt = 0;
}



//***************************************************************************************************************************
__interrupt void xint1ISR(void)         // xint1ISR --> XINT_1 Interrupt Service Routine
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
__interrupt void xint2ISR(void)         // xint1ISR --> XINT_2 Interrupt Service Routine
{

    // do something
    if (Status_Button_Interrupt == 0){  // will change toggel frequency of LED_ONE
        Status_Button_Interrupt = 1;
    } else {
        Status_Button_Interrupt = 0;
    }

    // clear flag for INT1.5
    Pie_clearInterruptFlag(INT_XINT2);                //HWREG(PIECTRL_BASE  + PIE_O_IFR1) |= PIE_IER1_INTX5;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT1.5 is in group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP1;
}



//***************************************************************************************************************************
__interrupt void xint3ISR(void)         // xint1ISR --> XINT_3 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);


    // clear flag for INT12.1
    Pie_clearInterruptFlag(INT_XINT3);                 //HWREG(PIECTRL_BASE  + PIE_O_IFR12) = PIE_IFR12_INTX1;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT12.1 is in group 12
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP12;
}



//***************************************************************************************************************************
__interrupt void xint4ISR(void)         // xint1ISR --> XINT_4 Interrupt Service Routine
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
__interrupt void xint5ISR(void)         // xint1ISR --> XINT_5 Interrupt Service Routine
{
    // do something
    // LED_toggle(LED_ONE);


    // clear flag for INT12.3
    Pie_clearInterruptFlag(INT_XINT5);                 // HWREG(PIECTRL_BASE  + PIE_O_IFR12) = PIE_IFR12_INTX3;

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT12.3 is in group 12
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);    // HWREGH(PIECTRL_BASE + PIE_O_ACK) = INTERRUPT_ACK_GROUP12;
}




