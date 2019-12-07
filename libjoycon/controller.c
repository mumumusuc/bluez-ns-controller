#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "input_report.h"
#include "pro_controller.h"

InputReport_t *createInputReport(void *buffer)
{
    InputReport_t *report;
    if (buffer)
    {
        report = (InputReport_t *)buffer;
    }
    else
    {
        report = (InputReport_t *)malloc(sizeof(InputReport_t));
    }
    if (!report)
        return NULL;
    memset(report, 0, sizeof(InputReport_t));
}

void releaseInputReport(InputReport_t *report)
{
    free(report);
}

void dump_hex(uint8_t *data, size_t len);

int main()
{
    printf("hello world\n");

    Controller_t *pro = createProController(NULL);
    assert(pro != NULL);
    pro->button.Y = PRESSED;
    pro->button.X = PRESSED;
    pro->button.L = PRESSED;
    pro->button.ZL = PRESSED;
    dump_hex((uint8_t *)pro, sizeof(*pro));

    InputReport_t *report = createInputReport(NULL);
    assert(report != NULL);
    report->id = 0x30;
    report->standard.timer = 0x01;
    report->standard.controller = *pro;
    report->standard.imu.acc_0.X = 0xFF00;
    dump_hex((uint8_t *)report, sizeof(*report));

    releaseProController(pro);
    releaseInputReport(report);
    return 0;
}

void dump_hex(uint8_t *data, size_t len)
{
    if (!data)
        return;
    for (int i = 0; i < len; i++)
        fprintf(stdout, "0x%02x ", data[i]);
    fprintf(stdout, "\n");
}
