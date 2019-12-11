#ifndef _OUTPUT_REPORT_H_
#define _OUTPUT_REPORT_H_

#include <stdlib.h>
#include <stdint.h>
#include "defs.h"
#include "controller.h"

#define OUTPUT_REPORT_CMD 0x01
#define OUTPUT_REPORT_RUM 0x10
#define OUTPUT_REPORT_PHL 0x11
#define OUTPUT_REPORT_USB 0x80

#pragma pack(1)
// 11~63(53)
typedef struct
{
    uint8_t raw[53];
} SubCmd_t;

// Sub-command 0x01: Bluetooth manual pairing
typedef struct
{
    uint8_t subcmd;       // 11
    MacAddress_t address; // 12~17(6), little endian
    uint8_t fixed[3];     // 18~20(3)
    Alias_t alias;        // 21 ~40(20)
    uint8_t extra[8];     // 41~48(8)
} SubCmd_01_t;

// Sub-command 0x02: Request device info
typedef struct
{

} SubCmd_02_t;

typedef struct
{
    PollType_t poll_type : 8;
} SubCmd_03_t;

typedef struct
{
} SubCmd_04_t;

typedef struct
{
} SubCmd_05_t;

typedef struct
{
    //uint8_t subcmd;
    HciMode_t mode : 8;
} SubCmd_06_t;

typedef struct
{
} SubCmd_07_t;

typedef struct
{
    uint8_t subcmd;
} SubCmd_08_t;

typedef struct
{
    uint32_t address;
    uint8_t length;
} SubCmd_10_t;

typedef struct
{
    uint32_t address;
    uint8_t length;
    uint8_t data[0x1d];
} SubCmd_11_t;

typedef struct
{
    uint32_t address;
    uint8_t length;
} SubCmd_12_t;

typedef struct
{
} SubCmd_20_t;

typedef struct
{
    uint8_t subcmd;
    uint8_t raw[38];
} SubCmd_21_t;

typedef struct
{
    uint8_t subcmd;
} SubCmd_22_t;

typedef struct
{
    Player_t player : 4;
    PlayerFlash_t flash : 4;
} SubCmd_30_t;

typedef struct
{
    uint8_t raw[23];
} Patterns_t;

#define home_light_pattern(patterns, len)                                                        \
    ({                                                                                           \
        Patterns_t p = {};                                                                       \
        for (int i = 0; i < len; i++)                                                            \
        {                                                                                        \
            HomeLightPattern_t pts = (patterns)[i];                                              \
            int pos = i / 2;                                                                     \
            int res = i % 2;                                                                     \
            p.raw[3 * pos] |= pts.intensity << (4 * (1 - res));                                  \
            p.raw[3 * pos + res + 1] = (uint8_t)((pts.transition << 4) | (pts.duration & 0x0F)); \
        }                                                                                        \
        p;                                                                                       \
    })

typedef struct
{
    uint8_t base_duration : 4;   // 0_L : 1~F = 8~175ms, 0=OFF
    uint8_t pattern_count : 4;   // 0_H :
    uint8_t repeat_count : 4;    // 1_L : 0 = forever
    uint8_t start_intensity : 4; //
    Patterns_t patterns;
} SubCmd_38_t;

typedef struct
{
    uint8_t enable_imu;
} SubCmd_40_t;

typedef struct
{
    GyroSensitivity_t gyro_sensitivity : 8;
    AccSensitivity_t acc_sensitivity : 8;
    GyroPerformance_t gyro_performance : 8;
    AccBandwidth_t acc_bandwidth : 8;
} SubCmd_41_t;

typedef struct
{
    uint8_t address;
    uint8_t operation;
    uint8_t value;
} SubCmd_42_t;

typedef struct
{
    uint8_t address;
    uint8_t count;
} SubCmd_43_t;

typedef struct
{
    uint8_t enable_vibration;
} SubCmd_48_t;

typedef struct
{
} SubCmd_50_t;

typedef struct
{
    uint8_t raw[8];
} RumbleData_t;

typedef struct
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

typedef struct
{

} UsbData_t;

typedef struct
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

API_PUBLIC OutputReport_t *createOutputReport(void *);
API_PUBLIC OutputReport_t *createCmdOutputReport(void *, uint8_t, SubCmd_t *, size_t);
API_PUBLIC void releaseOutputReport(OutputReport_t *report);

#endif // !_OUTPUT_REPORT_H_
