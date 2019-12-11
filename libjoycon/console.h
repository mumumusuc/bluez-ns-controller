#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "controller.h"
#include "session.h"

static Device_t SwitchConsole = {
    .role = CONSOLE,
    .name = "Nintendo Switch",
    .mac_address = "DC:68:EB:15:9A:62",
    .serial_number = "XAJ70017696541",
};

API_PUBLIC int Console_establish(Session_t *);
API_PUBLIC int Console_abolish(Session_t *);
API_PUBLIC int Console_getControllerInfo(Session_t *, ControllerInfo_t *);
API_PUBLIC int Console_getControllerVoltage(Session_t *, uint16_t *);
API_PUBLIC int Console_getControllerColor(Session_t *, ControllerColor_t *);
API_PUBLIC int Console_setPlayerLight(Session_t *, Player_t, PlayerFlash_t);
API_PUBLIC int Console_setHomeLight(Session_t *, uint8_t, uint8_t, uint8_t, size_t, HomeLightPattern_t *);
API_PUBLIC int Console_enableImu(Session_t *, uint8_t);
API_PUBLIC int Console_configImu(Session_t *, GyroSensitivity_t, AccSensitivity_t, GyroPerformance_t, AccBandwidth_t);
API_PUBLIC int Console_readImuRegister(Session_t *);
API_PUBLIC int Console_writeImuRegister(Session_t *);
API_PUBLIC int Console_enableVibration(Session_t *, uint8_t);

// max size = 0x1D
API_HIDE int Console_readMemory(Session_t *, uint32_t, size_t);
API_HIDE int Console_writeMemory(Session_t *, uint32_t, uint8_t *, size_t);
API_HIDE int Console_eraseMemory(Session_t *, uint32_t, size_t);

// poll
API_PUBLIC int Console_testPoll(Session_t *);
API_PUBLIC int Console_stopPoll(Session_t *);
API_HIDE int Console_poll(Session_t *, PollType_t);
#endif //  _CONSOLE_H_
