#ifndef _TASK_H_
#define _TASK_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct ListHead
{
    struct ListHead *pre, *next;
} ListHead_t;

#define list_head_init(head)  \
    do                        \
    {                         \
        (head)->pre = (head); \
        (head)->next = NULL;  \
    } while (0)

#define list_empty(list) ((list)->pre == (list))

#define list_add(list, entry)           \
    do                                  \
    {                                   \
        ListHead_t *last = (list)->pre; \
        last->next = (entry);           \
        (entry)->pre = last;            \
        (entry)->next = NULL;           \
        (list)->pre = (entry);          \
    } while (0)

#define list_del(list, entry)             \
    do                                    \
    {                                     \
        ListHead_t *pre = (entry)->pre;   \
        ListHead_t *next = (entry)->next; \
        (entry)->pre = NULL;              \
        (entry)->next = NULL;             \
        if (!pre)                         \
            break;                        \
        pre->next = next;                 \
        if (next)                         \
        {                                 \
            next->pre = pre;              \
        }                                 \
        else                              \
        {                                 \
            (list)->pre = pre;            \
        }                                 \
    } while (0)

typedef struct
{
    ListHead_t head;
    pthread_mutex_t lock;
} TaskHead_t;

typedef enum
{
    NONE,
    DONE,
} TaskState_t;

typedef struct
{
    ListHead_t head;
    TaskState_t state;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int timeout;
    int delegate;
} Task_t;

#define task_head_init(header)                     \
    do                                             \
    {                                              \
        bzero(header, sizeof(TaskHead_t));         \
        list_head_init(&(header)->head);           \
        pthread_mutex_init(&(header)->lock, NULL); \
    } while (0)

#define task_head_free(header)                  \
    do                                          \
    {                                           \
        pthread_mutex_destroy(&(header)->lock); \
    } while (0)

#define task_head_empty(header) ({pthread_mutex_lock(&(header)->lock);int empty = list_empty(&(header)->head);pthread_mutex_unlock(&(header)->lock);empty; })

#define task_init(task)                          \
    do                                           \
    {                                            \
        bzero(task, sizeof(Task_t));             \
        list_head_init(&(task)->head);           \
        pthread_mutex_init(&(task)->lock, NULL); \
        pthread_cond_init(&(task)->cond, NULL);  \
        (task)->state = NONE;                    \
    } while (0)

#define task_free(task)                       \
    do                                        \
    {                                         \
        pthread_mutex_destroy(&(task)->lock); \
        pthread_cond_destroy(&(task)->cond);  \
    } while (0)

#define task_add(header, task)                    \
    do                                            \
    {                                             \
        pthread_mutex_lock(&(header)->lock);      \
        list_add(&(header)->head, &(task)->head); \
        pthread_mutex_unlock(&(header)->lock);    \
    } while (0)

#define task_del(header, task)                    \
    do                                            \
    {                                             \
        pthread_mutex_lock(&(header)->lock);      \
        list_del(&(header)->head, &(task)->head); \
        pthread_mutex_unlock(&(header)->lock);    \
    } while (0)

#endif // _TASK_H_