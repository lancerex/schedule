#include "schedule.h"

int sched1(void *data)
{
    printf("sched1 say hello\n");
    return 0;
}

int sched2(void *data)
{
    printf("sched2 say hello\n");
    return 0;
}

int sched3(void *data)
{
    printf("sched3 say hello\n");
    return 0;
}

struct sched_context *schedule;

int main(int argc, char **argv)
{

    schedule = sched_create();
    if ( !schedule )
        return 1;
    sched_add(schedule, 3, sched1, NULL);
    sched_add(schedule, 2, sched2, NULL);
    sched_add(schedule, 1, sched3, NULL);
    sched_run(schedule);

    return 0;
}
