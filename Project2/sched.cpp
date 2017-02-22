// Project 2: Scheduler
// Goin Bahnanas 2: Just Two Brothers
// Alex Bahna
// Michael Goin

#include "sched.h"
#include "limits.h"

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
void timer_interrupt(SCHEDULER *s) {

    int i, newProcess, minTime;

    // Pick a process to switch to using the scheduling algorithm
    switch (s->scheduler_algorithm) {
        //Interactive scheduling
        case SA_ROUND_ROBIN:
            i = s->current + 1;
            while(i != (int)s->current)
            {
                if (i == MAX_PROCESSES - 1)
                    i = 0;
                else if (s->process_list[i].state != PS_RUNNING)
                    i++;
                else
                    break;
            }
            newProcess = i;
            break;
        case SA_FAIR:
            // pick the lowest of each process'  total_cpu_time / switched_cpu_time
            minTime = INT_MAX;
            for (i = 0; i < MAX_PROCESSES; i++) {
                if (s->process_list[i].state == PS_RUNNING) {
                    if (s->process_list[i].total_cpu_time == 0) {
                        minTime = 0;
                        newProcess = i;
                    }
                    else if (s->process_list[i].switched_cpu_time == 0) {
                        if (minTime == INT_MAX) {
                            newProcess = i;
                        }
                    }
                    else if ((int)(s->process_list[i].total_cpu_time / s->process_list[i].switched_cpu_time) < minTime) {
                        minTime = (s->process_list[i].total_cpu_time / s->process_list[i].switched_cpu_time);
                        newProcess = i;
                    }
                }
            }
            break;
        //Batch scheduling
        case SA_FCFS:
            // if the current process still has job time left, we keep running it
            if (s->process_list[s->current].job_time != -1) {
                newProcess = s->current;
                break;
            }
            for (i = 0; i < MAX_PROCESSES; i++) {
                if (s->process_list[i].state == PS_RUNNING && i != 1) {
                    newProcess = i;
                    break;
                }
            }
            break;
        case SA_SJF:
            minTime = INT_MAX;
            for (i = 0; i < MAX_PROCESSES; i++)
            {
                if (s->process_list[i].state == PS_RUNNING && s->process_list[i].job_time != -1) {
                    if (s->process_list[i].job_time < minTime)
                    {
                        minTime = s->process_list[i].job_time;
                        newProcess = i;
                    }
                }
            }
            break;
    }

    // switch registers
    s->process_list[s->current].saved_registers = s->active_registers;
    s->active_registers = s->process_list[newProcess].saved_registers;

    s->current = newProcess;
    s->process_list[newProcess].switched++;
    s->process_list[newProcess].switched_cpu_time = 0;

    RETURN rv;
    // Call either init (context switch count == 1), or step (context switch count > 1)
    if (s->process_list[newProcess].switched == 1)
        s->process_list[newProcess].init(&s->active_registers, &rv);
    else
        s->process_list[newProcess].step(&s->active_registers, &rv);

    // Update process’ state based on the RETURN structure
    s->process_list[newProcess].state = rv.state;
    s->process_list[newProcess].total_cpu_time += rv.cpu_time_taken;
    s->process_list[newProcess].sleep_time_remaining = rv.sleep_time;

    // if the process has exited or the job is done, then we should deschedule it
    if (rv.state == PS_EXITED ||
            (s->process_list[newProcess].job_time != -1 &&
             (int)s->process_list[newProcess].total_cpu_time >= s->process_list[newProcess].job_time))
        s->process_list[newProcess].state = PS_NONE;

    for (i = 0; i < MAX_PROCESSES; i++) {
        if (i != newProcess) {
            // Add to the switched_cpu_time of all the other processes
            if (s->process_list[i].state == PS_RUNNING) {
                s->process_list[i].switched_cpu_time += rv.cpu_time_taken;
            }
            // Decrement the sleep time for all the processes who are sleeping
            else if (s->process_list[i].state == PS_SLEEPING) {
                // If a process is done sleeping, make it run
                if (s->process_list[i].sleep_time_remaining <= rv.cpu_time_taken) {
                    s->process_list[i].sleep_time_remaining = 0;
                    s->process_list[i].state = PS_RUNNING;
                }
                // Otherwise substract from sleep time
                else
                    s->process_list[i].sleep_time_remaining -= rv.cpu_time_taken;
            }
        }
    }
}

// Create a new scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code)) {
    // Create a new scheduler (on heap)
    SCHEDULER *s = new SCHEDULER;

    for (int i = 0; i < MAX_MUTEX; i++)
        s->mutex_list[i] = -1;
    for (int i = 0; i < MAX_SEM; i++)
        s->sem_list[i] = -1;

    for (int i = 0; i < MAX_PROCESSES; i++)
        s->process_list[i].state = PS_NONE;

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
    s->active_registers = s->process_list[1].saved_registers;

    // Return the created scheduler
    return s;
}

//Duplicate a process (copied, NOT SHARED)
//The passed scheduler forks a process that is a duplicate
//of src_p.
int fork(SCHEDULER *s, PID src_p) {
    PID pid;

    // find a pid that is not being used
    for (int i = 0; i < MAX_PROCESSES; i++) {
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


//Create or destroy a mutex in the scheduler
//mutex_create returns the ID of the new mutex, or -1 if
//there are no more allocations
MUTEX mutex_create(SCHEDULER *s) {
    int mutexID = -1;
    for (int i = 0; i < MAX_MUTEX; i++) {
        if (s->mutex_list[i] == -1) {
            mutexID = i;
            s->mutex_list[i] = 0;
            break;
        }
    }
    return mutexID;
}
void mutex_destroy(SCHEDULER *s, MUTEX m) {
    s->mutex_list[m] = -1;
}
//Lock or unlock a mutex given by the mutex index m
//If a lock cannot be granted, return 0, otherwise return 1
int mutex_lock(SCHEDULER *s, MUTEX m) {
    if (s->mutex_list[m] != 0)
        return 0;

    s->mutex_list[m] = 1;
    return 1;
}
void mutex_unlock(SCHEDULER *s, MUTEX m) {
    s->mutex_list[m] = 0;
}

//Create or destroy a semaphore in the scheduler
//sem_create returns the ID of the new semaphore, or -1 if
//there are no more allocations.
SEMAPHORE sem_create(SCHEDULER *s) {
    int semID = -1;
    for (int i = 0; i < MAX_SEM; i++) {
        if (s->sem_list[i] == -1) {
            semID = i;
            s->sem_list[i] = 0;
            break;
        }
    }
    return semID;
}
void sem_destroy(SCHEDULER *s, SEMAPHORE m) {
    s->sem_list[m] = -1;
}
//Raises or lowers the semaphore number of semaphore m
void sem_up(SCHEDULER *s, SEMAPHORE m) {
    s->sem_list[m]++;
}
int sem_down(SCHEDULER *s, SEMAPHORE m) {
    if (s->sem_list[m] > 0)
        s->sem_list[m]--;
    return s->sem_list[m];
}
