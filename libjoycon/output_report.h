#ifndef _OUTPUT_REPORT_H_
#define _OUTPUT_REPORT_H_

#include <stdlib.h>
#include <stdint.h>
#include "controller.h"

#define OUTPUT_REPORT_CMD 0x01
#define OUTPUT_REPORT_RUM 0x10
#define OUTPUT_REPORT_PHL 0x11
#define OUTPUT_REPORT_USB 0x80

#pragma pack(1)
// 11~63(53)
typedef struct SubCmd
{
    uint8_t raw[53];
} SubCmd_t;

// Sub-command 0x01: Bluetooth manual pairing
typedef struct SubCmd_01
{
    uint8_t subcmd;       // 11
    MacAddress_t address; // 12~17(6), little endian
    uint8_t fixed[3];     // 18~20(3)
    Alias_t alias;        // 21 ~40(20)
    uint8_t extra[8];     // 41~48(8)
} SubCmd_01_t;

typedef struct SubCmd_02
{
} SubCmd_02_t;

typedef struct SubCmd_03
{
    uint8_t subcmd;
} SubCmd_03_t;

typedef struct SubCmd_04
{
} SubCmd_04_t;

typedef struct SubCmd_05
{
} SubCmd_05_t;

typedef struct SubCmd_06
{
    //uint8_t subcmd;
    HciMode_t mode : 8;
} SubCmd_06_t;

typedef struct SubCmd_07
{
} SubCmd_07_t;

typedef struct SubCmd_08
{
    uint8_t subcmd;
} SubCmd_08_t;

typedef struct SubCmd_10
{
    uint32_t address;
    uint8_t length;
} SubCmd_10_t;

typedef struct SubCmd_11
{
    uint32_t address;
    uint8_t length;
    uint8_t data[0x1d];
} SubCmd_11_t;

typedef struct SubCmd_12
{
    uint32_t address;
    uint8_t length;
} SubCmd_12_t;

typedef struct SubCmd_20
{
} SubCmd_20_t;

typedef struct SubCmd_21
{
    uint8_t subcmd;
    uint8_t raw[38];
} SubCmd_21_t;

typedef struct SubCmd_22
{
    uint8_t subcmd;
} SubCmd_22_t;

typedef struct SubCmd_30
{
    Player_t player : 4;
    PlayerFlash_t flash : 4;
} SubCmd_30_t;

typedef struct SubCmd_38
{
    uint8_t base_duration : 4;   // 0_L : 1~F = 8~175ms, 0=OFF
    uint8_t pattern_count : 4;   // 0_H :
    uint8_t repeat_count : 4;    // 1_L : 0 = forever
    uint8_t start_intensity : 4; //
    HomeLightPattern_t patterns[8];
} SubCmd_38_t;

typedef struct SubCmd_40
{
    uint8_t enable_imu;
} SubCmd_40_t;

typedef struct SubCmd_41
{
    uint8_t gyro_sensitivity;
    uint8_t acc_sensitivity;
    uint8_t gyro_performance;
    uint8_t acc_bandwidth;
} SubCmd_41_t;

typedef struct SubCmd_42
{
    uint8_t address;
    uint8_t operation;
    uint8_t value;
} SubCmd_42_t;

typedef struct SubCmd_43
{
    uint8_t address;
    uint8_t count;
} SubCmd_43_t;

typedef struct SubCmd_48
{
    uint8_t enable_vibrator;
} SubCmd_48_t;

typedef struct SubCmd_50
{
} SubCmd_50_t;

typedef struct RumbleData
{
    uint8_t raw[8];
} RumbleData_t;

typedef struct CmdData
{
    RumbleData_t rumble; // 2~9(8)
    uint8_t cmd;         // 10
    union {              // 11~63(53)
        SubCmd_t subcmd;
        SubCmd_01_t subcmd_01;
        SubCmd_03_t subcmd_03;
        SubCmd_10_t subcmd_10;
        SubCmd_21_t subcmd_21;
        SubCmd_22_t subcmd_22;
        SubCmd_30_t subcmd_30;
        SubCmd_40_t subcmd_40;
        SubCmd_43_t subcmd_43;
        SubCmd_48_t subcmd_48;
    };
} CmdData_t;

typedef struct UsbData
{

} UsbData_t;

typedef struct OutputReport
{
    uint8_t id;    // 0 : 0x01,0x80,0x10,0x11
    uint8_t timer; // 1
    union {        // 2~63(62)
        uint8_t raw[62];
        UsbData_t usb;
        CmdData_t cmd;
        RumbleData_t rumble;
    };
} OutputReport_t;

#pragma pack()

OutputReport_t *createOutputReport(void *);
OutputReport_t *createCmdOutputReport(void *, uint8_t, SubCmd_t *, size_t);
void releaseOutputReport(OutputReport_t *report);

#endif // !_OUTPUT_REPORT_H_
