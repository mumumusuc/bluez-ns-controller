#ifndef _LINKS_H_
#define _LINKS_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct LinkHeader
{
    struct LinkHeader *pre;
    struct LinkHeader *next;
} LinkHeader_t;

typedef struct
{
    LinkHeader_t header;
    int test;
    int done;
    int timeout;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Task_t;

typedef struct
{
    LinkHeader_t header;
    size_t length;
} TaskLink_t;

#define links_init(links)                       \
    do                                          \
    {                                           \
        (links)->header.pre = &(links)->header; \
        (links)->header.next = NULL;            \
        (links)->length = 0;                    \
    } while (0)
#define entry_init(entry)     \
    do                        \
    {                         \
        (entry)->pre = NULL;  \
        (entry)->next = NULL; \
    } while (0)
#define links_entry(header, type) ({ (typeof(type))(header); })
#define links_add_entry(links, entry)             \
    do                                            \
    {                                             \
        LinkHeader_t *last = (links)->header.pre; \
        last->next = (entry);                     \
        (entry)->pre = last;                      \
        (entry)->next = NULL;                     \
        (links)->header.pre = (entry);            \
        (links)->length++;                        \
    } while (0)
#define links_remove_entry(links, entry)    \
    do                                      \
    {                                       \
        LinkHeader_t *pre = (entry)->pre;   \
        LinkHeader_t *next = (entry)->next; \
        (entry)->pre = NULL;                \
        (entry)->next = NULL;               \
        if (!pre)                           \
            break;                          \
        pre->next = next;                   \
        if (next)                           \
        {                                   \
            next->pre = pre;                \
        }                                   \
        else                                \
        {                                   \
            (links)->header.pre = pre;      \
        }                                   \
        (links)->length--;                  \
    } while (0)

#endif // !_LINKS_H_