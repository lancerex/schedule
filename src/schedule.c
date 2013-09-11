#include "schedule.h"

// time33 hash
int hash(char *args)
{
    int ret = -1;
    unsigned int hash = 0;
    if ( !args ) {
        return ret;
    }

    while ( *args ) {
        hash = (hash << 5) + hash + *args++;
    } 

    ret = hash % QUEUEHASHSIZE;
    return ret;
}

struct sched_context* sched_create(void)
{
    struct sched_context *con = NULL;

    con = (struct sched_context *)malloc(sizeof(*con));
    if ( !con ){
        return con;
    }

    memset(con, 0, sizeof(struct sched_context));

    con->sched_queue = (struct queue *)malloc(sizeof(struct queue));
    if ( !con->sched_queue ){
        free(con);
        con = NULL;
        return NULL;
    }
    con->sched_queue->head = NULL;
    con->sched_queue->tail = NULL;

    return con;
}

sched_t *sched_queue_get(struct sched_context *schedule, int id)
{
    sched_t *cur = NULL;
    struct queue *pos = NULL;

    if ( !schedule )
    {
        return NULL;
    }
    pos = schedule->sched_queue;
    if ( !pos )
    {
        return NULL;
    }
    
    for ( cur = pos->head; cur; cur = cur->next )
    {
        if ( cur->id == id )
        {
            return cur;
        }
    }

    return NULL;
}

int sched_add(struct sched_context *schedule, int when, callback cb, void *data)
{
    sched_t *add = NULL;
    struct queue *q = NULL;

    if ( !schedule )
    {
        return -1;
    }
    
    add = (sched_t *)malloc(sizeof(*add));

    add->id = ++(schedule->schedcnt);   // allocate  id
    schedule->eventcnt++;
    add->when = when;
    add->cb = cb;
    add->data = data;
    add->next = 0;
    gettimeofday(&add->time, NULL);
    add->time.tv_sec += when;
//    schedule->schedcnt++;

    pthread_mutex_lock(&(schedule->sched_mutex));
    q = schedule->sched_queue;
    if ( !q )
    {
        return -1;
    }
    
    if ( !q->head )
    {
        q->head = add;
        q->tail = add;
    }
    else{
        q->tail->next = add;
        q->tail = add;
    }

    sched_t *p = schedule->sched_queue->head;
    int i = 0;
    while ( p != NULL ) {
        i++;
        p = p->next;
    }
    pthread_mutex_unlock(&(schedule->sched_mutex));
    
    return add->id;
}

int sched_del(struct sched_context *schedule, int sched_id)
{
    sched_t *del = NULL, *pre;
    int id = 0;
    if ( !schedule || !schedule->sched_queue )
    {
        return -1;
    }

    pre = schedule->sched_queue->head;
	del = schedule->sched_queue->head;
    while ( del != NULL )
    {
        if ( del->id == sched_id )
        {
            if ( del != schedule->sched_queue->head && 
                    del != schedule->sched_queue->tail )
                pre->next = del->next;
                
            // maintain the queue->head
            if ( del == schedule->sched_queue->head ) {
                schedule->sched_queue->head = del->next;
                pre = NULL;
            }
            // maintain the queue->tail
            if ( del == schedule->sched_queue->tail )
                    schedule->sched_queue->tail = pre;
            
            schedule->eventcnt--;
            id = del->id;
            free(del);
			break;
        }
        pre = del;
		del = del->next;
    }


    sched_t *p = schedule->sched_queue->head;
    int i = 0;
    while ( p != NULL ) {
        i++;
        p = p->next;
    }
    
    return 0;
    
}

int sched_run(struct sched_context *schedule)
{
    struct queue *q = NULL;
    sched_t *s = NULL, *next = NULL;
    struct timeval t, timeout;
    fd_set rdfds;
    int fd = 0;

    if ( !schedule || !schedule->sched_queue )
    {
       return 1;
    }
    q = schedule->sched_queue;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( fd < 0 ) {
        return 1;
    }
    FD_ZERO(&rdfds);
    FD_SET(fd, &rdfds);
    memset(&timeout, 0, sizeof(timeout));
    
    for ( ; ; )
    {
        timeout.tv_sec = 1;
        s = q->head;
        select(fd+1, &rdfds, NULL, NULL, &timeout);
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        for (; s; )
        {
            gettimeofday(&t, NULL);
            
            pthread_mutex_lock(&(schedule->sched_mutex));
            next = s->next;
            if ( t.tv_sec >= s->time.tv_sec )
            {
                s->cb(s->data);
                sched_del(schedule, s->id);
            }
            s = next;
            pthread_mutex_unlock(&(schedule->sched_mutex));
            
            select(fd+1, &rdfds, NULL, NULL, &timeout);
        }
    }

    close(fd);
    return 0;
}

int sched_clean(struct sched_context *con)
{
    sched_t *it = NULL, *head;
    if ( !con ) {
        return 1;
    }

    if ( con->sched_queue ) {
        head = con->sched_queue->head;
        while ( it = head ) {
            head = head->next;
            free(it);
        }
    } else {
        free(con);
        return 0;
    }

    free(con->sched_queue);
    free(con);

    return 0;
}
