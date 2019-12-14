#include "session.h"
#include "defs.h"
#include "input_report.h"
#include "output_report.h"
#include "task.h"
#include <assert.h>
#include <errno.h>
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
    int8_t poll_active;
    pthread_t poll_thread;
    pthread_mutex_t poll_lock;
    pthread_cond_t poll_cond;
    pthread_rwlock_t input_lock;
    pthread_rwlock_t output_lock;
    TaskHead_t tasks;
};

static void poll_cleanup(void *arg) {
    _func_printf_();
    Session_t *session;
    assert(arg);
    session = (Session_t *)arg;
    session->poll_thread = 0;
    session->poll_active = 0;
    _func_printf_("exit poll thread, clean up ...");
}

static int handle_tasks(Session_t *session) {
    _func_printf_();
    int ret = 0;
    session_recv_begin(session);
    ret = session_recv_end(session);
    if (ret < 0) {
        _func_printf_("session_recv error(%d)", ret);
        return ret;
    }
    pthread_rwlock_rdlock(&session->input_lock);
    //_func_printf_("handle tasks begin");
    Task_t *task = (Task_t *)(session->tasks.head.next);
    while (task) {
        Task_t *next = (Task_t *)(task->head.next);
        // _func_printf_("handle task[%p]", task);
        task->state = DONE;
        ret = pthread_cond_signal(&task->cond);
        if (ret != 0) {
            _func_printf_("pthread_cond_signal error(%d), skip this task", ret);
        } else if (!task->persist) {
            list_del(&(session->tasks.head), &task->head);
            // _func_printf_("removable task[%p], remove it", task);
            if (task->delegate) {
                //   _func_printf_("delegated task[%p], free it", task);
                task_free(task);
                free(task);
            }
        } else {
            //_func_printf_("persisted task[%p], wont remove it", task);
        }
        task = next;
        //_func_printf_("next task -> %p", task);
    };
    //_func_printf_("handle tasks over");
    pthread_rwlock_unlock(&session->input_lock);
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
    while (!session->poll_active) {
        ret = pthread_cond_wait(&session->poll_cond, &session->poll_lock);
        if (ret != 0) {
            _func_printf_("pthread_cond_wait error(%d)", ret);
        }
    }
    pthread_mutex_unlock(&session->poll_lock);
    // main loop
    while (1) {
        //_func_printf_("wake up, check my homework ~");
        pthread_mutex_lock(&session->tasks.lock);
        if (!list_empty(&session->tasks.head)) {
            //_func_printf_("I do have homework to do -_-");
            handle_tasks(session);
        }
        pthread_mutex_unlock(&session->tasks.lock);
        //_func_printf_("all done ! ^_^, go to sleep zzZ");
        msleep(1);
    };
    pthread_cleanup_pop(0);
    poll_cleanup((void *)session);
    pthread_exit(NULL);
    _func_printf_("pthread_exit\n");
    return NULL;
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
    task_head_init(&session->tasks);
    session->poll_active = 0;
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
            printf("polling, try to cancel it...\n");
            if (pthread_cancel(session->poll_thread) < 0) {
                perror("pthread_cancel\n");
                exit(-1);
            }
            pthread_join(session->poll_thread, NULL);
            Task_t *task = (Task_t *)session->tasks.head.next;
            while (task) {
                Task_t *next = (Task_t *)task->head.next;
                task_del(&session->tasks, task);
                task_free(task);
                free(task);
                task = next;
            }
            task_head_free(&session->tasks);
            pthread_mutex_destroy(&session->poll_lock);
            pthread_rwlock_destroy(&session->input_lock);
            pthread_rwlock_destroy(&session->output_lock);
            free(session->output);
            free(session->input);
            printf("release done\n");
        }
    }
    free(session);
}

int Session_active(Session_t *session) {
    _func_printf_();
    int ret = 0;
    ret = pthread_mutex_lock(&session->poll_lock);
    session->poll_active = 1;
    ret = pthread_cond_signal(&session->poll_cond);
    ret = pthread_mutex_unlock(&session->poll_lock);
    return ret;
}

int Session_await(Session_t *session) {
    _func_printf_();
    int ret = 0;
    Task_t task = {};
    task_init(&task);
    task_add(&session->tasks, &task);
    pthread_mutex_lock(&task.lock);
    while (task.state != DONE) {
        ret = pthread_cond_wait(&task.cond, &task.lock);
        if (ret != 0) {
            _func_printf_("pthread_cond_wait error(%d)", ret);
            break;
        }
    }
    pthread_mutex_unlock(&task.lock);
    task_free(&task);
    return ret;
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
    ret = 5;
    session_await_timeout(session, &ret) {
        if (session->input->id == code) {
            break;
        }
    }
    ret = ret ? 0 : -ETIMEDOUT;
    return ret;
}
