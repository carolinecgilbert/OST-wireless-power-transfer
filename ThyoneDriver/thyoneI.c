/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */

/**
 * @file
 * @brief Thyone-I driver source file.
 */

#include "thyoneI.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**************************************
 *          Global variables          *
 **************************************/
#define CMDCONFIRMATIONARRAY_LENGTH 3
CMD_Confirmation_t cmdConfirmation_array[CMDCONFIRMATIONARRAY_LENGTH];
uint16_t currentCmdConfirmation;
PacketThyoneI bufferThyone;
ThyoneISettings settings;
int status;
uint16_t readBuffer;


/**************************************
 *          Static variables          *
 **************************************/
#define CMD_ARRAY_SIZE()                                     \
    ((((uint16_t)CMD_Array[CMD_POSITION_LENGTH_LSB] << 0) |  \
      ((uint16_t)CMD_Array[CMD_POSITION_LENGTH_MSB] << 8)) + \
     LENGTH_CMD_OVERHEAD)
char CMD_Array[MAX_CMD_LENGTH];
uint16_t CMD_RX_Array[MAX_CMD_LENGTH] = {};
static uint16_t RxByteCounter = 0;
static int packetReceived = 0;
static uint16_t BytesToReceive = 0;

/**
 * @brief Initialize the ThyoneI interface for serial interface
 *
 * @return true if initialization succeeded,
 *         0 otherwise
 */
int ThyoneI_Init()
{
    int ret = 0;

    /* initialize the pins */
    EALLOW;
    // RESET GPIO13/GPIO5
    GPIO_setPadConfig(GPIO_RESET_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_RESET_PIN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(GPIO_RESET_PIN,1);
    // WAKEUP GPIO9/GPIO10
    GPIO_setPadConfig(GPIO_WAKEUP_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_10_GPIO10);
    GPIO_setDirectionMode(GPIO_WAKEUP_PIN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(GPIO_WAKEUP_PIN,1);
    // MODE GPIOA3/GPIO4
    GPIO_setPadConfig(GPIO_MODE_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_4_GPIO4);
    GPIO_setDirectionMode(GPIO_MODE_PIN, GPIO_DIR_MODE_OUT);
    // command mode
    GPIO_writePin(GPIO_MODE_PIN,0);
    EDIS;

    //
    // setup ThyoneI addresses
    //
    settings.sourceAddress = 0;
    settings.destinationAddress = 0;
    packetReceived = 0;
    RxByteCounter = 0;

    ret = 1;
    return ret;
}


/**
 * @brief  Manually reset ThyoneI module with GPIO13
 * @retval 1 if initialization succeeded,
 *         0 otherwise
 */
void ThyoneI_ManualReset() {
    EALLOW;

    GPIO_writePin(GPIO_RESET_PIN,0);
    DEVICE_DELAY_US(THYONEI_MANUAL_RESET_TIME);
    GPIO_writePin(GPIO_RESET_PIN,1);

    EDIS;

}

/**
 * @brief  Broadcast data
 * @param  self Pointer to the Thyone object.
 * @param  payloadP Pointer to data
 * @param  length Length of the payload
 * @retval 1 if successful 0 in case of failure
 */
int ThyoneI_TransmitBroadcast(uint16_t *payloadP, uint16_t length) {
    int ret = 0;
    if (length <= MAX_PAYLOAD_LENGTH) {
        CMD_Array[CMD_POSITION_STX] = CMD_STX;
        CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_BROADCAST_DATA_REQ;
        CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint16_t)(length >> 0);
        CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint16_t)(length >> 8);

        memcpy(&CMD_Array[CMD_POSITION_DATA], payloadP, length);

        if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
            ThyoneI_sendBytes(CMD_Array, CMD_ARRAY_SIZE());
            //return ThyoneI_waitForReply(ThyoneI_CMD_TXCOMPLETE_RSP,CMD_Status_Success, 1);
            ret = 1;
        }
    }

    return ret;
}

/**
 * @brief  Thyone send data over Thyone serial port
 * @param  data Pointer to the bytes to send
 * @param  dataLength Length of the data
 * @retval none
 */
void ThyoneI_sendBytes(const char *data, int dataLength) {
    int i;
    for (i=0; i<dataLength; i++) {
        HWREGH(SCIA_BASE + SCI_O_TXBUF) = data[i];
    }

}

/**
 * @brief  Thyone receive data over Thyone serial port
 * @param  data Pointer to the buffer to store the received bytes
 * @param  dataLength Length of the data to receive
 * @retval none
 */
void ThyoneI_receiveBytes(uint16_t * const data, int dataLength) {
    uint16_t checksum = 0;
    int i;
    for (i = 0; i < dataLength; i++) {

        // Read the data from the RX buffer
        readBuffer = (uint16_t)HWREGH(SCIA_BASE + SCI_O_RXBUF);

        data[i] = readBuffer;


        // interpret received byte
        CMD_RX_Array[RxByteCounter] = readBuffer;

        switch (RxByteCounter) {
            case 0:
                /* wait for start byte of frame */
                if (CMD_RX_Array[RxByteCounter] == CMD_STX) {
                    BytesToReceive = 0;
                    RxByteCounter = 1;
                }
                break;

            case 1:
                /* CMD */
                RxByteCounter++;
                break;

            case 2:
                /* length field lsb */
                RxByteCounter++;
                BytesToReceive = (uint16_t)(CMD_RX_Array[RxByteCounter - 1]);
                break;

            case 3:
                /* length field msb */
                RxByteCounter++;
                BytesToReceive +=
                    (((uint16_t)CMD_RX_Array[RxByteCounter - 1] << 8) +
                     LENGTH_CMD_OVERHEAD); /* len_msb + len_lsb + crc + sfd +
                                                  cmd */
                break;

            default:
                /* data field */
                RxByteCounter++;
                if (RxByteCounter == BytesToReceive) {
                    /* check CRC */
                    checksum = 0;
                    int i = 0;
                    for (i = 0; i < (BytesToReceive - 1); i++) {
                        checksum ^= CMD_RX_Array[i];
                    }

                    if (checksum == CMD_RX_Array[BytesToReceive - 1]) {
                        /* received frame ok, interpret it now */
                        //HandleRxPacket(CMD_RX_Array);
                    }
                    RxByteCounter = 0;
                    packetReceived = 1;
                    return;
                }
                break;
        }
    }
}

/**
 * @brief  Parse the received packet
 * @param  self Pointer to the Thyone object.
 * @param  data Pointer to the received packet
 * @retval true if successful false in case of failure
 */
void HandleRxPacket(THYONEI *self, uint8_t *pRxBuffer) {
    CMD_Confirmation_t cmdConfirmation;
    cmdConfirmation.cmd = CNFINVALID;
    cmdConfirmation.status = CMD_Status_Invalid;

#if SERIAL_DEBUG
    // uint16_t cmd_length = (uint16_t)(pRxBuffer[CMD_POSITION_LENGTH_LSB] +
    //                                  (pRxBuffer[CMD_POSITION_LENGTH_MSB] <<
    //                                  8));
    // THYONEI_printPacket(self,
    //                     pRxBuffer, cmd_length + LENGTH_CMD_OVERHEAD);
#endif
    switch (pRxBuffer[CMD_POSITION_CMD]) {
        case ThyoneI_CMD_RESET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case ThyoneI_CMD_DATA_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            int length = ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) +
                         ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8);
            memcpy(
                &self->bufferThyone.data[0], &pRxBuffer[CMD_POSITION_DATA + 1],
                length -
                    1); /* First Data byte is status, following bytes response*/
            self->bufferThyone.length = length - 1;
            break;
        }

        case ThyoneI_CMD_SET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GETSTATE_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case ThyoneI_CMD_FACTORYRESET_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_SLEEP_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_LOCAL_READ_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];

            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_READ_CNF: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_Invalid;

            break;
        }

        case ThyoneI_CMD_START_IND: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case ThyoneI_CMD_DATA_IND: {
            uint16_t payload_length =
                ((((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_LSB] << 0) |
                  ((uint16_t)pRxBuffer[CMD_POSITION_LENGTH_MSB] << 8)));
            uint32_t src = (uint32_t)pRxBuffer[CMD_POSITION_DATA + 3];
            src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA + 2];
            src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA + 1];
            src = (src << 8) + (uint32_t)pRxBuffer[CMD_POSITION_DATA];
            self->bufferThyone.sourceAddress = src;
            self->bufferThyone.length = payload_length - 5;
            self->bufferThyone.RSSI =
                (int)(pRxBuffer[CMD_POSITION_DATA + 4] - 255);
            /*Copy payload to the thyone buffer*/
            memcpy(&self->bufferThyone.data[0],
                   pRxBuffer + CMD_POSITION_DATA + 5, payload_length - 5);
            break;
        }

        case ThyoneI_CMD_TXCOMPLETE_RSP: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = pRxBuffer[CMD_POSITION_DATA];
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }

        case ThyoneI_CMD_GPIO_REMOTE_READ_RSP: {
            cmdConfirmation.cmd = pRxBuffer[CMD_POSITION_CMD];
            cmdConfirmation.status = CMD_Status_NoStatus;
            break;
        }
        default: {
            /* invalid*/
            break;
        }
    }

    int i = 0;
    for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
        if (cmdConfirmation_array[i].cmd == CNFINVALID) {
            cmdConfirmation_array[i].cmd = cmdConfirmation.cmd;
            cmdConfirmation_array[i].status = cmdConfirmation.status;
            break;
        }
    }
}

/**
 * @brief  Wait for response from Thyone
 * @param  expectedCmdConfirmation expected confirmation
 * @param  expectedStatus expected status
 * @param  reset_confirmstate reset status
 * @retval true if successful false in case of failure
 */
int ThyoneI_waitForReply(uint16_t expectedCmdConfirmation,CMD_Status_t expectedStatus,int reset_confirmstate) {
    //unsigned long startTime = micros();
    unsigned long interval = 0;
    bufferThyone.length = 0;
    packetReceived = 0;
    RxByteCounter = 0;
    if (reset_confirmstate) {
        int i;
        for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            cmdConfirmation_array[i].cmd = CNFINVALID;
        }
    }

    while (1) {
        //interval = micros() - startTime;
        interval++;
        uint16_t waitDummy[MESSAGE_LENGTH] = {};
        ThyoneI_receiveBytes(waitDummy, MESSAGE_LENGTH);
        if ((interval) >= (TIMEOUT * 1000.0)) /*ms to microseconds*/
        {
            break;
        }
        int i;
        for (i = 0; i < CMDCONFIRMATIONARRAY_LENGTH; i++) {
            if (expectedCmdConfirmation == cmdConfirmation_array[i].cmd) {
                return (cmdConfirmation_array[i].status == expectedStatus);
            }
        }
    }
    return 0;
}


/**
 * @brief  Fill checksum
 * @param  pArray Pointer to the packet
 * @param  dataLength Length of the packet
 * @retval none
 */
int FillChecksum(char *pArray, uint16_t length) {
    int ret = 0;

    if ((length >= LENGTH_CMD_OVERHEAD) && (pArray[0] == CMD_STX)) {
        uint16_t checksum = (uint16_t)0;
        uint16_t payload_length =
            (uint16_t)(pArray[CMD_POSITION_LENGTH_MSB] << 8) +
            pArray[CMD_POSITION_LENGTH_LSB];
        uint16_t i = 0;
        for (i = 0; i < (payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC);
             i++) {
            checksum ^= pArray[i];
        }
        pArray[payload_length + LENGTH_CMD_OVERHEAD_WITHOUT_CRC] = checksum;
        ret = 1;
    }
    return ret;
}

/**
 * @brief  Thyone get parameter
 * @param  userSetting Paramenter to set
 * @param  ResponseP Pointer to the value
 * @param  Response_LengthP Pointer to the length of the value
 * @retval 1 if successful 0 in case of failure
 */
int ThyoneI_Get(ThyoneI_UserSettings_t userSetting,uint16_t *ResponseP, uint16_t *Response_LengthP) {
    int ret = 0;

    /* fill CMD_ARRAY packet */
    CMD_Array[CMD_POSITION_STX] = CMD_STX;
    CMD_Array[CMD_POSITION_CMD] = ThyoneI_CMD_GET_REQ;
    CMD_Array[CMD_POSITION_LENGTH_LSB] = (uint16_t)1;
    CMD_Array[CMD_POSITION_LENGTH_MSB] = (uint16_t)0;
    CMD_Array[CMD_POSITION_DATA] = userSetting;

    if (FillChecksum(CMD_Array, CMD_ARRAY_SIZE())) {
        /* now send CMD_ARRAY */
        ThyoneI_sendBytes(CMD_Array, CMD_ARRAY_SIZE());

        /* wait for cnf */
        if (ThyoneI_waitForReply(ThyoneI_CMD_GET_CNF, CMD_Status_Success,1)) {
            if (bufferThyone.length != 0) {
                memcpy(ResponseP, &bufferThyone.data[0],bufferThyone.length);
                // First Data byte is status,ollowing bytes response
                *Response_LengthP = bufferThyone.length;
                ret = 1;
            }
        }
    }
    return ret;
}


