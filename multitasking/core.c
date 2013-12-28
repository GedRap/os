/*
 * core.c
 *
 * Implementation of core.h
 */ 
#include <stdlib.h> 

#include "task.h"
#include "core.h"

//Initialize multitasking. Sets up the global state variable
void os_multitasking_init() {
	os_state_multitasking = (os_multitasking_state*)malloc(sizeof(os_multitasking_state));
	
	(*os_state_multitasking).last_pid = 0;
	
	os_tasks_queue *queue = os_tasks_queue_init();
	(*os_state_multitasking).queue = queue;
}

int os_multitasking_get_next_pid(os_multitasking_state *state) {
	(*state).last_pid++;
	
	return (*state).last_pid;
}

void os_multitasking_isr() {
	SAVE_CONTEXT();
	os_tasks_queue_item *next_item = os_task_scheduler_next(os_state_multitasking);
	os_task *next_task = next_item->task;
	os_task_execute(next_task);	
}