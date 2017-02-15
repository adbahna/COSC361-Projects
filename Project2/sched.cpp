#include "sched.h"
#include "string.h"

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
void timer_interrupt(SCHEDULER *s) {

    // Pick a process to switch to using the scheduling algorithm
    switch (s->scheduler_algorithm) {
        //Interactive scheduling
        case SA_ROUND_ROBIN:
            break;
        case SA_FAIR:
            // pick the lowest of each process'  total_cpu_time / switched_cpu_time
            break;

        //Batch scheduling
        // run a batch process until the cpu time is >= the job time OR it returns PS_EXITED
        case SA_FCFS:
            break;
        case SA_SJF:
            break;
    }


    s->process_list[i].switched++;

    // Call either init or step, based on the context switch numbers
    if (s->process_list[i].switched == 1) {

    }
    // Call either init (context switch count == 1), or step (context switch count > 1)
    // Read RETURN structure
    // Update process’ state based on the RETURN structure

}

// Create a new scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code)) {
    // Create a new scheduler (on heap)
    SCHEDULER *s = new SCHEDULER;

    // Schedule process 1 (init)
    s->process_list[1].name = (char*)"init";
    s->process_list[1].pid = 1;
    s->process_list[1].switched = 0;
    s->process_list[1].total_cpu_time = 0;
    s->process_list[1].switched_cpu_time = 0;
    s->process_list[1].sleep_time_remaining = 0;
    s->process_list[1].job_time = -1;
    s->process_list[1].state = PS_RUNNING;
    s->process_list[1].init = code;
    s->process_list[1].step = code;

    // Set process 1 to current process
    s->current = 1;

    // Return the created scheduler
    return s;
}

//Duplicate a process (copied, NOT SHARED)
//The passed scheduler forks a process that is a duplicate
//of src_p.
int fork(SCHEDULER *s, PID src_p) {
    PID pid;

    // find a pid that is not being used
    for (int i = 2; i < MAX_PROCESSES; i++) {
        if (s->process_list[i].state == PS_NONE) {
            pid = i;
            break;
        }
    }

    s->process_list[pid] = s->process_list[src_p];
    s->process_list[pid].pid = pid;

    return pid;
}

//Overwrite the current process with the new information
//This exec is called on any PID that IS NOT 1!
//exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {
    s->process_list[pid].name = (char*)new_name;
    s->process_list[pid].switched = 0;
    s->process_list[pid].total_cpu_time = 0;
    s->process_list[pid].switched_cpu_time = 0;
    s->process_list[pid].sleep_time_remaining = 0;
    s->process_list[pid].job_time = job_time;
    s->process_list[pid].state = PS_RUNNING;
    s->process_list[pid].init = init;
    s->process_list[pid].step = step;

    return 0;
}
