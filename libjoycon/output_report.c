#include <memory.h>
#include <assert.h>
#include "output_report.h"

#define OUTPUT_REPORT_ID_CMD 0x01
#define OUTPUT_REPORT_ID_RUM 0x10

static uint8_t timer = 0;

#define TIMER ({if(timer==0xFF)timer=0;timer++; })

inline OutputReport_t *createOutputReport(void *buffer)
{
    OutputReport_t *report;
    if (buffer)
    {
        report = (OutputReport_t *)buffer;
    }
    else
    {
        report = (OutputReport_t *)malloc(sizeof(OutputReport_t));
    }
    if (!report)
        return NULL;
    memset(report, 0, sizeof(OutputReport_t));
    report->timer = TIMER;
    return report;
}

inline void releaseOutputReport(OutputReport_t *report)
{
    free(report);
}

OutputReport_t *createCmdOutputReport(void *buffer, uint8_t cmd, SubCmd_t *subcmd, size_t size)
{
    OutputReport_t *report = createOutputReport(buffer);
    if (!report)
        return NULL;
    report->id = OUTPUT_REPORT_CMD;
    report->cmd.cmd = cmd;
    if (subcmd)
    {
        //report->cmd.subcmd = *subcmd;
        assert(size > 0 && size <= sizeof(SubCmd_t));
        memmove(&report->cmd.subcmd, subcmd, size);
    }
    return report;
}
