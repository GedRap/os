/*
 * queue.h
 *
 * Created: 24/11/2013 16:02:49
 *  Author: Ged
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#include "task.h"

//Single item of the tasks queue
typedef struct os_tasks_queue_item_struct os_tasks_queue_item;

typedef struct  os_tasks_queue_item_struct {
	os_task *task;
	os_tasks_queue_item *prev;
	os_tasks_queue_item *next;
};



//Circular linked list with moving pointer current_task
// to store the queue of tasks
typedef struct {
	//points to the head of the queue
	os_tasks_queue_item *queue; 
	
	//task being executed now
	os_tasks_queue_item *current_task; 
	
	//points to the last inserted task
	os_tasks_queue_item *queue_tail; 
	
	//total length
	int length; 
} os_tasks_queue;



//Init the queue
os_tasks_queue *os_tasks_queue_init();

//Add item to the tasks queue
os_tasks_queue_item *os_task_queue_add(os_tasks_queue *queue, os_task *task);

//Remove item from the queue
int os_task_queue_remove(os_tasks_queue *queue, os_tasks_queue_item *item);


#endif /* QUEUE_H_ */