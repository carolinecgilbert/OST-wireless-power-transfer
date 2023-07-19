/*
 * timer.c
 *
 *  Created on: 27.11.2020
 *      Author: Christoph Fehr
 */


//#############################################################################
//
// FILE:   timer_ex1_cputimers.c
//
// TITLE:  CPU Timers Example
//
//! \addtogroup driver_example_list
//! <h1> CPU Timers </h1>
//!
//! This example configures CPU Timer0, 1, and 2 and increments
//! a counter each time the timer asserts an interrupt.
//!
//! \b External \b Connections \n
//!  - None
//!
//! \b Watch \b Variables \n
//! - cpuTimer0IntCount
//! - cpuTimer1IntCount
//! - cpuTimer2IntCount
//!
//
//#############################################################################
// $TI Release: F28004x Support Library v1.11.00.00 $
// $Release Date: Sun Oct  4 15:49:15 IST 2020 $
// $Copyright:
// Copyright (C) 2020 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "timer.h"

//
// Globals
//
uint16_t cpuTimer0IntCount = 0;
uint16_t cpuTimer1IntCount = 0;
uint16_t cpuTimer2IntCount = 0;
uint16_t cpuTimer2IntCount2 = 0;
uint16_t ADC_Voltage_Value = 0;
uint16_t ADC_Current_Value = 0;
uint16_t byte_1 = 0x01;
uint16_t byte_2 = 0x02;
uint16_t byte_3 = 0x03;
uint16_t byte_4 = 0x04;
uint16_t byte_5 = 0x05;
uint16_t byte_6 = 0x06;
uint16_t byte_7 = 0x07;
//uint16_t TX_Message[8] = {35,0,0,0};   // in ASCII-CODE = #000
uint16_t TX_Message[MESSAGE_LENGTH] = {};
uint16_t sendBuffer[SENDBUFFER_LENGTH] = {};


//
// Function Prototypes
//
__interrupt void cpuTimer0ISR(void);
__interrupt void cpuTimer1ISR(void);
__interrupt void cpuTimer2ISR(void);



//
// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void InitCPUTimers(void)
{
    // Initialize timer period to maximum
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER2_BASE, 0xFFFFFFFF);

    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER2_BASE, 0);

    // Make sure timer is stopped
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);
    CPUTimer_stopTimer(CPUTIMER2_BASE);

    // Reload all counter register with period value
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER2_BASE);

    // Reset interrupt counter
    cpuTimer0IntCount = 0;
    cpuTimer1IntCount = 0;
    cpuTimer2IntCount = 0;
}



//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in uSeconds. The timer is held in the stopped
// state after configuration.
//
void
configCPUTimer(uint32_t cpuTimer, uint32_t freq, uint32_t period)
{
    uint32_t temp;

    // Initialize timer period:
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    CPUTimer_setPreScaler(cpuTimer, 0);

    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);     // behavior at a brakepoint
    CPUTimer_enableInterrupt(cpuTimer);

    // Resets interrupt counters for the three cpuTimers
    if (cpuTimer == CPUTIMER0_BASE)
    {
        cpuTimer0IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER1_BASE)
    {
        cpuTimer1IntCount = 0;
    }
    else if(cpuTimer == CPUTIMER2_BASE)
    {
        cpuTimer2IntCount = 0;
    }
}




void Init_Timer0(uint32_t period){
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, period);     // 1000000 = 1 second    // DEVICE_SYSCLK_FREQ = 100MHz
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);                  // ISR Vektor
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);                       // enable Interrupts for timer 0
    Interrupt_enable(INT_TIMER0);                                   // enabel timerinterrupt vor cpu
    CPUTimer_startTimer(CPUTIMER0_BASE);                            // start timer 0
}



void Init_Timer1(uint32_t period){
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, period);     // 1000000 = 1 second    // DEVICE_SYSCLK_FREQ = 100MHz
    Interrupt_register(INT_TIMER1, &cpuTimer1ISR);                  // ISR Vektor
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);                       // enable Interrupts for timer 1
    Interrupt_enable(INT_TIMER1);                                   // enabel timerinterrupt vor cpu
    CPUTimer_startTimer(CPUTIMER1_BASE);                            // start timer 1
}



void Init_Timer2(uint32_t period){
    configCPUTimer(CPUTIMER2_BASE, DEVICE_SYSCLK_FREQ, period);     // 1000000 = 1 second    // DEVICE_SYSCLK_FREQ = 100MHz
    Interrupt_register(INT_TIMER2, &cpuTimer2ISR);                  // ISR Vektor
    CPUTimer_enableInterrupt(CPUTIMER2_BASE);                       // enable Interrupts for timer 2
    Interrupt_enable(INT_TIMER2);                                   // enabel timerinterrupt vor cpu
    CPUTimer_startTimer(CPUTIMER2_BASE);                            // start timer 2
    TX_Message[BYTE_1] = byte_1;
    TX_Message[BYTE_2] = byte_2;
    TX_Message[BYTE_3] = byte_3;
    TX_Message[BYTE_4] = byte_4;
    TX_Message[BYTE_5] = byte_5;
    TX_Message[BYTE_6] = byte_6;
}



void set_ADC_AResult(uint16_t value){
    ADC_Voltage_Value = value;
}


void set_ADC_BResult(uint16_t value){
    ADC_Current_Value = value;
}

//
// cpuTimer0ISR - Counter for CpuTimer0
//
__interrupt void
cpuTimer0ISR(void)
{
    //cpuTimer0IntCount++;  // increase internal counter

    // clear flag for INT1.7
    Pie_clearInterruptFlag(INT_TIMER0);
    // Acknowledge this interrupt to receive more interrupts from group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


//
// cpuTimer1ISR - Counter for CpuTimer1
//
__interrupt void
cpuTimer1ISR(void)  // do nothing --> is used to trigger the ADC measurement
{
    /* Interrupt will start ADC measurement */
    // cpuTimer1IntCount++;  // increase internal counter

   /* no flags must be cleared --> Interrupt directly connected to the CPU (doesen't use the PIE) */
}


//
// cpuTimer2ISR - Counter for CpuTimer2
//
__interrupt void
cpuTimer2ISR(void)
{
    cpuTimer2IntCount++;        // increase internal counter for LED_ONE
    cpuTimer2IntCount2++;       // increase internal counter to trigger a UART communication

    if (cpuTimer2IntCount == 1){
        cpuTimer2IntCount = 0;
        //byte_5 = (ADC_Voltage_Value & 0b0000111111110000)>>4;
        //byte_6 = (ADC_Voltage_Value & 0b0000000000001111)<<4;
        //byte_6 = byte_6 | ((ADC_Current_Value & 0b0000111100000000)>>8);
        //byte_7 = (ADC_Current_Value & 0b0000000011111111);
        //byte_7 = ADC_Current_Value;
        //TX_Message[BYTE_5] = byte_5;
        //TX_Message[BYTE_6] = byte_6;
        //TX_Message[BYTE_7] = byte_7;
        //SCI_writeCharArray(SCIA_BASE, (uint16_t*)TX_Message, 4);
    }


    if (cpuTimer2IntCount2 == 1000){   // slow flashing (every second) if idle modus
        cpuTimer2IntCount2 = 0;

        sendBuffer[BYTE_1] = byte_1;
        sendBuffer[BYTE_2] = byte_2;
        sendBuffer[BYTE_3] = byte_3;
        ThyoneI_TransmitBroadcast(sendBuffer, SENDBUFFER_LENGTH);
        LED_toggle(LED_ONE);
    }


//    if (cpuTimer2IntCount == 25 && get_Status_Button_Interrupt() != 0){   // middle fast flashing --> Button does not work
//        cpuTimer2IntCount = 0;
//        LED_toggle(LED_ONE);
//    }

    /* no flags must be cleared --> Interrupt directly connected to the CPU (doesen't use the PIE) */
}



