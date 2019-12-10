#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include "device.h"

#pragma pack(1)

typedef enum BatteryLevel
{
    EMPTY = 0,    // 0000
    CHARGING = 1, // 0001
    CRITICAL = 2, // 0010
    LOW = 4,      // 0100
    MEDIUM = 6,   // 0110
    FULL = 8,     // 1000
} BatteryLevel_t;

typedef enum DeviceType
{
    PRO_GRIP = 0,
    HANDHELD = 1,     // from yuzu
    JoyCon_DUAL = 2,  // from yuzu
    JoyCon_LEFT = 3,  //
    JoyCon_RIGHT = 4, //
    POKE_BALL = 6,    // from yuzu
} DeviceType_t;

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
    PowerType_t power_type : 1;
    DeviceType_t device_type : 3;
    BatteryLevel_t battery_level : 4;
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

typedef struct HomeLightPattern
{
    uint8_t intensity_2 : 4;
    uint8_t intensity_1 : 4;
    uint8_t duration_m_1 : 4;
    uint8_t fade_duration_m_1 : 4;
    uint8_t duration_m_2 : 4;
    uint8_t fade_duration_m_2 : 4;
} HomeLightPattern_t;

#pragma pack()

typedef enum Player
{
    PLAYER_1 = 0x1,
    PLAYER_2 = 0x2,
    PLAYER_3 = 0x7,
    PLAYER_4 = 0xF,
} Player_t;

typedef enum PlayerFlash
{
    PLAYER_FLASH_1 = 0x1,
    PLAYER_FLASH_2 = 0x2,
    PLAYER_FLASH_3 = 0x7,
    PLAYER_FLASH_4 = 0xF,
} PlayerFlash_t;

typedef enum HciMode
{
    DISCONNECT = 0x0,
    RECONNECT = 0x1,
    REPAIR = 0x2,
    REBOOT = 0x4,
} HciMode_t;

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

#endif // !_CONTROLLER_H_
