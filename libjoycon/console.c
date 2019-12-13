#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "defs.h"
#include "task.h"
#include "session.h"
#include "console.h"
#include "input_report.h"
#include "output_report.h"

struct Session
{
    Recv *recv;
    Send *send;
    InputReport_t *input;
    OutputReport_t *output;
    Device_t host;
    int polling;
    pthread_t poll_thread;
    pthread_rwlock_t input_lock;
    pthread_rwlock_t output_lock;
    TaskHead_t tasks;
};

#define msleep(t) usleep(1000 * (t))
#define assert_console_session(s)        \
    do                                   \
    {                                    \
        assert(s && s->recv && s->send); \
        assert(s->host.role == CONSOLE); \
    } while (0)

#define _send_begin(session) \
    do                       \
    {                        \
    pthread_rwlock_rdlock(&session->output_lock)

#define _send_end(session)                                               \
    session->send((uint8_t *)session->output, sizeof(*session->output)); \
    pthread_rwlock_unlock(&session->output_lock);                        \
    }                                                                    \
    while (0)

#define _recv_begin(session) \
    do                       \
    {                        \
    pthread_rwlock_wrlock(&session->input_lock)

#define _recv_end(session)                                             \
    session->recv((uint8_t *)session->input, sizeof(*session->input)); \
    pthread_rwlock_unlock(&session->input_lock);                       \
    }                                                                  \
    while (0)

_STATIC_INLINE_ int _wait_until_timeout(Session_t *s, uint8_t subcmd, size_t timeout)
{
    _func_printf_();
    int ret = 0;
    Task_t *task = (Task_t *)malloc(sizeof(Task_t));
    task_init(task);
    task->timeout = timeout;
    task_add(&s->tasks, task);
    _func_printf_("add task(%p) {pre=%p, next=%p}", task, task->head.pre, task->head.next);
    pthread_t self = pthread_self();
    _func_printf_("jam this thread[%lu]...", self);
    pthread_mutex_lock(&task->lock);
    while (task->state != DONE)
    {
        if (0 != pthread_cond_wait(&task->cond, &task->lock))
        {
            _func_printf_("pthread_cond_wait error(%d),what hanppend?", errno);
            break;
        }
        // do test
        _func_printf_("do input test -> %s", (char *)s->input);
        int ok = (s->input->id == subcmd);
        //msleep(5);
        if (ok)
        {
            // do work
            //msleep(5);
            task->state = DONE;
            break;
        }
        else if (--task->timeout)
        {
            _func_printf_("timeout remains(%d), re-add this task", task->timeout);
            task->state = NONE;
            task_add(&s->tasks, task);
            continue;
        }
        else
        {
            // timeout
            ret = -ETIMEDOUT;
            task->state = DONE;
            _func_printf_("timeout, return(%d)", ret);
            break;
        }
    }
    pthread_mutex_unlock(&task->lock);
    _func_printf_("jam over[%lu]...ret = %d\n", self, ret);

    task_free(task);
    free(task);
    return ret;
}

static void _poll_cleanup(void *arg)
{
    _func_printf_();
    assert(arg);
    Session_t *session = (Session_t *)arg;
    _func_printf_("exit poll thread ...");
    session->polling = 0;
    session->poll_thread = 0;
}

static void *_poll(void *arg)
{
    _func_printf_();
    assert(arg);
    int ret = 0;
    Session_t *session;
    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    session = (Session_t *)arg;
    _func_printf_("enter poll thread ...");

    pthread_cleanup_push(_poll_cleanup, (void *)session);
    while (1)
    {
        if (!task_head_empty(&session->tasks))
        {
            //msleep(1000);
            _func_printf_("Good morning~");
            _recv_begin(session);
            ret = _recv_end(session);
            _func_printf_("polled");
            if (ret < 0)
            {
                _func_printf_("poll recv error[%d],should exit?", ret);
            }
            else
            {
                // handle tasks
                pthread_rwlock_rdlock(&session->input_lock);
                pthread_mutex_lock(&session->tasks.lock);
                _func_printf_("handle tasks begin");

                Task_t *task = (Task_t *)(session->tasks.head.next);
                while (task)
                {
                    Task_t *next = (Task_t *)(task->head.next);
                    _func_printf_("handle task[%p]", task);

                    pthread_mutex_lock(&task->lock);
                    task->state = DONE;
                    if (0 == pthread_cond_signal(&task->cond))
                    {
                        list_del(&(session->tasks.head), &task->head);
                        _func_printf_("handle task[%p] over, remove it", task);
                    }
                    pthread_mutex_unlock(&task->lock);
                    if (task->delegate)
                    {
                        _func_printf_("task[%p] delegated, free it", task);
                        task_free(task);
                        free(task);
                    }
                    task = next;
                    _func_printf_("next task -> %p", task);
                };
                // handle task over
                _func_printf_("handle tasks over");
                pthread_mutex_unlock(&session->tasks.lock);
                pthread_rwlock_unlock(&session->input_lock);
            }
            _func_printf_("nothing to do, sleeping..zzZ");
            msleep(15);
        }
    };
    pthread_cleanup_pop(0);
    _poll_cleanup((void *)session);
    pthread_exit(NULL);
    _func_printf_("pthread_exit\n");
    return NULL;
}

int Console_test(Session_t *session, uint8_t code)
{
    _func_printf_();
    int ret = 0;
    _send_begin(session);
    memset((void *)session->output, code, sizeof(uint8_t));
    ret = _send_end(session);
    if (ret < 0)
    {
        _func_printf_("send error(%d)", ret);
        return ret;
    }
    ret = _wait_until_timeout(session, code, 5);
    return ret;
}

int Console_establish(Session_t *session)
{
    return 0;
}

int Console_suspend(Session_t *session)
{
    return 0;
}

int Console_abolish(Session_t *session)
{
    return 0;
}

int Console_getControllerInfo(Session_t *session, ControllerInfo_t *info)
{
    return 0;
}

int Console_getControllerVoltage(Session_t *session, uint16_t *voltage)
{
    return 0;
}

int Console_getControllerColor(Session_t *session, ControllerColor_t *color)
{
    return 0;
}

int Console_setPlayerLight(Session_t *session, Player_t player, PlayerFlash_t flash)
{
    return 0;
}

int Console_setHomeLight(Session_t *session, uint8_t intensity, uint8_t duration, uint8_t repeat, size_t len, HomeLightPattern_t patterns[])
{
    return 0;
}

int Console_enableImu(Session_t *session, uint8_t enable)
{
    return 0;
}

int Console_configImu(Session_t *session, GyroSensitivity_t gs, AccSensitivity_t as, GyroPerformance_t gp, AccBandwidth_t ab)
{
    return 0;
}
//max = 0x20
int Console_readImuRegister(Session_t *session)
{
    return 0;
}

int Console_writeImuRegister(Session_t *session)
{
    return 0;
}

int Console_enableVibration(Session_t *session, uint8_t enable)
{
    return 0;
}

int Console_getControllerData(Session_t *session, Controller_t *controller)
{
    return 0;
}

int Console_setControllerDataCallback(Session_t *session, void (*callback)(Controller_t *))
{
    return 0;
}

int Console_getImuData(Session_t *session, ImuData_t *imu)
{
    return 0;
}

int Console_setImuDataCallback(Session_t *session, void (*callback)(ImuData_t *))
{
    return 0;
}

int Console_testPoll(Session_t *session)
{
    _func_printf_();
    int ret = 0;
    if (session->poll_thread)
    {
        return 1;
    }
    ret = pthread_create(&session->poll_thread, NULL, _poll, (void *)session);
    if (ret != 0)
        fprintf(stderr, "pthread_create error(%d)\n", ret);
    return ret;
}

int Console_stopPoll(Session_t *session)
{
    int ret = 0;
    if (session->polling || session->poll_thread)
    {
        _func_printf_("polling, try to cancel it...");
        if (ret = pthread_cancel(session->poll_thread) < 0)
        {
            fprintf(stderr, "pthread_cancel error(%d)\n", ret);
            return ret;
        }
        pthread_join(session->poll_thread, NULL);
        session->poll_thread = 0;
        _func_printf_("stop poll done");
    }
    return ret;
}

int Console_poll(Session_t *session, PollType_t type)
{
    return 0;
}
