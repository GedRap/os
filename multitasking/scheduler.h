/*
 * scheduler.h
 *
 * My Little Operating System https://github.com/GedRap/os
 *  - Multitasking module
 * Header file for the tasks scheduler
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "multitasking.h"

typedef struct os_multitasking_state_ os_multitasking_state;

//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state);



#endif /* SCHEDULER_H_ */