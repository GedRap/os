/*
 * scheduler.h
 *
 * My Little Operating System https://github.com/GedRap/os
 *  - Multitasking module
 * Header file for the tasks scheduler
 */ 


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "queue.h"

typedef struct os_tasks_queue_ os_tasks_queue;
typedef struct os_multitasking_state_ os_multitasking_state;

//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state);


//Single item of the tasks queue
typedef struct os_tasks_queue_item_ os_tasks_queue_item;

struct os_tasks_queue_item_ {
	os_task *task;
	os_tasks_queue_item *prev;
	os_tasks_queue_item *next;
};



//Circular linked list with moving pointer current_task
// to store the queue of tasks
typedef struct os_tasks_queue_ os_tasks_queue;
struct os_tasks_queue_ {
	//points to the head of the queue
	os_tasks_queue_item *queue;
	
	//task being executed now
	os_tasks_queue_item *current_task;
	
	//points to the last inserted task
	os_tasks_queue_item *queue_tail;
	
	//total length
	int length;
} ;



//Init the queue
os_tasks_queue *os_tasks_queue_init();

//Add item to the tasks queue
os_tasks_queue_item *os_task_queue_add(os_tasks_queue *queue, os_task *task);

//Remove item from the queue
int os_task_queue_remove(os_tasks_queue *queue, os_tasks_queue_item *item);

//Find task in the queue. Return NULL if not found
os_tasks_queue_item *os_task_queue_find(os_tasks_queue *queue, os_task *task);


#endif /* SCHEDULER_H_ */