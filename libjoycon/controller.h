#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#pragma pack(1)

typedef enum BatteryLevel
{
    EMPTY = 0,      // 0000
    u_charGING = 1, // 0001
    CRITICAL = 2,   // 0010
    LOW = 4,        // 0100
    MEDIUM = 6,     // 0110
    FULL = 8,       // 1000
} BatteryLevel_t;

typedef enum DeviceType
{
    PRO_GRIP = 0,
    HANDHELD = 1,    // from yuzu
    JoyCon_DUAL = 2, //from yuzu
    JoyCon_LEFT = 3,
    JoyCon_RIGHT = 4,
    POKE_BALL = 6, // from yuzu
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

typedef struct Accelerator
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} Accelerator_t;

typedef struct Gyro
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} Gyro_t;

typedef struct Payload
{
    PowerType_t power_type : 1;
    DeviceType_t device_type : 3;
    BatteryLevel_t battery_level : 4;
    Button_t button;
    Stick_t left_stick;
    Stick_t right_stick;
} Payload_t;
//static_assert(sizeof(Payload_t) == 10);

// for report_id 0x21
typedef struct StandardData
{
    u_char subcmd_ack;
    u_char subcmd_id;
    u_char data[49];
} StandardData_t;

// for report_id 0x23
typedef struct PeripheralsData
{
    union {
        u_char nfc[37];
        u_char ir[37];
    };

} PeripheralsData_t;

// for report_id 0x30,0x31,0x32,0x33
typedef struct SixAxisData
{
    Accelerator_t acc_0;
    Gyro_t gyro_0;
    Accelerator_t acc_1;
    Gyro_t gyro_1;
    Accelerator_t acc_2;
    Gyro_t gyro_2;
} SixAxisData_t;

typedef struct InputReport
{
    u_char id;
    u_char timer;
    Payload_t payload;
    u_char vibrator;
    union {
        StandardData_t reply;
        PeripheralsData_t peripherals;
        SixAxisData_t six_axis;
    };
} InputReport_t;

#pragma pack()