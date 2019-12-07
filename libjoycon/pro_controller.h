#include "controller.h"

void init(Payload_t *payload)
{
    payload->device_type = PRO_GRIP;
    payload->battery_level = MEDIUM;
    payload->power_type = SWITCH;
}