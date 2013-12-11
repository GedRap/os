/*
 * task.c
 *
 * My Little Operating System https://github.com/GedRap/os
 *  - Multitasking module
 * Single task related methods and data structures
 * Implementation of task.h methods
 */ 

#include <stdlib.h>

#include "task.h"
#include "multitasking.h"
#include "queue.h"
#include "context.h"

//Create task instance, which can be added to the queue
os_task *os_task_create(void (*entry_point)(os_task*), int priority) {
	os_task *task = (os_task *)malloc(sizeof(os_task));
	
	int pid = os_multitasking_state(os_state_multitasking);
	(*task).pid = pid;
	(*task).state = OS_TASK_STATE_NOT_STARTED;
	(*task).entry_point = entry_point;
	
	if(priority < 1) priority = 1;
	(*task).priority = priority;
	
	return task;
}

//Kill the task. Removes the task from the queue and frees it's memory
int os_task_kill(os_tasks_queue *queue, os_task *task) {
	os_tasks_queue_item *queue_item = os_task_queue_find(queue, task);
	
	if(queue_item == NULL) return 0;
	
	os_task_queue_remove(queue, queue_item);
	
	free(task);
	
	return 1;
}

//Either resume or start the task
//Loads the context for the task and jumps to it
void os_task_execute(os_task *task) {
	int state = (*task).state;
	
	if(state == OS_TASK_STATE_RUNNING) {
		(*task).time_slices_had++;
		
		return;
	}
	
	if(state == OS_TASK_STATE_NOT_STARTED) {
		(*task).time_slices_had = 0;
		(*task).state = OS_TASK_STATE_RUNNING;
		
		os_task_current_context_addr = (*task).context_addr;
		
		(*task).entry_point(task);
	}
	
	if(state == OS_TASK_STATE_PAUSED) {
		(*task).time_slices_had = 0;
		(*task).state = OS_TASK_STATE_RUNNING;
		
		os_task_current_context_addr = (*task).context_addr;
		LOAD_CONTEXT();
		asm volatile("ret");
	}
}

//Set priority for the given task
int os_task_set_priority(os_task *task, int priority) {
	(*task).priority = priority;
	
	return 1;
}
//Return to the scheduler, meant to be called from the task itself
//Task passes it's own data structure so the scheduler knows who returned
void os_task_return_to_scheduler(os_task *task) {
	os_multitasking_isr();
}