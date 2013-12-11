/*
 * scheduler.c
 *
 * Created: 07/12/2013 19:36:55
 *  Author: Ged
 */ 

#include "queue.h"
#include "multitasking.h"
#include "scheduler.h"


//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state) {
	os_tasks_queue *queue = (*state).queue;
	
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

//Init the queue
os_tasks_queue *os_tasks_queue_init() {
	os_tasks_queue *queue = (os_tasks_queue*)malloc(sizeof(os_tasks_queue));
	
	(*queue).current_task = NULL;
	(*queue).queue = NULL;
	(*queue).queue_tail = NULL;
	
	(*queue).length = 0;
	
	return queue;
}

//Add item to the tasks queue
os_tasks_queue_item *os_task_queue_add(os_tasks_queue *queue, os_task *task) {
	os_tasks_queue_item *item = (os_tasks_queue_item*) malloc(sizeof(os_tasks_queue_item));
	
	if((*queue).length == 0) {
		//first element in the queue, everything points to it
		(*item).next = item;
		(*item).prev = item;
		
		(*queue).queue = item;
		(*queue).queue_tail = item;
	} else {
		//add to the end of the queue
		(*item).prev = (*queue).queue_tail;
		(*item).next = (*queue).queue;
		
		os_tasks_queue_item *tail = (*queue).queue_tail;
		(*tail).next = item;
		
		os_tasks_queue_item *head = (*queue).queue;
		(*head).prev = item;
	}
	
	(*item).task = task;
	(*queue).length = 0;
	
	return item;
	
	
}

//Remove item from the queue
//Returns value greater than 0 on success
int os_task_queue_remove(os_tasks_queue *queue, os_tasks_queue_item *item) {
	os_tasks_queue_item *prev = (*item).prev;
	os_tasks_queue_item *next = (*item).next;
	
	if((*queue).length == 1) {
		//we are removing the last item from the queue so basically
		//set all the pointers to null
		(*queue).queue = NULL;
		(*queue).queue_tail = NULL;
	} else {
		//since the queue is a circular list, we just need to care
		//about setting (next) pointer
		//in the previous element to the right place
		(*prev).next = next;
		
		//if it's the last element in the queue,
		//move the tail back
		if((*queue).queue_tail == item) {
			(*queue).queue_tail = prev;
		}
	}
	
	(*queue).length--;
	
	free(item);
	
	return 1;
}

//Find task in the queue. Return NULL if not found
os_tasks_queue_item *os_task_queue_find(os_tasks_queue *queue, os_task *task) {
	if((*queue).length == 0) return NULL;
	
	int found = 0;
	
	os_tasks_queue_item *pointer = (*queue).queue;
	do {
		if((*pointer).task == task) {
			found = 1;
			break;
		}
	} while(pointer != (*queue).queue);
	
	if(!found) return NULL;
	
	return pointer;
}