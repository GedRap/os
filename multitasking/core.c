/*
 * core.c
 *
 * Implementation of core.h
 */ 
#include <stdlib.h> 

#include "core.h"
#include "../memory.h"

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

void os_multitasking_start() {
	os_tasks_queue *queue = os_state_multitasking->queue;
	os_tasks_queue_item *next_item = os_task_scheduler_next(os_state_multitasking);
	
	if(next_item == NULL) {
		return;
	}
	
	os_task *next_task = next_item->task;
	os_task_execute(next_task);
}

void os_multitasking_isr() {
	SAVE_CONTEXT();
	
	os_tasks_queue *queue = os_state_multitasking->queue;
	if(queue->current_task != NULL) {
		os_task *current_task = queue->current_task->task;
		current_task->time_slices_had++;
		current_task->sp = os_task_sp_addr;
		current_task->context_addr = os_task_current_context_addr;
	}
	os_tasks_queue_item *next_item = os_task_scheduler_next(os_state_multitasking);
	
	if(next_item == NULL) {
		return;
	}
	
	os_task *next_task = next_item->task;
	os_task_execute(next_task);	
	
	return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//Create task instance, which can be added to the queue
os_task *os_task_create(void (*entry_point)(os_task *), int priority) {
	os_task *task = (os_task *)malloc(sizeof(os_task));
	
	int pid = os_multitasking_get_next_pid(os_state_multitasking);
	(*task).pid = pid;
	(*task).state = OS_TASK_STATE_NOT_STARTED;
	(*task).entry_point = entry_point;
	
	if(priority < 1) priority = 1;
	(*task).priority = priority;
	
	task->context_addr = &(*os_task_stacks[task->pid - 1]) - OS_TASK_STACK_SIZE + 1;
	
	task->context = &(task->context_addr);
	
	//task->sp = &(*os_task_stacks[task->pid - 1]) - OS_TASK_STACK_SIZE + 1;
	task->stack_bottom = &(*os_task_stacks[task->pid - 1]) - OS_TASK_STACK_SIZE + 1;
	task->sp = &(*os_task_stacks[task->pid - 1]);
	
	return task;
}

//Kill the task. Removes the task from the queue and frees it's memory
int os_task_kill(os_task *task) {
	os_tasks_queue *queue = os_state_multitasking->queue;
	os_tasks_queue_item *queue_item = os_task_queue_find(queue, task);
	
	if(queue_item == NULL) return 0;
	
	os_task_queue_remove(queue, queue_item);
	
	//free(task);
	
	return 1;
}

//Either resume or start the task
//Loads the context for the task and jumps to it
void os_task_execute(os_task *task) {
	int state = (*task).state;
	
	if(state == OS_TASK_STATE_NOT_STARTED) {
		(*task).time_slices_had = 0;
		(*task).state = OS_TASK_STATE_RUNNING;
		
		os_task_current_context_addr = (*task).context_addr;

		void (*entry_point)(os_task *);
		entry_point = task->entry_point;
		
		entry_point(task);
	}
	
	if(state == OS_TASK_STATE_RUNNING) {
		//should never get here
		//remove it?
	}
	
	if(state == OS_TASK_STATE_PAUSED) {
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

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state) {
	state = os_state_multitasking;
	os_tasks_queue *queue = (*state).queue;
	
	if(queue->length == 0) {
		return NULL;
	}
	
	os_tasks_queue_item *current_item = queue->current_task;
	os_task *current_task = current_item->task;
	os_tasks_queue_item *next_item;
	
	if(current_item == NULL) {
		//no task is running, pick the first one
		queue->current_task = queue->queue;
		next_item = queue->queue;
	} else {
		current_task->state = OS_TASK_STATE_PAUSED;
		if(current_task->time_slices_had < current_task->priority) {
			next_item = current_item;
		} else {
			current_task->time_slices_had = 0;
			next_item = current_item->next;
		}
	}		
	
	queue->current_task = next_item;
	
	return next_item;
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
	} else {
		//add to the end of the queue
		(*item).prev = (*queue).queue_tail;
		(*item).next = (*queue).queue;
		
		os_tasks_queue_item *tail = (*queue).queue_tail;
		(*tail).next = item;
		
		os_tasks_queue_item *head = (*queue).queue;
		(*head).prev = item;
	}
	
	(*queue).queue_tail = item;
	
	(*item).task = task;
	(*queue).length++;
	
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