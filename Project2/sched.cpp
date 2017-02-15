#include "sched.h"
#include "string.h"

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
// - Context switch must save active process' state into the PROCESS structure
// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {
	
	int i, foundSwitch, minTime; 	

    switch (s->scheduler_algorithm) {
        //Interactive scheduling
     	case SA_ROUND_ROBIN:
			
			i = s->current + 1;
			while(1)
			{ 
				if (i == MAX_PROCESSES - 1) 
					i = 1;
				else if (s->process_list[i].state == PS_NONE) 
					i++;
				else {
					foundSwitch = i; 
					break;
				}
			}
          	
			break;

        case SA_FAIR:

			minTime = 0;

			for (i = 1; i < MAX_PROCESSES; i++)
			{
				if ((s->process_list[i].total_cpu_time / s->process_list[i].switched_cpu_time) < minTime);
					minTime = i;
			}

			i = minTime; 

            break;
        //Batch scheduling
        case SA_FCFS:
            break;
        case SA_SJF:
            break;
    }

	s->process_list[s->current].saved_registers = s->active_registers; 
	s->active_registers = s->process_list[i].saved_registers; 
}

// Create a new scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code)) {
    // Create a new scheduler (on heap)
    SCHEDULER *s = new SCHEDULER;
    // Schedule process 1 (init)
    s->process_list[1].name = strdup("init");
    s->process_list[1].pid = 1;
    s->process_list[1].switched = 0;
    s->process_list[1].total_cpu_time = 0;
    s->process_list[1].switched_cpu_time = 0;
    s->process_list[1].sleep_time_remaining = 0;
    s->process_list[1].job_time = -1;
    s->process_list[1].state = PS_RUNNING;
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

    s->process_list[pid].name = s->process_list[src_p].name;
    s->process_list[pid].pid = pid;
    s->process_list[pid].switched = s->process_list[src_p].switched;
    s->process_list[pid].total_cpu_time = s->process_list[src_p].total_cpu_time;
    s->process_list[pid].switched_cpu_time = s->process_list[src_p].switched_cpu_time;
    s->process_list[pid].sleep_time_remaining = s->process_list[src_p].sleep_time_remaining;
    s->process_list[pid].job_time = s->process_list[src_p].job_time;
    s->process_list[pid].state = PS_SLEEPING;
    s->process_list[pid].init = s->process_list[src_p].init;
    s->process_list[pid].step = s->process_list[src_p].step;

    return pid;
}

//Overwrite the current process with the new information
//This exec is called on any PID that IS NOT 1!
//exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {
    s->process_list[pid].name = strdup(new_name);
    s->process_list[pid].switched = 0;
    s->process_list[pid].total_cpu_time = 0;
    s->process_list[pid].switched_cpu_time = 0;
    s->process_list[pid].sleep_time_remaining = 0;
    s->process_list[pid].job_time = job_time;
    s->process_list[pid].state = PS_SLEEPING;
    s->process_list[pid].init = init;
    s->process_list[pid].step = step;

    return 0;
}
