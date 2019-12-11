#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include "defs.h"
#include "device.h"

#pragma pack(1)
// struct for program
typedef enum BatteryLevel
{
    EMPTY = 0,    // 0000
    CHARGING = 1, // 0001
    CRITICAL = 2, // 0010
    LOW = 4,      // 0100
    MEDIUM = 6,   // 0110
    FULL = 8,     // 1000
} BatteryLevel_t;

typedef enum ControllerCategory
{
    PRO_GRIP = 0,
    HANDHELD = 1,     // from yuzu
    JoyCon_DUAL = 2,  // from yuzu
    JoyCon_LEFT = 3,  //
    JoyCon_RIGHT = 4, //
    POKE_BALL = 6,    // from yuzu
} Category_t;

typedef enum PowerType
{
    SELF = 0,
    SWITCH = 1,
} PowerType_t;

typedef enum ButtonState
{
    IDLE = 0,
    PRESSED = 1,
} ButtonState_t;

typedef struct Button
{
    // Right byte
    ButtonState_t Y : 1;
    ButtonState_t X : 1;
    ButtonState_t B : 1;
    ButtonState_t A : 1;
    ButtonState_t RSR : 1; // right Joy-Con only
    ButtonState_t RSL : 1; // right Joy-Con only
    ButtonState_t R : 1;
    ButtonState_t ZR : 1;
    // Shared byte
    ButtonState_t MINUS : 1;
    ButtonState_t PLUS : 1;
    ButtonState_t RS : 1;
    ButtonState_t LS : 1;
    ButtonState_t HOME : 1;
    ButtonState_t CAPTURE : 1;
    ButtonState_t NA : 1;
    ButtonState_t CAHRGING_GRIP : 1;
    // Left byte
    ButtonState_t DPAD_DOWN : 1;
    ButtonState_t DPAD_UP : 1;
    ButtonState_t DPAD_RIGHT : 1;
    ButtonState_t DPAD_LEFT : 1;
    ButtonState_t LSR : 1; // left Joy-Con only
    ButtonState_t LSL : 1; // left Joy-Con only
    ButtonState_t L : 1;
    ButtonState_t ZL : 1;
} Button_t;
//static_assert(sizeof(Button_t) == 3);

typedef struct Stick
{
    uint16_t X : 12;
    uint16_t Y : 12;
} Stick_t;
//static_assert(sizeof(Stick_t) == 3);

typedef struct Controller
{
    PowerType_t power : 1;
    Category_t category : 3;
    BatteryLevel_t battery : 4;
    Button_t button;
    Stick_t left_stick;
    Stick_t right_stick;
    void *meta[0]; // point to a device info, but will not increase struct size
} Controller_t;
//static_assert(sizeof(Payload_t) == 10);

typedef struct ControllerColor
{
    uint32_t body_color : 24;
    uint32_t button_color : 24;
    uint32_t left_handler_color : 24;
    uint32_t right_handler_color : 24;
} ControllerColor_t;

typedef struct ControllerInfo
{
    uint8_t firmware[2];
    Category_t category : 8;
    uint8_t _;
    MacAddress_t mac_address;
} ControllerInfo_t;

#pragma pack()

#define firmware(info) (_u16_be(info.firmware))
#define category(info) ({ 0; })
#define battery(voltage) ({                        \
    BatteryLevel level = EMPTY;                    \
    if (voltage >= 0x528 && voltage <= 0x59F)      \
    {                                              \
        level = CRITICAL;                          \
    }                                              \
    else if (voltage >= 0x5A0 && voltage <= 0x5DF) \
    {                                              \
        level = LOW;                               \
    }                                              \
    else if (voltage >= 0x5E0 && voltage <= 0x617) \
    {                                              \
        level = MEDIUM;                            \
    }                                              \
    else if (voltage >= 0x618 && voltage <= 0x690) \
    {                                              \
        level = FULL;                              \
    }                                              \
    level;                                         \
})

typedef enum
{
    PLAYER_1 = 0x1,
    PLAYER_2 = 0x2,
    PLAYER_3 = 0x7,
    PLAYER_4 = 0xF,
} Player_t;

typedef enum
{
    PLAYER_FLASH_1 = 0x1,
    PLAYER_FLASH_2 = 0x2,
    PLAYER_FLASH_3 = 0x7,
    PLAYER_FLASH_4 = 0xF,
} PlayerFlash_t;

typedef enum
{
    DISCONNECT = 0x0,
    RECONNECT = 0x1,
    REPAIR = 0x2,
    REBOOT = 0x4,
} HciMode_t;

typedef struct
{
    uint8_t intensity;
    uint8_t duration;
    uint8_t transition;
} HomeLightPattern_t;

typedef enum
{
    GYRO_SEN_250DPS = 0x0,
    GYRO_SEN_500DPS = 0x1,
    GYRO_SEN_1000DPS = 0x2,
    GYRO_SEN_2000DPS = 0x3,
} GyroSensitivity_t;

#define GYRO_SEN_DEFAULT GYRO_SEN_2000DPS

typedef enum
{
    ACC_SEN_8G = 0x0,
    ACC_SEN_4G = 0x1,
    ACC_SEN_2G = 0x2,
    ACC_SEN_16G = 0x3,
} AccSensitivity_t;

#define ACC_SEN_DEFAULT ACC_SEN_8G

typedef enum
{
    GYRO_PERF_833HZ = 0x0,
    GYRO_PERF_208HZ = 0x1,
} GyroPerformance_t;

#define GYRO_PERF_DEFAULT GYRO_PERF_208HZ

typedef enum
{
    ACC_BW_200HZ = 0x0,
    ACC_BW_100HZ = 0x1,
} AccBandwidth_t;

#define ACC_BW_DEFAULT ACC_BW_100HZ

typedef enum
{
    POLL_NFC_IR_CAM = 0x0,
    POLL_NFC_IR_MCU = 0x1,
    POLL_NFC_IR_DATA = 0x2,
    POLL_IR_CAM = 0x3,
    POLL_STANDARD = 0x30,
    POLL_NFC_IR = 0x31,
    POLL_33 = 0x33, // ?
    POLL_35 = 0x35, // ?
    POLL_SIMPLE_HID = 0x3F,
} PollType_t;
// examples
static Device_t JoyCon_L = {
    .role = CONTROLLER,
    .name = "Joy-Con (L)",
    .mac_address = "",
    .serial_number = "XBW17006642912",
};

static Device_t JoyCon_R = {
    .role = CONTROLLER,
    .name = "Joy-Con (R)",
    .mac_address = "DC:68:EB:30:7F:08",
    .serial_number = "XCW17006887993",
};

static Device_t ProController = {
    .role = CONTROLLER,
    .name = "Pro Controller",
    .mac_address = "",
    .serial_number = "",
};

static HomeLightPattern_t double_blink_pattern[] = {
    {
        .intensity = 0xF,
        .duration = 0x0,
        .transition = 0xF,
    },
    {
        .intensity = 0x0,
        .duration = 0x0,
        .transition = 0xF,
    },
    {
        .intensity = 0xF,
        .duration = 0x0,
        .transition = 0xF,
    },
    {
        .intensity = 0x0,
        .duration = 0x0,
        .transition = 0xF,
    },
    {
        .intensity = 0x0,
        .duration = 0x0,
        .transition = 0xF,
    },
    {
        .intensity = 0x0,
        .duration = 0x0,
        .transition = 0xF,
    },
};

API_PUBLIC HomeLightPattern_t *get_double_blink_pattern();
#endif // !_CONTROLLER_H_
