/*
 * OS.cpp
 *
 * Created: 24/11/2013 03:58:05
 *  Author: Ged
 */ 


#include <avr/io.h>

#include "multitasking/scheduler.h"

os_multitasking_state *os_state_multitasking;

 volatile int *os_task_current_context = &os_task_current_context_addr;

int task1_counter = 0;
int task2_counter = 0;

void task1_ep(os_task *task) {
	task_body:
		task1_counter++;
		//os_task_return_to_scheduler(task);
	goto task_body;
}

void task2_ep(os_task *task) {
	task_body:
		task2_counter++;
		//os_task_return_to_scheduler(task);
	goto task_body;
}

int main(void)
{
	os_multitasking_init();
	os_tasks_queue *queue = os_state_multitasking->queue;
	
	os_task *task1 = os_task_create(&task1_ep, OS_TASK_PRIORITY_LOW);
	os_tasks_queue_item *task1_item = os_task_queue_add(queue, task1);
	
	os_task *task2 = os_task_create(&task2_ep, OS_TASK_PRIORITY_NORMAL); 
	os_tasks_queue_item *task2_item = os_task_queue_add(queue, task2);
		
    while(1)
    {
        //TODO:: Please write your application code 
    }
}