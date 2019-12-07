#ifndef _PRO_CONTROLLER_H_
#define _PRO_CONTROLLER_H_

#include "controller.h"

Controller_t *createProController(void *buffer)
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
    memset(payload, 0, sizeof(Controller_t));
    payload->device_type = PRO_GRIP;
    payload->battery_level = MEDIUM;
    payload->power_type = SWITCH;
    return payload;
}

void releaseProController(Controller_t *controller)
{
    free(controller);
}

#endif // !_PRO_CONTROLLER_H_
