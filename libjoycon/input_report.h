#include "controller.h"

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

// for report_id 0x21
typedef struct ReplyData
{
    u_char subcmd_ack;
    u_char subcmd_id;
    u_char data[49];
} ReplyData_t;

// for report_id 0x23
typedef struct PeripheralsData
{
    union {
        u_char nfc[37];
        u_char ir[37];
    };
} PeripheralsData_t;

typedef struct NfcData
{

} NfcData_t;

typedef struct IrData
{

} IrData_t;

// for report_id 0x30,0x31,0x32,0x33
typedef struct ImuData
{
    Accelerator_t acc_0;
    Gyroscope_t gyro_0;
    Accelerator_t acc_1;
    Gyroscope_t gyro_1;
    Accelerator_t acc_2;
    Gyroscope_t gyro_2;
} ImuData_t;

typedef struct StandardReport
{
    u_char timer;
    Controller_t payload;
    u_char vibrator;
    union {
        ReplyData_t reply;
        NfcData_t nfc;
        IrData_t ir;
        ImuData_t imu;
    };
} StandardReport_t;

typedef struct LargePacketReport
{
    u_char raw[313];
} LargePacketReport_t;

typedef struct UsbReport
{
    u_char raw[63];
} UsbReport_t;

typedef struct InputReport
{
    u_char id;
    union {
        u_char raw[63];
        UsbReport_t usb;
        StandardReport_t standard;
    };
} InputReport_t;
