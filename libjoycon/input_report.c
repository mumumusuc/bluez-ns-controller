#include "input_report.h"

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
    return report;
}

void releaseInputReport(InputReport_t *report)
{
    free(report);
}
