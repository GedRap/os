/*
 * task.h
 *
 * My Little Operating System https://github.com/GedRap/os
 *  - Multitasking module
 * Single task related methods and data structures
 */ 


#ifndef TASK_H_
#define TASK_H_

//Forward type declarations, needed because of the include guards
typedef struct os_task_ os_task;

typedef struct os_tasks_queue_ os_tasks_queue;
typedef struct os_tasks_queue_item_ os_tasks_queue_item;
os_tasks_queue_item *os_task_queue_find(os_tasks_queue *queue, os_task *task);

//Task states
#define OS_TASK_STATE_NOT_STARTED 0
#define OS_TASK_STATE_RUNNING 1
#define OS_TASK_STATE_PAUSED 2

//Task priorities
#define OS_TASK_PRIORITY_LOW 1
#define OS_TASK_PRIORITY_NORMAL 2
#define OS_TASK_PRIORITY_HIGH 3

//Task data structure
struct os_task_ {
	int pid; //unique pid, might be used in future
	
	int state; //state, constants defined above
	
	int *context; //pointer to address where the context is stored
	
	int context_addr; //address of the context
	
	//pointer to the entry function, this os_task object will be passed
	void (*entry_point)(os_task *); 
	
	int priority; //priority, constants defined above
	
	//how many time slices the task had. set to 0
	//when the task is switched to. used for priorities
	int time_slices_had;
};

//Create task instance, which can be added to the queue
os_task *os_task_create(void (*entry_point)(os_task*), int priority);

//Kill task, remove from the queue
int os_task_kill(os_tasks_queue *queue, os_task *task);

//Either resume or start the task
void os_task_execute(os_task *task);

//Set priority for the given task
int os_task_set_priority(os_task *task, int priority);

//Return to the scheduler, meant to be called from the task itself
//Task passes it's own data structure so the scheduler knows who returned
void os_task_return_to_scheduler(os_task *task);


#endif /* TASK_H_ */