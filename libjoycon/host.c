#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "session.h"
#include "input_report.h"
#include "output_report.h"

#define ASSERT_SESSION assert(session && session->recv && session->send)
#define msleep(t) usleep(1000 * (t))

struct Session
{
    Recv *recv;
    Send *send;
    InputReport_t *input;
    OutputReport_t *output;
};

//static Session_t _session;

static inline int do_recv()
{
    return session->recv(session->input, sizeof(*session->input));
    //return 0;
}

static inline int do_send()
{
    return session->send((uint8_t *)session->output, sizeof(*session->output));
}

static int wait_or_timeout(uint8_t subcmd, size_t timeout)
{
    while (timeout--)
    {
        do_recv();
        if (session->input->standard.reply.subcmd_id == subcmd)
            return 0;
    }
    printf('[%s] timeout(%ld)\n', __func__, timeout);
    return -ETIMEDOUT;
}

int createSession(Recv *recv, Send *send)
{
    int ret = 0;
    session = (Session_t *)malloc(sizeof(Session_t));
    if (!session)
    {
        ret = -ENOMEM;
        goto done;
    }
    session->recv = recv;
    session->send = send;
    ASSERT_SESSION;
    session->output = createOutputReport(NULL);
    if (!session->output)
    {
        ret = -ENOMEM;
        goto free;
    }
    session->input = createInputReport(NULL);
    if (!session->input)
    {
        ret = -ENOMEM;
        goto free;
    }
    return 0;

free:
    destroySession();
done:
    return ret;
}

inline void destroySession()
{
    if (session)
    {
        free(session->output);
        free(session->input);
    }
    free(session);
}

void device_connect()
{
    int ret = 0;
    SubCmd_t subcmd = {};
    createCmdOutputReport(session->output, 0x02, NULL, 0);
    ret = do_send();
    wait_or_timeout(0x02, 10);

    createCmdOutputReport(session->output, 0x08, NULL, 0);
    ret = do_send();
    wait_or_timeout(0x08, 10);

    bzero(&subcmd, sizeof(SubCmd_10_t));
    ((SubCmd_10_t *)&subcmd)->address = 0x00006000;
    ((SubCmd_10_t *)&subcmd)->length = 0x10;
    createCmdOutputReport(session->output, 0x10, &subcmd, sizeof(SubCmd_10_t));
    ret = do_send();
    wait_or_timeout(0x10, 10);

    bzero(&subcmd, sizeof(SubCmd_10_t));
    ((SubCmd_10_t *)&subcmd)->address = 0x00006050;
    ((SubCmd_10_t *)&subcmd)->length = 0x0d;
    createCmdOutputReport(session->output, 0x10, &subcmd, sizeof(SubCmd_10_t));
    ret = do_send();
    wait_or_timeout(0x10, 10);

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
    ret = do_send();
    wait_or_timeout(0x01, 10);

    bzero(&subcmd, sizeof(SubCmd_03_t));
    ((SubCmd_03_t *)&subcmd)->subcmd = 0x30;
    createCmdOutputReport(session->output, 0x03, &subcmd, sizeof(SubCmd_03_t));
    ret = do_send();
    wait_or_timeout(0x03, 10);

    createCmdOutputReport(session->output, 0x04, NULL, 0);
    ret = do_send();
    wait_or_timeout(0x04, 10);

    bzero(&subcmd, sizeof(SubCmd_30_t));
    ((SubCmd_30_t *)&subcmd)->player = PLAYER_1;
    ((SubCmd_30_t *)&subcmd)->flash = PLAYER_FLASH_4;
    createCmdOutputReport(session->output, 0x30, &subcmd, sizeof(SubCmd_30_t));
    ret = do_send();
    wait_or_timeout(0x30, 10);
}

void device_suspend();

void device_disconnect();

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