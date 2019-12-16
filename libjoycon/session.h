#ifndef _SESSION_H_
#define _SESSION_H_

#include "defs.h"
#include "device.h"
#include "task.h"
#include <stdint.h>
#include <stdlib.h>

typedef int(Recv)(uint8_t *, size_t);
typedef int(Send)(uint8_t *, size_t);

typedef struct Session Session_t;

typedef enum {
    INVALID,
    INACTIVE,
    ACTIVED,
    NO_DATA,
    DATA_INCOMING,
    DATA_READY,
} SessionState;

API_PUBLIC Session_t *Session_create(Device_t *, Recv *, Send *);
API_PUBLIC void Session_release(Session_t *);

API_HIDE int Session_active(Session_t *);
API_HIDE int Session_test(Session_t *, uint8_t); 
API_HIDE int __session_send_begin(Session_t *);
API_HIDE int __session_send_end(Session_t *);
API_HIDE int __session_recv_begin(Session_t *);
API_HIDE int __session_recv_end(Session_t *);
API_HIDE int __session_task_begin(Session_t *);
API_HIDE int __session_task_end(Session_t *);
API_HIDE int __session_task_wait(Session_t *, int);
API_HIDE void *__session_input(Session_t *);
API_HIDE void *__session_output(Session_t *);

#define session_task_begin(session)                                            \
    int _ret = 0;                                                              \
    __session_task_begin((session))

#define session_task_wait(session, timeout)                                    \
    int _timeout = *(timeout);                                                 \
    for (_ret = __session_task_wait((session), 0);                             \
         _timeout-- > 0 && _ret >= 0;                                          \
         *(timeout) = _timeout, _ret = __session_task_wait((session), 1),      \
        _func_printf_("retry %d", _timeout))

#define session_task_end(session)                                              \
    ({                                                                         \
        __session_task_end(session);                                           \
        if (_ret >= 0 && _timeout <= 0) {                                      \
            _ret = -ETIMEDOUT;                                                 \
        };                                                                     \
        _ret;                                                                  \
    })

#define session_send_begin(session)                                            \
    do {                                                                       \
    __session_send_begin(session)

#define session_send_end(session)                                              \
    __session_send_end(session);                                               \
    }                                                                          \
    while (0)

#define session_recv_begin(session)                                            \
    do {                                                                       \
    __session_recv_begin(session)

#define session_recv_end(session)                                              \
    __session_recv_end(session);                                               \
    }                                                                          \
    while (0)

#endif //  _SESSION_H_