#ifndef _TASK_H_
#define _TASK_H_

#include <pthread.h>
#include <stdlib.h>

typedef struct ListHead {
    struct ListHead *pre, *next;
} ListHead_t;

#define list_head_init(head)                                                   \
    do {                                                                       \
        (head)->pre = (head);                                                  \
        (head)->next = NULL;                                                   \
    } while (0)

#define list_empty(list) ((list)->pre == (list))

#define list_add(list, entry)                                                  \
    do {                                                                       \
        ListHead_t *last = (list)->pre;                                        \
        last->next = (entry);                                                  \
        (entry)->pre = last;                                                   \
        (entry)->next = NULL;                                                  \
        (list)->pre = (entry);                                                 \
    } while (0)

#define list_del(list, entry)                                                  \
    do {                                                                       \
        ListHead_t *pre = (entry)->pre;                                        \
        ListHead_t *next = (entry)->next;                                      \
        (entry)->pre = NULL;                                                   \
        (entry)->next = NULL;                                                  \
        if (!pre)                                                              \
            break;                                                             \
        pre->next = next;                                                      \
        if (next) {                                                            \
            next->pre = pre;                                                   \
        } else {                                                               \
            (list)->pre = pre;                                                 \
        }                                                                      \
    } while (0)

#endif // _TASK_H_