#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "session.h"
#include "console.h"
#include "input_report.h"
#include "output_report.h"
#include "pro_controller.h"

static const char *hidraw = "/dev/hidraw0";
static int fd;
static uint8_t test_magic;

void dump_hex(uint8_t *data, size_t len)
{
    if (!data)
        return;
    for (int i = 0; i < len; i++)
        fprintf(stdout, "%02x ", data[i]);
    fprintf(stdout, "\n");
}

int test_recv(uint8_t *buffer, size_t size)
{
    //return printf("recv -> %s , %ld\n", buffer, size);
    int ret = read(fd, buffer, size);
    puts("recv -> ");
    dump_hex(buffer, size);
    return ret;
}

int test_send(uint8_t *buffer, size_t size)
{
    //return printf("send -> %s , %ld\n", buffer, size);
    puts("send -> ");
    dump_hex(buffer, size);

    return write(fd, buffer, size);
}

void python_call(int (*Print)(const char *))
{
    (*Print)("callback from C\n");
}

int test_recv2(uint8_t *buffer, size_t size)
{
    usleep(1000 * 500);
    buffer[0] = test_magic;
    return 1;
    //return sprintf(buffer, "test_recv2\n");
}

int test_send2(uint8_t *buffer, size_t size)
{
    //test_magic = buffer[0];
    return 1;
    //return printf("test_send2 -> %s , %ld\n", buffer, size);
}

static void *test_input_thread(void *arg)
{
    Session_t *session = (Session_t *)arg;
    for (int i = 0; i < 5; i++)
    {
        _func_printf_();
        int ret = Console_test(session, 'p');
        _func_printf_("Console_test return -> %d", ret);
        assert(ret == -ETIMEDOUT);
    }

    pthread_exit(NULL);
    return NULL;
}

int main()
{
    /*
    fd = open(hidraw, O_RDWR);
    if (fd < 0)
    {
        perror("open hid failed\n");
    }
    */
    Session_t *session = Session_create(&SwitchConsole, test_recv2, test_send2);
    test_magic = 'm';
    Console_testPoll(session);
    pthread_t input;
    pthread_create(&input, NULL, test_input_thread, session);
    usleep(500 * 1000);

    for (int i = 0; i < 3; i++)
    {
        _func_printf_();
        int ret = Console_test(session, test_magic);
        _func_printf_("Console_test return -> %d", ret);
        assert(ret >= 0);
        usleep(1000 * 500);
    }
    pthread_join(input, NULL);
    Console_stopPoll(session);
    usleep(500 * 1000);
    Session_release(session);
    /*
    Console_establish(session);
    ControllerInfo_t info = {};
    Console_getControllerInfo(session, &info);
    ControllerColor_t color = {};
    Console_getControllerColor(session, &color);
    for (int i = 0; i <= 0xF; i++)
    {
        Console_setPlayerLight(session, i, 0);
        usleep(1000 * 300);
    }
    Console_setPlayerLight(session, 0, 0xFE);
    Console_abolish(session);
    Session_release(session);
    close(fd);
    */
    /*
    Controller_t *pro = createProController(NULL);
    assert(pro != NULL);
    pro->button.Y = PRESSED;
    pro->button.X = PRESSED;
    pro->button.L = PRESSED;
    pro->button.ZL = PRESSED;
    dump_hex((uint8_t *)pro, sizeof(*pro));
    puts("");

    InputReport_t *report = createInputReport(NULL);
    assert(report != NULL);
    report->id = 0x30;
    report->standard.timer = 0x01;
    report->standard.controller = *pro;
    report->standard.imu.acc_0.X = 0xFF00;
    dump_hex((uint8_t *)report, sizeof(*report));
    puts("");

    SubCmd_30_t subcmd = {
        .player_0 = 1,
        .player_1 = 1,
    };
    OutputReport_t *output = createCmdOutputReport(NULL, 0x30, (SubCmd_t *)&subcmd, sizeof(subcmd));
    /*
    OutputReport_t *output = createOutputReport(NULL);
    output->id = 0x01;
    output->cmd.cmd = 0x30;
    output->cmd.subcmd_30 = subcmd;
    */
    /*
    dump_hex((uint8_t *)output, sizeof(*output));

    releaseOutputReport(output);
    releaseProController(pro);
    releaseInputReport(report);
*/
    return 0;
}
