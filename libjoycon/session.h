#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "controller.h"

typedef int(Recv)(uint8_t *, size_t);
typedef int(Send)(uint8_t *, size_t);

typedef struct Session Session_t;

Session_t *Session_create(Device_t *, Recv *, Send *);
void Session_release(Session_t *);

#endif //  _SESSION_H_