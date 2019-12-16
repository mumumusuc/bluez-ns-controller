#include "session.h"
#include "defs.h"
#include "input_report.h"
#include "output_report.h"
#include "task.h"
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#define msleep(t) usleep(1000 * (t))
#define assert_session(session)                                                \
    assert(session && session->recv && session->send)

struct Session {
    Recv *recv;
    Send *send;
    InputReport_t *input;
    OutputReport_t *output;
    Device_t host;
    SessionState poll_state;
    pthread_t poll_thread;
    int tasks_count;
    int tasks_waiting;
    pthread_barrier_t barrier;
    pthread_mutex_t poll_lock;
    pthread_cond_t poll_cond;
    pthread_cond_t task_cond;
    pthread_rwlock_t input_lock;
    pthread_rwlock_t output_lock;
};

static void poll_cleanup(void *arg) {
    _func_printf_();
    Session_t *session;
    assert(arg);
    session = (Session_t *)arg;
    _func_printf_("lock poll");
    pthread_mutex_lock(&session->poll_lock);
    _func_printf_("inactive poll");
    session->poll_state = INVALID;
    if (session->tasks_count) {
        pthread_cond_broadcast(&session->task_cond);
        // pthread_cond_wait(&session->poll_cond, &session->poll_lock);
    }
    pthread_mutex_unlock(&session->poll_lock);
    pthread_rwlock_unlock(&session->input_lock);
    pthread_rwlock_unlock(&session->output_lock);
    // session->poll_thread = 0;
    _func_printf_("exit poll thread, clean up ...");
}

static void *poll(void *arg) {
    _func_printf_();
    int ret = 0;
    Session_t *session;
    assert(arg);
    assert(0 == pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
    assert(0 == pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));
    session = (Session_t *)arg;
    _func_printf_("enter poll thread ...");
    pthread_cleanup_push(poll_cleanup, (void *)session);
    // wait for active
    pthread_mutex_lock(&session->poll_lock);
    session->poll_state == INACTIVE;
    pthread_barrier_wait(&session->barrier);
    session->poll_state == ACTIVED;
    pthread_mutex_unlock(&session->poll_lock);
    // main loop
    while (1) {
        // 1. lock input
        // 2. recv
        // 3. lock task
        // 4. check tasks
        // 5. unlock task
        // true->
        //      6_1. wake up all thread(task_waiting = x)
        //      6_2. wait for all thread(task_waiting = 0)
        // false->
        //      4_1. sleep
        // 5. unlock input
        pthread_rwlock_wrlock(&session->input_lock);
        session->poll_state = DATA_INCOMING;
        uint8_t *buffer = (uint8_t *)session->input;
        size_t size = sizeof(*session->input);
        bzero(buffer, size);
        ret = session->recv(buffer, size);
        if (ret <= 0) {
            session->poll_state = NO_DATA;
        } else {
            session->poll_state = DATA_READY;
        }
        pthread_rwlock_unlock(&session->input_lock);
        clock_t begin = clock();
        pthread_rwlock_rdlock(&session->input_lock);
        pthread_mutex_lock(&session->poll_lock);
        int tasks_total = session->tasks_count;
        //_func_printf_("tasks_total = %d", tasks_total);
        session->tasks_waiting = 0;
        if (tasks_total > 0) {
            session->tasks_waiting = tasks_total;
            pthread_cond_broadcast(&session->task_cond);
            while (session->tasks_waiting > 0) {
                //_func_printf_("waiting = %d", session->tasks_waiting);
                // unlock poll_lock
                pthread_cond_wait(&session->poll_cond, &session->poll_lock);
                _func_printf_("wakeup, waiting = %d", session->tasks_waiting);
            }
        }
        pthread_mutex_unlock(&session->poll_lock);
        pthread_rwlock_unlock(&session->input_lock);
        clock_t end = clock();
        _func_printf_("handle tasks use %ld us",
                      ((end - begin) * 1000000) / CLOCKS_PER_SEC);
        if (!tasks_total) {
            msleep(15);
        }
    };
    pthread_cleanup_pop(0);
free:
    poll_cleanup((void *)session);
    pthread_exit(NULL);
    _func_printf_("pthread_exit\n");
    return NULL;
}

static inline size_t select_send(Session_t *session, pthread_rwlock_t **lock,
                                 uint8_t **buffer) {
    size_t size = 0;
    if (session->host.role == CONSOLE) {
        *lock = &session->output_lock;
        *buffer = (uint8_t *)session->output;
        size = sizeof(*session->output);
    } else if (session->host.role == CONTROLLER) {
        *lock = &session->input_lock;
        *buffer = (uint8_t *)session->input;
        size = sizeof(*session->input);
    }
    return size;
}

static inline size_t select_recv(Session_t *session, pthread_rwlock_t **lock,
                                 uint8_t **buffer) {
    size_t size = 0;
    if (session->host.role == CONSOLE) {
        *lock = &session->input_lock;
        *buffer = (uint8_t *)session->input;
        size = sizeof(*session->input);
    } else if (session->host.role == CONTROLLER) {
        *lock = &session->output_lock;
        *buffer = (uint8_t *)session->output;
        size = sizeof(*session->output);
    }
    return size;
}

inline void *__session_input(Session_t *session) { return session->input; }
inline void *__session_output(Session_t *session) { return session->output; }

inline int __session_send_begin(Session_t *session) {
    pthread_rwlock_t *lock;
    uint8_t *buffer;
    size_t size = select_send(session, &lock, &buffer);
    pthread_rwlock_rdlock(lock);
    bzero(buffer, size);
    return 0;
}

inline int __session_send_end(Session_t *session) {
    pthread_rwlock_t *lock;
    uint8_t *buffer;
    size_t size = select_send(session, &lock, &buffer);
    int ret = session->send(buffer, size);
    pthread_rwlock_unlock(lock);
    return ret;
}

inline int __session_recv_begin(Session_t *session) {
    pthread_rwlock_t *lock;
    uint8_t *buffer;
    size_t size = select_recv(session, &lock, &buffer);
    pthread_rwlock_wrlock(lock);
    bzero(buffer, size);
    return 0;
}

inline int __session_recv_end(Session_t *session) {
    pthread_rwlock_t *lock;
    uint8_t *buffer;
    size_t size = select_recv(session, &lock, &buffer);
    int ret = session->recv(buffer, size);
    pthread_rwlock_unlock(lock);
    return ret;
}

inline int __session_task_begin(Session_t *session) {
    int ret = 0;
    ret = pthread_mutex_lock(&session->poll_lock);
    if (ret == 0) {
        session->tasks_count++;
        //_func_printf_("add task -> %d", session->tasks_count);
    }
    return ret;
}

inline int __session_task_end(Session_t *session) {
    session->tasks_count--;
    pthread_cond_signal(&session->poll_cond);
    return pthread_mutex_unlock(&session->poll_lock);
}

inline int __session_task_wait(Session_t *session, int wake) {
    _func_printf_();
    if (wake > 0) {
        pthread_cond_signal(&session->poll_cond);
    }
    do {
        if (session->poll_state == INVALID) {
            return -ENXIO;
        }
        pthread_cond_wait(&session->task_cond, &session->poll_lock);
    } while (session->poll_state != DATA_READY);
    session->tasks_waiting--;
    return 0;
}

int Session_test(Session_t *session, uint8_t code) {
    _func_printf_();
    int ret = 0;
    session_send_begin(session);
    memset((void *)session->output, code, sizeof(uint8_t));
    ret = session_send_end(session);
    if (ret < 0) {
        _func_printf_("send error(%d)", ret);
        return ret;
    }
    int timeout = 5;
    session_task_begin(session);
    session_task_wait(session, &timeout) {
        if (session->input->id == code)
            break;
    }
    ret = session_task_end(session);
    _func_printf_("timeout -> %d", timeout);
    return ret;
}

Session_t *Session_create(Device_t *host, Recv *recv, Send *send) {
    _func_printf_();
    Session_t *session;
    assert(host && recv && send);
    session = (Session_t *)malloc(sizeof(Session_t));
    if (!session)
        goto done;
    bzero(session, sizeof(Session_t));
    session->recv = recv;
    session->send = send;
    memmove(&session->host, host, sizeof(Device_t));
    session->output = createOutputReport(NULL);
    if (!session->output)
        goto free;
    session->input = createInputReport(NULL);
    if (!session->input)
        goto free;
    if (0 != pthread_mutex_init(&session->poll_lock, NULL))
        goto free;
    if (0 != pthread_rwlock_init(&session->input_lock, NULL))
        goto free;
    if (0 != pthread_rwlock_init(&session->output_lock, NULL))
        goto free;
    if (0 != pthread_cond_init(&session->poll_cond, NULL))
        goto free;
    if (0 != pthread_cond_init(&session->task_cond, NULL))
        goto free;
    if (0 != pthread_barrier_init(&session->barrier, NULL, 2))
        goto free;
    session->poll_state = INVALID;
    session->poll_thread = 0;
    if (0 != pthread_create(&session->poll_thread, NULL, poll, (void *)session))
        goto free;
    return session;

free:
    Session_release(session);
done:
    perror("error when create session\n");
    return NULL;
}

void Session_release(Session_t *session) {
    _func_printf_();
    if (session) {
        if (session->poll_thread) {
            _func_printf_("polling, try to top it...");
            if (session->poll_state == INVALID ||
                session->poll_state == INACTIVE) {
                pthread_barrier_wait(&session->barrier);
            }
            pthread_cancel(session->poll_thread);
        }
        _func_printf_("wait poll thread...");
        pthread_join(session->poll_thread, NULL);
        _func_printf_("wait poll thread...done");
        session->poll_thread = 0;
        pthread_cond_destroy(&session->poll_cond);
        pthread_cond_destroy(&session->task_cond);
        pthread_barrier_destroy(&session->barrier);
        pthread_mutex_destroy(&session->poll_lock);
        pthread_rwlock_destroy(&session->input_lock);
        pthread_rwlock_destroy(&session->output_lock);
        free(session->output);
        free(session->input);
        _func_printf_("release done\n");
    }
    free(session);
}

int Session_active(Session_t *session) {
    _func_printf_();
    return pthread_barrier_wait(&session->barrier);
}
