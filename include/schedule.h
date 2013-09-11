/*******************************************************
*   CopyRight @2011-12
*   Auth: Leo
*   Project: schedule
*
*   DataStruct:
*                           sched_context
*                                   |
*                                   |
*                           sched_queue
*                         /                       \
*                      /                             \
*                   head                            tail
*                   /                                      \
*               sched_1->sched_2->..->sched_n->NULL
*******************************************************/


#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include <stdio.h>

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define LIST_TEMPLATE(structtype, datatype)\
struct structtype { \
    struct structtype *next;\
    datatype *data; \
}
 

#define LIST_QUEUE_TEMPLATE(structtype) \
struct structtype {   \
    struct structtype *head; \
    struct structtype *tail;  \
    struct structtype *next;    \
    int id; \
} 

typedef int (*callback)(void *data);

typedef struct _sched {
    int id;
    struct timeval time;
    int when;
    callback cb;
    void *data;
    struct _sched *next;
}sched_t;


/****************************************************
*   Reserved
*   queuehash:
*       This structure is easy to search , insert  and delelte. 
*
*   id      0          1        2         3          ...
*       +----+----+----+----+----+
*        | id    |         |         |         |         |
*       +----+----+----+----+----+
*   seq    1         0        2         3
*           s0<---s1      s2--->s3-->?
*            |                    |
*             ----->----  
*   
*       sched_queue->head = s1;
*       sched_queue->tail = s3
*******************************************************/
#define QUEUEHASHSIZE   10
struct _queuehash {
    int seq; // sequence
    sched_t *s; // shced
};

struct queue {
    struct _sched *head;
    struct _sched *tail;
};

struct sched_context {
    unsigned int eventcnt; // create ids
    unsigned int schedcnt;  //   the number of current schedule
    struct queue *sched_queue;
    pthread_mutex_t sched_mutex;    // write mutex
    void *data;
};

int hash(char *args);
int sched_add(struct sched_context *schedule, int when, callback cb, void *data);
int sched_del(struct sched_context *schedule, int sched_id);
struct sched_context* sched_create(void);
int sched_run(struct sched_context *schedule);
sched_t *sched_queue_get(struct sched_context *schedule, int id);
int sched_clean(struct sched_context *con);

#endif
