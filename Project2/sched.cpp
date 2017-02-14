#include "sched.h"

int main() {

    return 0;
}

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
// - Context switch must save active process' state into the PROCESS structure
// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {

}

//Create a new scheduler
//This function needs to
// - Create a new scheduler (on heap)
// - Schedule process 1 (init)
// - Set process 1 to current process
// - Return the created scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code)) {
    SCHEDULER *sch;
    sch = new SCHEDULER;

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
