#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include "session.h"
#include "input_report.h"
#include "output_report.h"
#include "pro_controller.h"

static const char *hidraw = "/dev/hidraw0";
static int fd;

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

int main()
{
    fd = open(hidraw, O_RDWR);
    if (fd < 0)
    {
        perror("open hid failed\n");
    }
    createSession(test_recv, test_send);
    device_connect();
    destroySession();
    close(fd);
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
