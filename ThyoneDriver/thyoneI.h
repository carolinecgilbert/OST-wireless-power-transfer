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
 * @brief Thyone-I driver header file.
 */

#include "../TIDriverLib/driverlib.h"
#include "../TIDriverLib/inc/hw_ints.h"
#include "../TIDriverLib/inc/hw_pie.h"
#include "../TIDriverLib/inc/hw_types.h"
#include "../TIDriverLib/inc/hw_memmap.h"
#include "../Driver/my_sci.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef THYONEI_H_INCLUDED
#define THYONEI_H_INCLUDED

/* timings */
#define TIMEOUT 500
#define THYONEI_BOOT_DURATION     7500
#define THYONEI_DEFAULT_BAUDRATE  (uint32_t)115200
#define THYONEI_MANUAL_RESET_TIME 1000

/* pins */
#define GPIO_RESET_PIN  5
#define GPIO_WAKEUP_PIN 10
#define GPIO_MODE_PIN   4


/* imported from thyoneI github example*/
#define CMD_WAIT_TIME 500
#define CNFINVALID 255

#define LENGTH_CMD_OVERHEAD (uint16_t)5
#define LENGTH_CMD_OVERHEAD_WITHOUT_CRC (uint16_t)(LENGTH_CMD_OVERHEAD - 1)
#define MAX_PAYLOAD_LENGTH (uint16_t)224
#define MAX_PAYLOAD_LENGTH_MULTICAST_EX (uint16_t)223
#define MAX_PAYLOAD_LENGTH_UNICAST_EX (uint16_t)220

#define MAX_CMD_LENGTH (uint16_t)(MAX_PAYLOAD_LENGTH + LENGTH_CMD_OVERHEAD)

#define CMD_POSITION_STX (uint16_t)0
#define CMD_POSITION_CMD (uint16_t)1
#define CMD_POSITION_LENGTH_LSB (uint16_t)2
#define CMD_POSITION_LENGTH_MSB (uint16_t)3
#define CMD_POSITION_DATA (uint16_t)4

#define CMD_STX 0x02

#define ThyoneI_CMD_TYPE_REQ (uint16_t)(0 << 6)
#define ThyoneI_CMD_TYPE_CNF (uint16_t)(1 << 6)
#define ThyoneI_CMD_TYPE_IND (uint16_t)(2 << 6)
#define ThyoneI_CMD_TYPE_RSP (uint16_t)(3 << 6)

#define ThyoneI_CMD_RESET (uint16_t)0x00
#define ThyoneI_CMD_RESET_REQ (ThyoneI_CMD_RESET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_RESET_CNF (ThyoneI_CMD_RESET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GETSTATE (uint16_t)0x01
#define ThyoneI_CMD_GETSTATE_REQ (ThyoneI_CMD_GETSTATE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GETSTATE_CNF (ThyoneI_CMD_GETSTATE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_SLEEP (uint16_t)0x02
#define ThyoneI_CMD_SLEEP_REQ (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SLEEP_CNF (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_SLEEP_IND (ThyoneI_CMD_SLEEP | ThyoneI_CMD_TYPE_IND)

#define ThyoneI_CMD_START_IND (uint16_t)0x73

#define ThyoneI_CMD_UNICAST_DATA (uint16_t)0x04
#define ThyoneI_CMD_UNICAST_DATA_REQ \
    (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_REQ)

/* Transmissions of any kind will be confirmed and indicated by the same message
 * CMD_DATA_CNF od CMD_DATA_IND*/
#define ThyoneI_CMD_DATA_CNF (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_DATA_IND (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_IND)
#define ThyoneI_CMD_TXCOMPLETE_RSP \
    (ThyoneI_CMD_UNICAST_DATA | ThyoneI_CMD_TYPE_RSP)

#define ThyoneI_CMD_MULTICAST_DATA (uint16_t)0x05
#define ThyoneI_CMD_MULTICAST_DATA_REQ \
    (ThyoneI_CMD_MULTICAST_DATA | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_BROADCAST_DATA (uint16_t)0x06
#define ThyoneI_CMD_BROADCAST_DATA_REQ \
    (ThyoneI_CMD_BROADCAST_DATA | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_UNICAST_DATA_EX (uint16_t)0x07
#define ThyoneI_CMD_UNICAST_DATA_EX_REQ \
    (ThyoneI_CMD_UNICAST_DATA_EX | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_MULTICAST_DATA_EX (uint16_t)0x08
#define ThyoneI_CMD_MULTICAST_DATA_EX_REQ \
    (ThyoneI_CMD_MULTICAST_DATA_EX | ThyoneI_CMD_TYPE_REQ)

#define ThyoneI_CMD_SETCHANNEL (uint16_t)0x09
#define ThyoneI_CMD_SETCHANNEL_REQ \
    (ThyoneI_CMD_SETCHANNEL | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SETCHANNEL_CNF \
    (ThyoneI_CMD_SETCHANNEL | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GET (uint16_t)0x10
#define ThyoneI_CMD_GET_REQ (ThyoneI_CMD_GET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GET_CNF (ThyoneI_CMD_GET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_SET (uint16_t)0x11
#define ThyoneI_CMD_SET_REQ (ThyoneI_CMD_SET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_SET_CNF (ThyoneI_CMD_SET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_FACTORYRESET (uint16_t)0x1C
#define ThyoneI_CMD_FACTORYRESET_REQ \
    (ThyoneI_CMD_FACTORYRESET | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_FACTORYRESET_CNF \
    (ThyoneI_CMD_FACTORYRESET | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG (uint16_t)0x25
#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_REQ \
    (ThyoneI_CMD_GPIO_LOCAL_SETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_SETCONFIG_CNF \
    (ThyoneI_CMD_GPIO_LOCAL_SETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG (uint16_t)0x26
#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_REQ \
    (ThyoneI_CMD_GPIO_LOCAL_GETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_GETCONFIG_CNF \
    (ThyoneI_CMD_GPIO_LOCAL_GETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_WRITE (uint16_t)0x27
#define ThyoneI_CMD_GPIO_LOCAL_WRITE_REQ \
    (ThyoneI_CMD_GPIO_LOCAL_WRITE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_WRITE_CNF \
    (ThyoneI_CMD_GPIO_LOCAL_WRITE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_LOCAL_READ (uint16_t)0x28
#define ThyoneI_CMD_GPIO_LOCAL_READ_REQ \
    (ThyoneI_CMD_GPIO_LOCAL_READ | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_LOCAL_READ_CNF \
    (ThyoneI_CMD_GPIO_LOCAL_READ | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG (uint16_t)0x29
#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_REQ \
    (ThyoneI_CMD_GPIO_REMOTE_SETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_SETCONFIG_CNF \
    (ThyoneI_CMD_GPIO_REMOTE_SETCONFIG | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG (uint16_t)0x2A
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_REQ \
    (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_CNF \
    (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_GPIO_REMOTE_GETCONFIG_RSP \
    (ThyoneI_CMD_GPIO_REMOTE_GETCONFIG | ThyoneI_CMD_TYPE_RSP)

#define ThyoneI_CMD_GPIO_REMOTE_WRITE (uint16_t)0x2B
#define ThyoneI_CMD_GPIO_REMOTE_WRITE_REQ \
    (ThyoneI_CMD_GPIO_REMOTE_WRITE | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_WRITE_CNF \
    (ThyoneI_CMD_GPIO_REMOTE_WRITE | ThyoneI_CMD_TYPE_CNF)

#define ThyoneI_CMD_GPIO_REMOTE_READ (uint16_t)0x2C
#define ThyoneI_CMD_GPIO_REMOTE_READ_REQ \
    (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_REQ)
#define ThyoneI_CMD_GPIO_REMOTE_READ_CNF \
    (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_CNF)
#define ThyoneI_CMD_GPIO_REMOTE_READ_RSP \
    (ThyoneI_CMD_GPIO_REMOTE_READ | ThyoneI_CMD_TYPE_RSP)

/* user settings */
typedef enum ThyoneI_UserSettings_t {
    ThyoneI_USERSETTING_INDEX_SERIAL_NUMBER = (uint16_t)0x01,
    ThyoneI_USERSETTING_INDEX_FW_VERSION = (uint16_t)0x02,
    ThyoneI_USERSETTING_INDEX_UART_CONFIG = (uint16_t)0x04,
    ThyoneI_USERSETTING_INDEX_RF_CHANNEL = (uint16_t)0x07,
    ThyoneI_USERSETTING_INDEX_ENCRYPTION_MODE = (uint16_t)0x08,
    ThyoneI_USERSETTING_INDEX_RF_PROFILE = (uint16_t)0x09,
    ThyoneI_USERSETTING_INDEX_RF_NUM_RETRIES = (uint16_t)0x0A,
    ThyoneI_USERSETTING_INDEX_RF_TX_POWER = (uint16_t)0x0B,
    ThyoneI_USERSETTING_INDEX_RF_RP_NUM_SLOTS = (uint16_t)0x0C,
    ThyoneI_USERSETTING_INDEX_MAC_SOURCE_ADDRESS = (uint16_t)0x10,
    ThyoneI_USERSETTING_INDEX_MAC_DESTINATION_ADDRESS = (uint16_t)0x11,
    ThyoneI_USERSETTING_INDEX_MAC_GROUP_ID = (uint16_t)0x12,
    ThyoneI_USERSETTING_INDEX_MAC_ENCRYPTION_KEY = (uint16_t)0x14,
    ThyoneI_USERSETTING_INDEX_MAC_TLL = (uint16_t)0x15,
    ThyoneI_USERSETTING_INDEX_CCA_MODE = (uint16_t)0x16,
    ThyoneI_USERSETTING_INDEX_CCA_THRESHOLD = (uint16_t)0x17,
    ThyoneI_USERSETTING_INDEX_REMOTE_GPIO_CONFIG = (uint16_t)0x18,
    ThyoneI_USERSETTING_INDEX_MODULE_MODE = (uint16_t)0x20,
} ThyoneI_UserSettings_t;

typedef enum ThyoneI_TXPower_t {
    ThyoneI_TXPower_8 = (int16_t)8,
    ThyoneI_TXPower_4 = (int16_t)4,
    ThyoneI_TXPower_0 = (int16_t)0,
    ThyoneI_TXPower_minus4 = (int16_t)-4,
    ThyoneI_TXPower_minus8 = (int16_t)-8,
    ThyoneI_TXPower_minus12 = (int16_t)-12,
    ThyoneI_TXPower_minus16 = (int16_t)-16,
    ThyoneI_TXPower_minus20 = (int16_t)-20,
    ThyoneI_TXPower_minus40 = (int16_t)-40,
} ThyoneI_TXPower_t;

/* type used to check the response, when a command was sent to the ThyoneI */

typedef enum CMD_Status_t {
    CMD_Status_Success = (uint16_t)0x00,
    CMD_Status_Failed = (uint16_t)0x01,
    CMD_Status_Invalid,
    CMD_Status_Reset,
    CMD_Status_NoStatus,
} CMD_Status_t;

typedef struct {
    uint16_t cmd; /* variable to check if correct CMD has been confirmed */
    CMD_Status_t status; /* variable used to check the response (*_CNF), when a
                        request (*_REQ) was sent to the ThyoneI */
} CMD_Confirmation_t;

typedef struct {
    uint32_t sourceAddress;
    char data[1024];
    int length;
    int RSSI;
} PacketThyoneI;

typedef struct {
    uint32_t sourceAddress;
    uint32_t destinationAddress;
} ThyoneISettings;


/* configure pins */
int ThyoneI_Init();
void ThyoneI_ManualReset();
int ThyoneI_TransmitBroadcast(uint16_t *payloadP, uint16_t length);
int ThyoneI_TransmitMulticast(uint16_t *payloadP, uint16_t length);
int ThyoneI_TransmitUnicast(uint16_t *payloadP,uint16_t length);
void ThyoneI_sendBytes(const char *data, int dataLength);
void ThyoneI_receiveBytes(uint16_t * const data, int dataLength);
void ThyoneI_receiveBytes_wait();
int HandleRxPacket(uint16_t *pRxBuffer);
int ThyoneI_Get(ThyoneI_UserSettings_t userSetting,uint16_t *ResponseP, uint16_t *Response_LengthP);
int ThyoneI_waitForReply(uint16_t expectedCmdConfirmation, CMD_Status_t expectedStatus, int reset_confirmstate);
int FillChecksum(char *pArray, uint16_t length);

#endif // THYONEI_H_INCLUDED

#ifdef __cplusplus
}
#endif

