/*
 * my_ADC.c
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */

#include "my_ADC.h"


//
// internal Parameter
//
float V_Primary_0 = 0.0;
float V_Primary_1 = 0.0;
float V_Primary_2 = 0.0;
float I_Primary_0 = 0.0;
float I_Primary_1 = 0.0;
float I_Primary_2 = 0.0;
uint16_t adcAResult_A2_1 = 0;
uint16_t adcAResult_A2_2 = 0;
uint16_t adcAResult_A2_mean = 0;
uint16_t adcBResult_B15_1 = 0;
uint16_t adcBResult_B15_2 = 0;
uint16_t adcBResult_B15_mean = 0;


//
// gains for ADC back calculation
//
float gain_2_V_Primary = 121;       // gain_V_Primary = (float)(1200+100)/100 = 121
float internal_V_Primary = 0.0;
float gain_2_I_Primary = 20;        // gain_I_Primary = 40/2=20
float internal_I_Primary = 0.0;


//
// define ISR
//
__interrupt void int_ADCA2ISR(void);    // object deceleration
__interrupt void int_ADCB3ISR(void);    // object deceleration


    //***************************************************************************************************************************
    // initADCs - Function to configure and power up ADCs A,B and C.
    void InitADCs(void)
    {
        // Setup VREF as internal
        ADC_setVREF(ADCA_BASE, ADC_REFERENCE_EXTERNAL, ADC_REFERENCE_3_3V);     //use external 3V3 Voltage Reference
        ADC_setVREF(ADCB_BASE, ADC_REFERENCE_EXTERNAL, ADC_REFERENCE_3_3V);     //use external 3V3 Voltage Reference
        ADC_setVREF(ADCC_BASE, ADC_REFERENCE_EXTERNAL, ADC_REFERENCE_3_3V);     //use external 3V3 Voltage Reference

        // Set ADCCLK divider to /4
        ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);   // 100MHz / 4 = 25MHz
        ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);   // 100MHz / 4 = 25MHz
        ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);   // 100MHz / 4 = 25MHz

        // Set pulse positions to late
        ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);  // Interrupt as soon as conversion is finished
        ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);  // Interrupt as soon as conversion is finished
        ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);  // Interrupt as soon as conversion is finished

        // Power up the ADCs
        ADC_enableConverter(ADCA_BASE);
        ADC_enableConverter(ADCB_BASE);
        ADC_enableConverter(ADCC_BASE);

        ADC_setSOCPriority(ADCA_BASE,ADC_PRI_ALL_HIPRI); // All priorities based on SOC number
        ADC_setSOCPriority(ADCB_BASE,ADC_PRI_ALL_HIPRI); // All priorities based on SOC number
        ADC_setSOCPriority(ADCC_BASE,ADC_PRI_ALL_HIPRI); // All priorities based on SOC number
    }


    //***************************************************************************************************************************
    void InitADCSOC_A(void)
    {
        // Configure SOC (start_of_convertion) of ADC_A
        // - SOC_0 will convert pin A2 with a sample window of 25 SYSCLK cycles.
        ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_CPU1_TINT1, ADC_CH_ADCIN2, 25);    // trigger = CPU1 Timer 1

        // Set SOC0 to set the interrupt 3 flag. Enable the interrupt and make sure its flag is cleared.
        ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER0);
        //ADC_enableContinuousMode(ADCA_BASE, ADC_CH_ADCIN3);
        Interrupt_register(INT_ADCA3, &int_ADCA2ISR);
        Interrupt_enable(INT_ADCA3);
        ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER3);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER3);
    }


    //***************************************************************************************************************************
    void InitADCSOC_B(void)
    {
        // Configure SOC (start_of_convertion) of ADC_A
        // - SOC_0 will convert pin A2 with a sample window of 25 SYSCLK cycles.
        ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_CPU1_TINT1, ADC_CH_ADCIN15, 25);    // trigger = CPU1 Timer 1

        // Set SOC0 to set the interrupt 3 flag. Enable the interrupt and make sure its flag is cleared.
        ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER3, ADC_SOC_NUMBER0);
        //ADC_enableContinuousMode(ADCA_BASE, ADC_CH_ADCIN3);
        Interrupt_register(INT_ADCB3, &int_ADCB3ISR);
        Interrupt_enable(INT_ADCB3);
        ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER3);
        Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
        ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER3);
        ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER3);
    }


    //***************************************************************************************************************************
    float get_V_Primary(){
        return internal_V_Primary;
    }


    //***************************************************************************************************************************
    float get_I_Primary(){
        return internal_I_Primary;
    }


    //***************************************************************************************************************************
    float ADC_getResult(uint32_t ADCChannel){

        switch (ADCChannel)
            {
            case ADC_A2:
                return (3.3 * (float)adcAResult_A2 / 4096);
//            case ADC_B6:
//                return (3.3 * (float)adcBResult_B6 / 4096);
//            case ADC_C0:
//                return (3.3 * (float)adcCResult_C0 / 4096);
            default:
                return 0;
        }
     }




//***************************************************************************************************************************
__interrupt void int_ADCA2ISR(void)  // ISR for ADC-Interrupt-3 (Channel A) --> V_Primary
{
    GPIO_writePin(GPIO_9, 1); // set for debugging (ADC conversion is finished)
    GPIO_writePin(GPIO_9, 0);

    adcAResult_A2 = HWREGH(ADCARESULT_BASE + ADC_RESULTx_OFFSET_BASE + ADC_SOC_NUMBER0);
    adcAResult_A2_1 = adcAResult_A2;
    adcAResult_A2_2 = adcAResult_A2_1;
    adcAResult_A2_mean = ((adcAResult_A2 + adcAResult_A2_1 + adcAResult_A2_2)/3);

    set_ADC_AResult(adcAResult_A2_mean);    // set Value for UART Communication
    GPIO_writePin(GPIO_9, 1);               // set for debugging
    internal_V_Primary = (float32_t)((3.3 * (float)adcAResult_A2_mean / 4096) * gain_2_V_Primary );
    GPIO_writePin(GPIO_9, 0);               // ADC-Value conversion to a Voltage is finished


    // clear ADC interrupt and overflow flag
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);                       //clear INT1 flag for ADC-A
    if (1 == ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER3)){       //ADCINT overflow occurred
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER3);           //Clear overflow flag
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER3);                   //Re-clear ADCINT flag
    }

    GPIO_writePin(GPIO_9, 1); // set for debugging --> start clear all Interrupts/Events Flags
    // clear flag for INT10.3
    Pie_clearInterruptFlag(INT_ADCA3);

    // clear flag for Interruptkanal (Acknowledge interrupt group)
    // INT10.3 is in group 10
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);

    // clear EPWM Event Trigger
    //EPWM_clearADCTriggerFlag(EPWM1_BASE, EPWM_SOC_A);
    GPIO_writePin(GPIO_9, 0); // set for debugging --> ISR will be finished
}




//***************************************************************************************************************************
__interrupt void int_ADCB3ISR(void)  // ISR for ADC-Interrupt-3 (Channel B) --> I_Primary
{
    //GPIO_writePin(GPIO_13, 1); // set for debugging (ADC conversion is finished)
    //GPIO_writePin(GPIO_13, 0);

    adcBResult_B15 = HWREGH(ADCBRESULT_BASE + ADC_RESULTx_OFFSET_BASE + ADC_SOC_NUMBER0);
    adcBResult_B15_1 = adcBResult_B15;
    adcBResult_B15_2 = adcBResult_B15_1;
    adcBResult_B15_mean = ((adcBResult_B15 + adcBResult_B15_1 + adcBResult_B15_2)/3);


    set_ADC_BResult(adcBResult_B15_mean);   // set Value for UART Communication
    //GPIO_writePin(GPIO_13, 1);              // set for debugging
    internal_I_Primary = (float32_t)(((3.3 * (float)(adcBResult_B15_mean)/ 4096) - 1.65) * gain_2_I_Primary);
    //GPIO_writePin(GPIO_13, 0);   // ADC-Value conversion to a Voltage is finished


    // clear ADC interrupt and overflow flag
   ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER3);                       //clear INT1 flag for ADC-B
   if (1 == ADC_getInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER3)){       //ADCINT overflow occurred
       ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER3);           //Clear overflow flag
       ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER3);                   //Re-clear ADCINT flag
   }


   //GPIO_writePin(GPIO_13, 1); // set for debugging --> start clear all Interrupts/Events Flags
   // clear flag for INT10.7
   Pie_clearInterruptFlag(INT_ADCB3);

   // clear flag for Interruptkanal (Acknowledge interrupt group)
   // INT10.7 is in group 10
   Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
   //GPIO_writePin(GPIO_13, 0); // set for debugging --> ISR will be finished

}




