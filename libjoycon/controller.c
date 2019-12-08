#include <assert.h>
#include "input_report.h"
#include "pro_controller.h"

#define _STATIC_INLINE_ static inline

_STATIC_INLINE_ Controller_t *createController(void *buffer)
{
    Controller_t *payload;
    if (buffer)
    {
        payload = (Controller_t *)buffer;
    }
    else
    {
        payload = (Controller_t *)malloc(sizeof(Controller_t));
    }
    if (!payload)
        //return -ENOMEM;
        return NULL;
    bzero(payload, sizeof(Controller_t));
    //memset(payload, 0, sizeof(Controller_t));
    return payload;
}

_STATIC_INLINE_ void releaseController(Controller_t *controller)
{
    free(controller);
}

Controller_t *createProController(void *buffer)
{
    Controller_t *payload = createController(buffer);
    if (!payload)
        return NULL;
    payload->device_type = PRO_GRIP;
    payload->battery_level = FULL;
    payload->power_type = SELF;
    return payload;
}

void releaseProController(Controller_t *controller)
{
    releaseController(controller);
}

Controller_t *createJoyCon(void *buffer)
{
    Controller_t *payload = createController(buffer);
    if (!payload)
        return NULL;
    payload->device_type = JoyCon_DUAL;
    payload->battery_level = FULL;
    payload->power_type = SELF;
    return payload;
}

void releaseJoyCon(Controller_t *controller)
{
    releaseController(controller);
}
