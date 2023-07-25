/*
 * my_sci.c
 *
 *  Created on: 22.02.2021
 *      Author: Christoph Fehr
 */

#include "my_sci.h"


//
// Globals
//
uint16_t counter = 0;
unsigned char *msg;
//uint16_t RX_Message[MESSAGE_LENGTH] = {};   // in ASCII-CODE = 0000
uint16_t RX_Message[MESSAGE_LENGTH] = {};
uint16_t receivedData;
uint16_t ADC_Voltage_Value_2 = 0;
uint16_t ADC_Current_Value_2 = 0;


//
// Function Prototypes
//
__interrupt void sciaTxISR(void);
__interrupt void sciaRxISR(void);
__interrupt void scibTxISR(void);
__interrupt void scibRxISR(void);



//*****************************************************************************
void init_SCI_A(void){

    //GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    //GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_DIR_MODE_IN);
    //GPIO_setPadConfig(DEVICE_GPIO_PIN_SCIRXDA, GPIO_PIN_TYPE_STD);
    //GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_3_SCIA_RX);
    GPIO_setDirectionMode(GPIO_3_SCIA_RX, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(GPIO_3_SCIA_RX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_3_SCIA_RX, GPIO_QUAL_ASYNC);


    //GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    //GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_DIR_MODE_OUT);
    //GPIO_setPadConfig(DEVICE_GPIO_PIN_SCITXDA, GPIO_PIN_TYPE_STD);
    //GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_QUAL_ASYNC);
    GPIO_setPinConfig(GPIO_2_SCIA_TX);
    GPIO_setDirectionMode(GPIO_2_SCIA_TX, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(GPIO_2_SCIA_TX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_2_SCIA_TX, GPIO_QUAL_ASYNC);


    // Map the ISR to the wake interrupt.
    Interrupt_register(INT_SCIA_TX, sciaTxISR);
    Interrupt_register(INT_SCIA_RX, sciaRxISR);

    // Initialize SCIA and its FIFO.
    SCI_performSoftwareReset(SCIA_BASE);


    // Configure SCIA for echoback.
    SCI_setConfig(SCIA_BASE, 25000000, 115200, (SCI_CONFIG_WLEN_8 |
                                                 SCI_CONFIG_STOP_ONE |
                                                 SCI_CONFIG_PAR_NONE));



#ifdef AUTOBAUD
    //
    // Perform an autobaud lock.
    // SCI expects an 'a' or 'A' to lock the baud rate.
    //
    SCI_lockAutobaud(SCIA_BASE);
#endif

    SCI_resetChannels(SCIA_BASE);

    // Configure SCIA for echoback.
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    //SCI_enableModule(SCIA_BASE);
    //SCI_performSoftwareReset(SCIA_BASE);

    // Enable the TXRDY and RXRDY interrupts.
    //SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);

    // Enable FIFO
    SCI_enableFIFO(SCIA_BASE);

    // Set the transmit FIFO level to 7 and the receive FIFO level to 8.
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX7, SCI_FIFO_RX8);


    // Send starting message.
    //msg = "\r\n\n\nHello World!\0";
    //SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 17);
    //msg = "\r\nDebugg Message from the Secondary Side\n\0";
    //SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 42);

    // Clear the SCI interrupts before enabling them.
    //SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);

    // Enable the interrupts in the PIE: Group 9 interrupts 1 & 2.
    Interrupt_enable(INT_SCIA_RX);
    Interrupt_enable(INT_SCIA_TX);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    // Enable the TXRDY and RXRDY interrupts.
    //SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT | SCI_FFRX_RXFFIENA);


}


//*****************************************************************************
// SCI A FIFO INIT FOR RX INT ON 8 BYTES
void my_init_SCI_A(void){

    // init SCI_A RX
    //GPIO_setControllerCore(GPIO_3_SCIA_RX, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_3_SCIA_RX);
    GPIO_setDirectionMode(GPIO_3_SCIA_RX, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(GPIO_3_SCIA_RX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_3_SCIA_RX, GPIO_QUAL_ASYNC);

    // init SCI_A TX
    //GPIO_setControllerCore(GPIO_2_SCIA_TX, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_2_SCIA_TX);
    GPIO_setDirectionMode(GPIO_2_SCIA_TX, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(GPIO_2_SCIA_TX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_2_SCIA_TX, GPIO_QUAL_ASYNC);

    //
    // Disable global interrupts.
    //
    DINT;

    //
    // Initialize interrupt controller and vector table. - completed in main function of main_wireless_v2.c
    //
//    Interrupt_initModule();
//    Interrupt_initVectorTable();
//    IER = 0x0000;
//    IFR = 0x0000;

    //
    // Map the ISR to the wake interrupt.
    //
    Interrupt_register(INT_SCIA_TX, sciaTxISR);
    Interrupt_register(INT_SCIA_RX, sciaRxISR);

    //
    // Initialize SCIA and its FIFO.
    //
    SCI_performSoftwareReset(SCIA_BASE);

    //
    // Configure SCIA for echoback.
    //
    SCI_setConfig(SCIA_BASE, 25000000, 115200, (SCI_CONFIG_WLEN_8 |
                                                 SCI_CONFIG_STOP_ONE |
                                                 SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);

    //
    // Set the transmit FIFO level to 8 and the receive FIFO level to 12.
    // Enable the TXFF and RXFF interrupts.
    //
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX8, SCI_FIFO_RX12);
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);

#ifdef AUTOBAUD
    //
    // Perform an autobaud lock.
    // SCI expects an 'a' or 'A' to lock the baud rate.
    //
    SCI_lockAutobaud(SCIA_BASE);
#endif

    //
    // Send starting message.
    //
//    msg = "\r\n\n\nHello World!\0";
//    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 17);
//    msg = "\r\nYou will enter a character, and the DSP will echo it back!\n\0";
//    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 62);

    //
    // Clear the SCI interrupts before enabling them.
    //
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);

    //
    // Enable the interrupts in the PIE: Group 9 interrupts 1 & 2.
    //
    Interrupt_enable(INT_SCIA_RX);
    Interrupt_enable(INT_SCIA_TX);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    //
    // Enable global interrupts. - completed in main function of main_wireless_v2.c
    //
//    EINT;

}


//*****************************************************************************
void init_SCI_B(void){

    // init SCI_B RX
    GPIO_setPinConfig(GPIO_3_SCIA_RX);
    GPIO_setDirectionMode(GPIO_3_SCIA_RX, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(GPIO_3_SCIA_RX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_3_SCIA_RX, GPIO_QUAL_ASYNC);

    // init SCI_B TX
    GPIO_setPinConfig(GPIO_2_SCIA_TX);
    GPIO_setDirectionMode(GPIO_2_SCIA_TX, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(GPIO_2_SCIA_TX, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_2_SCIA_TX, GPIO_QUAL_ASYNC);


    // Map the ISR to the wake interrupt.
    Interrupt_register(INT_SCIB_TX, scibTxISR);
    Interrupt_register(INT_SCIB_RX, scibRxISR);

    // Initialize SCIA and its FIFO.
    SCI_performSoftwareReset(SCIB_BASE);

    // Configure SCIA for echoback.
    SCI_setConfig(SCIB_BASE, 25000000, 115200, (SCI_CONFIG_WLEN_8 |
                                                 SCI_CONFIG_STOP_ONE |
                                                 SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIB_BASE);


    // Configure SCIA for echoback.
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);
    SCI_enableModule(SCIB_BASE);
    SCI_performSoftwareReset(SCIB_BASE);


    // Enable the TXRDY and RXRDY interrupts.
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);


#ifdef AUTOBAUD
    //
    // Perform an autobaud lock.
    // SCI expects an 'a' or 'A' to lock the baud rate.
    //
    SCI_lockAutobaud(SCIB_BASE);
#endif

    // Clear the SCI interrupts before enabling them.
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXRDY | SCI_INT_RXRDY_BRKDT);


    // Enable the interrupts in the PIE: Group 9 interrupts 1 & 2.
    Interrupt_enable(INT_SCIB_RX);
    Interrupt_enable(INT_SCIB_TX);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}



//*****************************************************************************
void SCI_A_sent_Byte(uint16_t data){
    SCI_writeCharBlockingNonFIFO(SCIA_BASE, data);
}


//*****************************************************************************
void SCI_B_sent_Byte(uint16_t data){
    SCI_writeCharBlockingNonFIFO(SCIB_BASE, data);
}




//*****************************************************************************
// sciaTxISR - Disable the TXRDY interrupt and print message asking for a character.
__interrupt void
sciaTxISR(void)
{

    //
    // Disable the TXRDY interrupt.
    //
    //SCI_disableInterrupt(SCIA_BASE, SCI_INT_TXRDY);
    SCI_disableInterrupt(SCIA_BASE, SCI_INT_TXFF);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);

    // After sending 8 bytes, enable the RXRDY interrupt to start receiving data.
    //SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);

    //
    // Ackowledge the PIE interrupt.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}



//*****************************************************************************
// sciaRxISR - Read the character from the RXBUF and echo it back.
__interrupt void
sciaRxISR(void)
{

    //
    // Enable the TXRDY interrupt again.
    //
    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXFF);

    /* no connection to the USB-Interface --> no reading possible */
    //ThyoneI_receiveBytes(RX_Message, MESSAGE_LENGTH);
    // Wait for ThyoneI response

    // After processing the received data, disable the RXRDY interrupt until the TX buffer sends 8 more bytes.
    //SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);

    // Clear the RXRDY_BRKDT interrupt flag.
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);

    //
    // Acknowledge the PIE interrupt.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}



//*****************************************************************************
// scibTxISR - Disable the TXRDY interrupt and print message asking for a character.
__interrupt void
scibTxISR(void)
{

    //
    // Disable the TXRDY interrupt.
    //
    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXRDY);


    //
    // Ackowledge the PIE interrupt.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}



//*****************************************************************************
// scibRxISR - Read the character from the RXBUF and echo it back.
__interrupt void
scibRxISR(void)
{

    //
    // Enable the TXRDY interrupt again.
    //
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_TXRDY);

    //
    // Read a character from the RXBUF.
    //
    receivedData = SCI_readCharBlockingNonFIFO(SCIB_BASE);

    if (counter == 3){
        RX_Message[counter] = receivedData;
        counter = 4;
    }
    if (counter == 2){
           RX_Message[counter] = receivedData;
           counter = 3;
       }
    if (counter == 1){
        RX_Message[counter] = receivedData;
        counter = 2;
    }
    if (receivedData == 35){
        RX_Message[0] = receivedData;
        counter = 1;
    }


    ADC_Voltage_Value_2 = RX_Message[1]<<4;
    ADC_Voltage_Value_2 = ADC_Voltage_Value_2 | ((RX_Message[2] & 0b0000000011110000)>>4);
    ADC_Current_Value_2 = (RX_Message[2] & 0b0000000000001111)<<8;
    ADC_Current_Value_2 = ADC_Current_Value_2 | (RX_Message[3] & 0b0000000011111111);



    //
    // forwarding to the USB interface for debugging
    //
    //SCI_A_sent_Byte(receivedData);

    //
    // Acknowledge the PIE interrupt.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
