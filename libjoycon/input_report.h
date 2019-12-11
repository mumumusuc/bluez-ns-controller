#ifndef _INPUT_REPORT_H_
#define _INPUT_REPORT_H_
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "defs.h"
#include "controller.h"

#define STANDARD_REPORT_SIZE 64
#define LARGE_REPORT_SIZE 362
#define STANDARD_PACKET_SIZE 36
#define EXTRA_PACKET_SIZE 313

typedef struct Accelerator
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} Accelerator_t;

typedef struct Gyroscope
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} Gyroscope_t;

// 0x21 : 13~48(36)
typedef struct ReplyData
{
    u_char subcmd_ack;                     //13
    u_char subcmd_id;                      //14
    u_char data[STANDARD_PACKET_SIZE - 2]; //15~49(34)
} ReplyData_t;

// 0x31 : 49~361(313)
typedef struct PeripheralData
{
    union {
        u_char nfc[EXTRA_PACKET_SIZE];
        u_char ir[EXTRA_PACKET_SIZE];
    };
} PeripheralData_t;

// 0x23 : 13~48(36)
typedef struct McuData
{
    u_char raw[STANDARD_PACKET_SIZE];
} McuData_t;

// 0x30,0x31,0x32,0x33 : 13~48(36)
typedef struct ImuData
{
    Accelerator_t acc_0;
    Gyroscope_t gyro_0;
    Accelerator_t acc_1;
    Gyroscope_t gyro_1;
    Accelerator_t acc_2;
    Gyroscope_t gyro_2;
} ImuData_t;

// 48 bytes
typedef struct StandardPacket
{
    u_char timer;            // 1
    Controller_t controller; // 2~11
    u_char vibration;        // 12
    union {                  // 13~48
        ReplyData_t reply;
        McuData_t mcu;
        ImuData_t imu;
    };
} StandardPacket_t;

typedef struct LargePacket
{
    StandardPacket_t standard;
    PeripheralData_t extra;
} LargePacket_t;

typedef struct UsbPacket
{
    u_char raw[48];
} UsbPacket_t;

#pragma pack(1)
typedef struct InputReport
{
    u_char id;
    union {
        UsbPacket_t usb;
        StandardPacket_t standard;
        LargePacket_t large;
    };
} InputReport_t;
#pragma pack()

API_PUBLIC InputReport_t *createInputReport(void *);
API_PUBLIC void releaseInputReport(InputReport_t *);

#endif // !_INPUT_REPORT_H_
