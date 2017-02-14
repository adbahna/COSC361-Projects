//sched.h - Scheduler Project (COSC 361)
//Stephen Marz 2017

/*****************************************************
 * !!!!DO NOT MODIFY THIS FILE!!!!
 *****************************************************/
#pragma once

#define PROCESS_CODE_PTR(x)     void (*x) (REGISTER_FILE *mr, RETURN *r)
#define PROCESS_CODE(x)     void x (REGISTER_FILE *mr, RETURN *r)
#define RETURN_DESC(x,y,z)      r->state = x; r->cpu_time_taken = y; r->sleep_time = z


#define MAX_PROCESSES       15
//EXTRA CREDIT
#define MAX_MUTEX       15
#define MAX_SEM         15

/*******************************************************
 * Type definitions
 *******************************************************/
typedef int PID;
typedef int REGISTER;

//EXTRA CREDIT
typedef int MUTEX;
typedef int SEMAPHORE;

/*******************************************************
 * SCHEDULER algorithms
 * Describes the scheduling algorithm
 *******************************************************/
typedef enum {
    //Interactive scheduling
    SA_ROUND_ROBIN = 0,
    SA_FAIR,
    //Batch scheduling
    SA_FCFS,
    SA_SJF,
} SCHEDULER_ALGORITHM;

/*******************************************************
 * PROCESS states
 * Describes the states that a process can be in
 *******************************************************/
typedef enum {
    //PS_NONE - process doesn't exist
    PS_NONE = 0,
    //PS_RUNNING - process is running
    PS_RUNNING,
    //PS_SLEEPING - process is sleeping
    PS_SLEEPING,
    //PS_EXITED - the process exited, the scheduler
    //must deschedule the process
    PS_EXITED
} PROCESS_STATE;

/*******************************************************
 * REGISTER structure
 * Describes a processor's register
 *******************************************************/
typedef struct {
    REGISTER r0;
    REGISTER r1;
    REGISTER r2;
    REGISTER r3;
} REGISTER_FILE;


/*******************************************************
 * RETURN structure
 * This is returned by the process' code function
 * (initial and recurring) to let the scheduler know
 * what to do (since syscalls aren't implemented in this
 * project)
 *******************************************************/
typedef struct {
    //The new state that the process will enter
    PROCESS_STATE state;

    //The amount of cpu time that the job took
    unsigned int cpu_time_taken;

    //The amount of time the job wants to sleep (if the state is SLEEPING)
    unsigned int sleep_time;
} RETURN;

/*******************************************************
 * PROCESS structure
 * Describes a single process for running
 *******************************************************/
typedef struct {
    //Name of this process
    char *name;

    //Process ID
    PID pid;

    //How many context switches has this process received?
    unsigned int switched;

    //How much total CPU time this process has received
    unsigned int total_cpu_time;

    //How much CPU time this process has received since a context switch
    unsigned int switched_cpu_time;

    //If the process is sleeping, how much time before the scheduler should
    //wake it up. This only functions if the process is in the SLEEPING_XXX states
    unsigned int sleep_time_remaining;

    //The job's time (if not known == -1)
    int job_time;

    //The saved registers when this process is not active
    REGISTER_FILE saved_registers;

    //The state that this process is in
    PROCESS_STATE state;

    //Initial function, called when process is first started by scheduler
    //ran once and only once
    //void (*init) (REGISTER_FILE *machine_registers, RETURN *r);
    PROCESS_CODE_PTR(init);

    //Step function, called when this process is context switched to
    //assuming the process is running
    PROCESS_CODE_PTR(step);
} PROCESS;


/*******************************************************
 * SCHEDULER structure
 * Describes the scheduler and its associated processes
 *******************************************************/
typedef struct {
    //An array of processes. Process with a state of 0 (PS_NONE)
    //means it doesn't exist (no process)
    PROCESS process_list[MAX_PROCESSES];

    //An index into the process list of the currently
    //running program
    unsigned int current;

    //The algorithm this scheduler will run when interrupted
    //by the timer
    SCHEDULER_ALGORITHM scheduler_algorithm;

    //This is a hack--but these are the active registers in the
    //CPU. The registers in the PROCESS are SAVED registers.
    //DO NOT PASS the PROCESS' saved registers to the init or step
    //functions!!
    REGISTER_FILE active_registers;


    //EXTRA CREDIT
    int mutex_list[MAX_MUTEX];
    int sem_list[MAX_SEM];
} SCHEDULER;

//Print out processes in the following format:
//Pid Name          TotSwch TotCPU SwchCPU State       RemTime
//
//1   Test Process  1       155    5       SLEEPING    15
#if !defined(printf)
#if defined(__cplusplus)
extern "C" {
#endif
int printf(const char *format, ...);
#if defined(__cplusplus)
}
#endif
#endif
static void list_processes(SCHEDULER *s)
{
    unsigned int i;
    static const char *states[] = {
        "!RESERVED!",
        "RUNNING",
        "SLEEPING",
        "??EXITED??"
    };
    printf(" Pid Name           TotSwch TotCPU SwchCPU SlpTime JobTime State\n\n");
    for (i = 0;i < MAX_PROCESSES;i++) {
        if (s->process_list[i].state != PS_NONE) {
            printf("%c%-3d %-14s %-7d %-6d %-7d %-7d %-7d %-12s\n",
                i == s->current ? '*' : ' ',
                s->process_list[i].pid,
                s->process_list[i].name,
                s->process_list[i].switched,
                s->process_list[i].total_cpu_time,
                s->process_list[i].switched_cpu_time,
                s->process_list[i].sleep_time_remaining,
                s->process_list[i].job_time,
                states[s->process_list[i].state]);
        }
    }
}

/*******************************************************
 * Functions that you need to support in this project
 *******************************************************/

//Simulate a timer interrupt from hardware. This should initiate
//the context switch procedure
// - Context switch must save active process' state into the PROCESS structure
// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s);

//Create a new scheduler
//This function needs to
// - Create a new scheduler (on heap)
// - Schedule process 1 (init)
// - Set process 1 to current process
// - Return the created scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(code));

//Duplicate a process (copied, NOT SHARED)
//The passed scheduler forks a process that is a duplicate
//of src_p.
int fork(SCHEDULER *s, PID src_p);

//Overwrite the current process with the new information
//This exec is called on any PID that IS NOT 1!
//exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time);



/*******************************************************
 * EXTRA CREDIT FUNCTIONS
 *******************************************************/
//Create or destroy a mutex in the scheduler
//mutex_create returns the ID of the new mutex, or -1 if
//there are no more allocations
MUTEX mutex_create(SCHEDULER *s);
void mutex_destroy(SCHEDULER *s, MUTEX m);
//mutex_??lock
//Lock or unlock a mutex given by the mutex index m
//If a lock cannot be granted, return 0, otherwise return 1
int mutex_lock(SCHEDULER *s, MUTEX m);
void mutex_unlock(SCHEDULER *s, MUTEX m);

//Create or destroy a semaphore in the scheduler
//sem_create returns the ID of the new semaphore, or -1 if
//there are no more allocations.
SEMAPHORE sem_create(SCHEDULER *s);
void sem_destroy(SCHEDULER *s, SEMAPHORE m);
//sem_??
//Raises or lowers the semaphore number of semaphore m
void sem_up(SCHEDULER *s, SEMAPHORE m);
int sem_down(SCHEDULER *s, SEMAPHORE m);

