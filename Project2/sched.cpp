#include "sched.h"

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
// - Context switch must save active process' state into the PROCESS structure
// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {

}

// Create a new scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code)) {
    // Create a new scheduler (on heap)
    SCHEDULER *sch;
    sch = new SCHEDULER;

    // Schedule process 1 (init)
    sch->process_list[0].name = (char*)"init";
    sch->process_list[0].pid = 1;
    sch->process_list[0].switched = 0;
    sch->process_list[0].total_cpu_time = 0;
    sch->process_list[0].switched_cpu_time = 0;
    sch->process_list[0].sleep_time_remaining = 0;
    sch->process_list[0].job_time = -1;
    sch->process_list[0].state = PS_RUNNING;

    // Set process 1 to current process
    sch->current = 0;

    // Return the created scheduler
    return sch;
}

//Duplicate a process (copied, NOT SHARED)
//The passed scheduler forks a process that is a duplicate
//of src_p.
int fork(SCHEDULER *s, PID src_p) {

    return 0;
}

//Overwrite the current process with the new information
//This exec is called on any PID that IS NOT 1!
//exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {

    return 0;
}
