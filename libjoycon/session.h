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

API_PUBLIC Session_t *Session_create(Device_t *, Recv *, Send *);
API_PUBLIC void Session_release(Session_t *);

API_HIDE int Session_active(Session_t *);
API_HIDE int Session_append(Session_t *);
API_PUBLIC int Session_await(Session_t *);
API_HIDE int Session_test(Session_t *, uint8_t);

#define session_send_begin(session)                                            \
    do {                                                                       \
        pthread_rwlock_t *lock = NULL;                                         \
        uint8_t *buffer = NULL;                                                \
        size_t size = 0;                                                       \
        if (session->host.role == CONSOLE) {                                   \
            lock = &session->output_lock;                                      \
            buffer = (uint8_t *)session->output;                               \
            size = sizeof(*session->output);                                   \
        } else if (session->host.role == CONTROLLER) {                         \
            lock = &session->input_lock;                                       \
            buffer = (uint8_t *)session->input;                                \
            size = sizeof(*session->input);                                    \
        }                                                                      \
        pthread_rwlock_rdlock(lock);                                           \
    bzero(buffer, size)

#define session_send_end(session)                                              \
    session->send(buffer, size);                                               \
    pthread_rwlock_unlock(lock);                                               \
    }                                                                          \
    while (0)

#define session_recv_begin(session)                                            \
    do {                                                                       \
        pthread_rwlock_t *lock = NULL;                                         \
        uint8_t *buffer = NULL;                                                \
        size_t size = 0;                                                       \
        if (session->host.role == CONSOLE) {                                   \
            lock = &session->input_lock;                                       \
            buffer = (uint8_t *)session->input;                                \
            size = sizeof(*session->input);                                    \
        } else if (session->host.role == CONTROLLER) {                         \
            lock = &session->output_lock;                                      \
            buffer = (uint8_t *)session->output;                               \
            size = sizeof(*session->output);                                   \
        }                                                                      \
        pthread_rwlock_rdlock(lock);                                           \
    bzero(buffer, size)

#define session_recv_end(session)                                              \
    session->recv(buffer, size);                                               \
    pthread_rwlock_unlock(lock);                                               \
    }                                                                          \
    while (0)

#define session_await_timeout(session, timeout)                                \
    for (Session_await(session); *(timeout);                                   \
         --(*(timeout)), Session_await(session))

#endif //  _SESSION_H_