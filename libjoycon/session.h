#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdlib.h>
#include <stdint.h>
#include "defs.h"
#include "device.h"

typedef int(Recv)(uint8_t *, size_t);
typedef int(Send)(uint8_t *, size_t);

typedef struct Session Session_t;

API_PUBLIC Session_t *Session_create(Device_t *, Recv *, Send *);
API_PUBLIC void Session_release(Session_t *);

#endif //  _SESSION_H_