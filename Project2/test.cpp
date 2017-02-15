#include "sched.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

PROCESS_CODE(init)
{
    printf("Initializing process\n");
    mr->r0 = 0;
    mr->r1 = 1;
    mr->r2 = 2;
    mr->r3 = 3;
    RETURN_DESC(PS_SLEEPING, 2, 1);
}

PROCESS_CODE(step)
{
    printf("Stepping process\n");
    mr->r0 += 15;
    mr->r1 += 5;
    mr->r2 += 216;
    mr->r3 += 7;
    RETURN_DESC(PS_EXITED, 2, 0);
}

PROCESS_CODE(p2_step)
{
    int cpu_time = 1;
    printf("Stepping p2\n");
    if (mr->r0 == 15) {
        mr->r1 += 22;
        cpu_time += 3;
    }
    else {
        mr->r1 += 17;
    }
    RETURN_DESC(PS_SLEEPING, cpu_time, mr->r1);
}

PROCESS_CODE(p3_step)
{
    printf("Stepping p3\n");
    RETURN_DESC(PS_RUNNING, 2, 0);
}

int main()
{
    SCHEDULER *s;
    int pid;

    s = new_scheduler(init);
    s->scheduler_algorithm = SA_ROUND_ROBIN;

    pid = fork(s, 1);
    exec(s, pid, "My Process", init, p2_step, -1);

    timer_interrupt(s);
    timer_interrupt(s);
    timer_interrupt(s);
    timer_interrupt(s);

    s->scheduler_algorithm = SA_FAIR;

    pid = fork(s, pid);
    exec(s, pid, "My Fair Process", init, p3_step, -1);

    timer_interrupt(s);
    timer_interrupt(s);
    timer_interrupt(s);
    timer_interrupt(s);
    timer_interrupt(s);

    list_processes(s);

    return 0;
}
