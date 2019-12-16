#include "console.h"
#include "defs.h"
#include "input_report.h"
#include "output_report.h"
#include "session.h"
#include "task.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define msleep(t) usleep(1000 * (t))
#define assert_console_session(s)                                              \
    do {                                                                       \
        assert(s && s->recv && s->send);                                       \
        assert(s->host.role == CONSOLE);                                       \
    } while (0)

Session_t *Console_createSession(Recv *recv, Send *send) {
    //_func_printf_();
    Session_t *session = Session_create(&SwitchConsole, recv, send);
    if (!session) {
        //_func_printf_("create session error");
        return NULL;
    }
    Session_active(session);
    return session;
}
void Console_releaseSession(Session_t *session) {
    //_func_printf_();
    Session_release(session);
}

int Console_test(Session_t *session, uint8_t code) {
    //_func_printf_();
    int ret = 0;
    session_send_begin(session);
    memset(__session_output(session), code, sizeof(uint8_t));
    ret = session_send_end(session);
    if (ret < 0) {
        _func_printf_("send error(%d)", ret);
        return ret;
    }
    int timeout = 5;
    session_task_begin(session);
    session_task_wait(session, &timeout) {
        InputReport_t *input = (InputReport_t *)__session_input(session);
        if (input->id == code)
            break;
    }
    ret = session_task_end(session);
    _func_printf_("timeout -> %d", timeout);
    return ret;
}

int Console_poll(Session_t *session, PollType_t type) {
    //_func_printf_();
    int ret = 0;
    SubCmd_03_t subcmd = {.poll_type = POLL_STANDARD};
    session_send_begin(session);
    createCmdOutputReport(__session_output(session), 0x03, (SubCmd_t *)&subcmd,
                          sizeof(SubCmd_03_t));
    ret = session_send_end(session);
    if (ret < 0) {
        _func_printf_("send error(%d)", ret);
        return ret;
    }
    int timeout = 5;
    session_task_begin(session);
    session_task_wait(session, &timeout) {
        InputReport_t *input = (InputReport_t *)__session_input(session);
        if (input->id == 0x21 && input->standard.reply.subcmd_id == 0x03)
            break;
    }
    ret = session_task_end(session);
    _func_printf_("timeout -> %d", timeout);
    return ret;
}

int Console_establish(Session_t *session) { return 0; }

int Console_suspend(Session_t *session) { return 0; }

int Console_abolish(Session_t *session) { return 0; }

int Console_getControllerInfo(Session_t *session, ControllerInfo_t *info) {
    return 0;
}

int Console_getControllerVoltage(Session_t *session, uint16_t *voltage) {
    return 0;
}

int Console_getControllerColor(Session_t *session, ControllerColor_t *color) {
    return 0;
}

int Console_setPlayerLight(Session_t *session, Player_t player,
                           PlayerFlash_t flash) {
    //_func_printf_();
    int ret = 0;
    SubCmd_30_t subcmd = {
        .player = player,
        .flash = flash,
    };
    session_send_begin(session);
    createCmdOutputReport(__session_output(session), 0x30, (SubCmd_t *)&subcmd,
                          sizeof(SubCmd_30_t));
    ret = session_send_end(session);
    if (ret < 0) {
        _func_printf_("send error(%d)", ret);
        return ret;
    }
    int timeout = 50;
    session_task_begin(session);
    session_task_wait(session, &timeout) {
        InputReport_t *input = (InputReport_t *)__session_input(session);
        if (input->id == 0x21 && input->standard.reply.subcmd_id == 0x30)
            break;
    }
    ret = session_task_end(session);
    _func_printf_("timeout -> %d", timeout);
    return ret;
}

int Console_setHomeLight(Session_t *session, uint8_t intensity,
                         uint8_t duration, uint8_t repeat, size_t len,
                         HomeLightPattern_t patterns[]) {
    return 0;
}

int Console_enableImu(Session_t *session, uint8_t enable) { return 0; }

int Console_configImu(Session_t *session, GyroSensitivity_t gs,
                      AccSensitivity_t as, GyroPerformance_t gp,
                      AccBandwidth_t ab) {
    return 0;
}
// max = 0x20
int Console_readImuRegister(Session_t *session) { return 0; }

int Console_writeImuRegister(Session_t *session) { return 0; }

int Console_enableVibration(Session_t *session, uint8_t enable) { return 0; }

int Console_getControllerData(Session_t *session, Controller_t *controller) {
    //_func_printf_();
    int ret = 0;
    int timeout = 5;
    session_task_begin(session);
    session_task_wait(session, &timeout) {
        InputReport_t *input = (InputReport_t *)__session_input(session);
        *controller = input->standard.controller;
        break;
    }
    ret = session_task_end(session);
    _func_printf_("timeout -> %d", timeout);
    return ret;
}

int Console_setControllerDataCallback(Session_t *session,
                                      void (*callback)(Controller_t *)) {
    return 0;
}

int Console_getImuData(Session_t *session, ImuData_t *imu) { return 0; }

int Console_setImuDataCallback(Session_t *session,
                               void (*callback)(ImuData_t *)) {
    return 0;
}
