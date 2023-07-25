

//
// Included Files
//
#include <stdint.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "./TIDriverLib/device.h"
#include "./Driver/LED.h"
#include "./Driver/Button.h"
#include "./Driver/my_ADC.h"
#include "./Driver/my_DAC.h"
#include "./Driver/timer.h"
#include "./Driver/my_GPIO.h"
#include "./Driver/my_sci.h"
#include "./ThyoneDriver/thyoneI.h"


#define unknown_Communication 0
#define Wireless_Communication 1
#define Optical_Fiber_Communication 2


//
// init global Variable
//
uint16_t adcAResult_A2 = 0;    // set ADC variable during start to 0
uint16_t adcBResult_B15 = 0;   // set ADC variable during start to 0



//
// global Variable for Debugging
//
float ADC_A2_Voltage = 0.0;   // V_Primary ADC-Value
float ADC_B15_Voltage = 0.0;  // I_Primary ADC-Value
float V_Primary = 0.0;
float I_Primary = 0.0;
uint16_t V_Secondary = 0;
uint16_t I_Secondary = 0;
int resetThyoneI = 1;   // reset thyoneI only once in while loop
uint16_t payload[2] = {};
int receivedPayload = 0;

//
// Main
//
void main(void){

    //
    // Initialize device clock and peripherals
    //
    Device_init();
    Device_initGPIO();


    //
    // Initialize PIE and clear PIE registers, Disables CPU interrupts.
    //
    Interrupt_initModule();
    Interrupt_initVectorTable(); // PIE vector table
    Interrupt_enablePIE();


    //
    // INIT LED's
    //
    LED_SetGPIO();
    LED_Init();


    //
    // INIT GPIO's
    //
    my_GPIO_INT();


    //
    // INIT Buttons
    //
    Button_SetGPIO();
    /* ISR is in Button.c defined */
    Button_setInterruptPin(BUTTON_ONE, GPIO_INT_XINT2);     // defines the interrupt channel  & vector pointer for ISR
    Button_EnableInterrupt(GPIO_INT_XINT2);                 // enable interrupt for Button_1 (the interrupt will change the toggle frequency of LED_ONE)


    //
    // Init Timer
    //
    InitCPUTimers();
    uint32_t timer_1_period = 100;       // timecycle --> period in uSeconds
    Init_Timer1(timer_1_period);          // trigger for ADC
    uint32_t timer_2_period = 1000;      // timecycle --> period in uSeconds
    Init_Timer2(timer_2_period);          // trigger LED_ONE


    //
    // Init ADC's
    //
    /* ISR is in my_ADC.c defined */
    InitADCs();
    DEVICE_DELAY_US(1000);
    InitADCSOC_A();
    InitADCSOC_B();

    //
    // Configure ThyoneI module
    //
    if (!ThyoneI_Init()) {
        // ThyoneI Init failed
        resetThyoneI = 0;
    }

    //
    // Init SCI
    //
    //init_SCI_A();   // (communication to host PC)
    my_init_SCI_A();  // (communication to host PC using FIFO buffer)
    //init_SCI_B();   // (communication to primary side)


    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT; // enable interrupts      //DINT; // disable interrupts
    ERTM; // enable debug events

    //
    // IDLE loop. Just sit and loop forever
    //
    while(1){

        //
        // Configure ThyoneI Wireless FeatherWing
        //
        if (resetThyoneI==1) {
            ThyoneI_ManualReset();
            resetThyoneI = 0;
        }


#if THYONEI_TRANSMITTER
        //
        // get ADC Values and calculate back the voltage dividers
        //
        //V_Primary = sendBuffer[0];
        //I_Primary = sendBuffer[1];


#else
        //
        // Receive data from transmitting thyoneI
        //
        receivedPayload = 0;
        if (ThyoneI_receiveData(payload)) {
            // Decode payload message
            V_Secondary = payload[0];
            I_Secondary = payload[1];
            receivedPayload = 1;
        }
#endif



    } /* close while loop */
} /* close main function */








