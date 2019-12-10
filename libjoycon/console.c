#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "defs.h"
#include "session.h"
#include "console.h"
#include "input_report.h"
#include "output_report.h"

#define msleep(t) usleep(1000 * (t))
#define assert_console_session(s)        \
    do                                   \
    {                                    \
        assert(s && s->recv && s->send); \
        assert(s->host.role == CONSOLE); \
    } while (0)

/*extern*/ struct Session
{
    Recv *recv;
    Send *send;
    InputReport_t *input;
    OutputReport_t *output;
    Device_t host;
};

_STATIC_INLINE_ void _dump_hex(uint8_t *data, size_t len)
{
    if (!data)
        return;
    for (int i = 0; i < len; i++)
        fprintf(stdout, "%02x ", data[i]);
    fprintf(stdout, "\n");
}

_STATIC_INLINE_ int _send(Session_t *s)
{
    _FUNC_;
    _dump_hex((uint8_t *)s->output, sizeof(*s->output));
    return s->send((uint8_t *)s->output, sizeof(*s->output));
}

_STATIC_INLINE_ int _recv(Session_t *s)
{
    _FUNC_;
    int ret = s->recv((uint8_t *)s->input, sizeof(*s->input));
    if (ret < 0)
    {
        printf("recv error -> %d\n", ret);
        return ret;
    }
    _dump_hex((uint8_t *)s->input, ret);
    return ret;
}

_STATIC_INLINE_ int _wait_until_timeout(Session_t *s, uint8_t subcmd, size_t timeout)
{
    while (timeout--)
    {
        _recv(s);
        if (s->input->id == 0x21 && s->input->standard.reply.subcmd_id == subcmd)
            return 0;
    }
    printf("[%s] timeout(%ld)\n", __func__, timeout);
    return -ETIMEDOUT;
}

int Console_establish(Session_t *session)
{
    _FUNC_;
    int ret = 0;
    SubCmd_t subcmd = {};
    createCmdOutputReport(session->output, 0x02, NULL, 0);
    ret = _send(session);
    _wait_until_timeout(session, 0x02, 10);

    createCmdOutputReport(session->output, 0x08, NULL, 0);
    ret = _send(session);
    _wait_until_timeout(session, 0x08, 10);

    bzero(&subcmd, sizeof(SubCmd_10_t));
    ((SubCmd_10_t *)&subcmd)->address = 0x00006000;
    ((SubCmd_10_t *)&subcmd)->length = 0x10;
    createCmdOutputReport(session->output, 0x10, &subcmd, sizeof(SubCmd_10_t));
    ret = _send(session);
    _wait_until_timeout(session, 0x10, 10);

    bzero(&subcmd, sizeof(SubCmd_10_t));
    ((SubCmd_10_t *)&subcmd)->address = 0x00006050;
    ((SubCmd_10_t *)&subcmd)->length = 0x0d;
    createCmdOutputReport(session->output, 0x10, &subcmd, sizeof(SubCmd_10_t));
    ret = _send(session);
    _wait_until_timeout(session, 0x10, 10);

    //memset(&subcmd, 0, sizeof(SubCmd_01_t));
    bzero(&subcmd, sizeof(SubCmd_01_t));
    ((SubCmd_01_t *)&subcmd)->subcmd = 0x04;
    ((SubCmd_01_t *)&subcmd)->address = mac_address_le(SwitchConsole.mac_address);
    ((SubCmd_01_t *)&subcmd)->fixed[1] = 0x04;
    ((SubCmd_01_t *)&subcmd)->fixed[2] = 0x3c;
    ((SubCmd_01_t *)&subcmd)->alias = alias(SwitchConsole.name);
    ((SubCmd_01_t *)&subcmd)->extra[0] = 0x68;
    ((SubCmd_01_t *)&subcmd)->extra[2] = 0xc0;
    ((SubCmd_01_t *)&subcmd)->extra[3] = 0x39;
    ((SubCmd_01_t *)&subcmd)->extra[4] = 0x0; // ?
    ((SubCmd_01_t *)&subcmd)->extra[5] = 0x0; // ?
    ((SubCmd_01_t *)&subcmd)->extra[6] = 0x0; // ?
    createCmdOutputReport(session->output, 0x01, &subcmd, sizeof(SubCmd_01_t));
    ret = _send(session);
    _wait_until_timeout(session, 0x01, 10);

    bzero(&subcmd, sizeof(SubCmd_03_t));
    ((SubCmd_03_t *)&subcmd)->subcmd = 0x30;
    createCmdOutputReport(session->output, 0x03, &subcmd, sizeof(SubCmd_03_t));
    ret = _send(session);
    _wait_until_timeout(session, 0x03, 10);

    createCmdOutputReport(session->output, 0x04, NULL, 0);
    ret = _send(session);
    _wait_until_timeout(session, 0x04, 10);

    bzero(&subcmd, sizeof(SubCmd_30_t));
    ((SubCmd_30_t *)&subcmd)->player = PLAYER_1;
    ((SubCmd_30_t *)&subcmd)->flash = PLAYER_FLASH_4;
    createCmdOutputReport(session->output, 0x30, &subcmd, sizeof(SubCmd_30_t));
    ret = _send(session);
    _wait_until_timeout(session, 0x30, 10);

    return 0;
}

void Console_suspend(Session_t *session)
{
    _FUNC_;
    SubCmd_t subcmd = {};
    createCmdOutputReport(session->output, 0x02, NULL, 0);
    _send(session);
    _wait_until_timeout(session, 0x02, 10);
}

int Console_abolish(Session_t *session)
{
    _FUNC_;
    SubCmd_06_t subcmd = {};
    subcmd.mode = REPAIR;
    createCmdOutputReport(session->output, 0x06, (SubCmd_t *)&subcmd, sizeof(SubCmd_06_t));
    _send(session);
    _wait_until_timeout(session, 0x06, 15);
    return 0;
}

int Console_getControllerInfo(Session_t *session)
{
    _FUNC_;
    return 0;
}
int Console_getControllerColor(Session_t *session)
{
    _FUNC_;
    return 0;
}

int Console_setPlayerLight(Session_t *session, Player_t player, PlayerFlash_t flash)
{
    _FUNC_;
    int ret = 0;
    SubCmd_30_t subcmd = {
        .player = player,
        .flash = flash,
    };
    createCmdOutputReport(session->output, 0x30, (SubCmd_t *)&subcmd, sizeof(SubCmd_30_t));
    ret = _send(session);
    ret = _wait_until_timeout(session, 0x30, 10);
    return ret;
}

int Console_setHomeLight(Session_t *session)
{
    _FUNC_;
    int ret = 0;
    SubCmd_38_t subcmd = {
        .base_duration = 0x2,
        .pattern_count = 0x2,
        .repeat_count = 0x3,
        .start_intensity = 0,
        .patterns = {
            {
                .intensity_2 = 0x0,
                .intensity_1 = 0xF,
                .duration_m_1 = 0xF,      // x1
                .fade_duration_m_1 = 0x5, // x15
                .duration_m_2 = 0xF,      // x1
                .fade_duration_m_2 = 0x5,
            },
        },
    };
    createCmdOutputReport(session->output, 0x38, (SubCmd_t *)&subcmd, sizeof(SubCmd_38_t));
    ret = _send(session);
    ret = _wait_until_timeout(session, 0x38, 10);
    return ret;
}
/*
int device_get_color(ControllerColor_t *color)
{
    SubCmd_10_t subcmd = {};
    subcmd.address = 0x00006050;
    subcmd.length = 0x0d;
    createCmdOutputReport(session->output, 0x10, (SubCmd_t *)&subcmd, sizeof(SubCmd_10_t));
    int ret = do_send();
    if (ret < 0)
        return ret;
    ret = wait_or_timeout(0x10, 10);
    if (ret < 0)
        return ret;
    *color = *((ControllerColor_t *)&session->input->standard.reply.data[5]);
    return 0;
}
*/
// 0x02
void device_get_info();

// 0x03
void device_set_mode();

// 0x10
void device_read_mem();

// 0x11
void device_write_mem();

// 0x12
void device_erase_mem();

// 0x30
void device_set_player();

// 0x38
void device_set_homelight();

// 0x40
void device_enable_imu(bool enable);

// 0x41
void device_set_imu_sensitivity();

// 0x43
void device_get_imu_register();

// 0x48
void device_enable_vibration(bool enable);

// 0x50
void device_get_voltage();

//
void device_rumble();