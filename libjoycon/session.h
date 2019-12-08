#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef int(Recv)(uint8_t *, size_t);
typedef int(Send)(uint8_t *, size_t);

typedef struct Session Session_t;

static Session_t *session;

int createSession(Recv *, Send *);
void destroySession();

void device_connect();
void device_suspend();
void device_disconnect();

#endif //  _SESSION_H_