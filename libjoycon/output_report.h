#ifndef _OUTPUT_REPORT_H_
#define _OUTPUT_REPORT_H_

#include <stdlib.h>
#include <stdint.h>

// Sub-command 0x01: Bluetooth manual pairing
typedef struct SubCmd_01
{
    u_char subcmd;
    union {
        u_char bt_address[6]; // little endian
        u_char raw[48];
    };

} SubCmd_01_t;

typedef struct SubCmd_02
{
} SubCmd_02_t;

typedef struct SubCmd_03
{
    u_char subcmd;

} SubCmd_03_t;

typedef struct SubCmd_04
{
} SubCmd_04_t;

typedef struct SubCmd_05
{
} SubCmd_05_t;

typedef struct SubCmd_06
{
    uint8_t subcmd;
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
    u_char length;
} SubCmd_10_t;

typedef struct SubCmd_11
{
    uint32_t address;
    u_char length;
    u_char data[0x1d];
} SubCmd_11_t;

typedef struct SubCmd_12
{
    uint32_t address;
    u_char length;
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
    u_char player_0 : 1;
    u_char player_1 : 1;
    u_char player_2 : 1;
    u_char player_3 : 1;
    u_char flash_0 : 1;
    u_char flash_1 : 1;
    u_char flash_2 : 1;
    u_char flash_3 : 1;
} SubCmd_30_t;

typedef struct SubCmd_38
{
    u_char cycle_duration : 4;
    u_char cycle_number : 4;
    u_char circle_number : 4;
    u_char start_intensity : 4;
    u_char cycle_2_intensity : 4;
    u_char cycle_1_intensity : 4;
    u_char cycle_1_duration_multiplier : 4;
    u_char cycle_1_fade_duration : 4;
    u_char cycle_2_duration_multiplier : 4;
    u_char cycle_2_fade_duration : 4;
} SubCmd_38_t;

typedef struct SubCmd_40
{
    u_char enable_imu;
} SubCmd_40_t;

typedef struct SubCmd_41
{
    u_char gyro_sensitivity;
    u_char acc_sensitivity;
    u_char gyro_performance;
    u_char acc_bandwidth;
} SubCmd_41_t;

typedef struct SubCmd_42
{
    u_char address;
    u_char operation;
    u_char value;
} SubCmd_42_t;

typedef struct SubCmd_43
{
    u_char address;
    u_char count;
} SubCmd_43_t;

typedef struct SubCmd_48
{
    u_char enable_vibrator;
} SubCmd_48_t;

typedef struct SubCmd_50
{
} SubCmd_50_t;

typedef struct CmdData
{
    RumbleData_t rumble;
    u_char cmd;
    union {
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

typedef struct RumbleData
{
    u_char raw[8];
} RumbleData_t;

#define OUTPUT_REPORT_CMD 0x01
#define OUTPUT_REPORT_RUM 0x10
#define OUTPUT_REPORT_PHL 0x11
#define OUTPUT_REPORT_USB 0x80

typedef struct OutputReport
{
    u_char id; // 0x01,0x80,0x10,0x11
    u_char timer;
    union {
        u_char raw[62];
        UsbData_t usb;
        CmdData_t cmd;
        RumbleData_t rumble;
    };
} OutputReport_t;

#endif // !_OUTPUT_REPORT_H_
