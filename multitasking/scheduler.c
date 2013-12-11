/*
 * scheduler.c
 *
 * Created: 07/12/2013 19:36:55
 *  Author: Ged
 */ 

#include "multitasking.h"
#include "scheduler.h"
#include "queue.h"

//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state) {
	os_tasks_queue *queue = state->queue;
	
	os_tasks_queue_item *current_item = queue->current_task;
	os_task *current_task = current_item->task;
	
	if(current_task->time_slices_had >= current_task->priority) {
		//go to the next task
		return current_item->next;
	} else {
		//keep the same task running
		return current_item;
	}
}