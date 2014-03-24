/*
 * OS.cpp
 *
 * Created: 24/11/2013 03:58:05
 *  Author: Ged
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>

#include "multitasking/core.h"
#include "memory.h"

//use timer to switch tasks if set to 1
//if set to 0, tasks will call scheduler and won't be swithced otherwise
//helpful for debugging to make it deterministic
#define OS_MULTITASKING_TIMER 0

//Global variable which holds current multitasking state
//Such as pointer to the Queue
os_multitasking_state *os_state_multitasking;

//SP address
//Used by get_sp and set_sp methods. Has to be global so that I could use a label
//inside inline assembly
unsigned int os_global_sp_addr;
unsigned int *os_global_sp_ptr = &os_global_sp_addr;

unsigned int os_task_sp_addr;

volatile int os_task_current_context_addr;
volatile int *os_task_current_context = &os_task_current_context_addr;

volatile int os_context_addr;
volatile int *os_context = &os_context_addr;

extern char *os_task_stacks[OS_TASK_STACKS];
extern char os_task_stack0[OS_TASK_STACK_SIZE];
extern char os_task_stack1[OS_TASK_STACK_SIZE];
extern char os_task_stack2[OS_TASK_STACK_SIZE];

int task1_counter = 0;
int task2_counter = 0;
int task3_counter = 0;

void task1_ep(os_task *task) {
	task_body:
		task1_counter++;
		if(OS_MULTITASKING_TIMER == 0) {
			os_multitasking_isr();
		}		
	goto task_body;
}

void task2_ep(os_task *task) {
	task_body:
		task2_counter++;
		if(OS_MULTITASKING_TIMER == 0) {
			os_multitasking_isr();
		}
	goto task_body;
}

void task3_ep(os_task *task) {
	task_body:
		task3_counter++;
		if(task3_counter == 10) {
			task->priority = OS_TASK_PRIORITY_NORMAL;
		}
		if(OS_MULTITASKING_TIMER == 0) {
			os_multitasking_isr();
		}
	goto task_body;
}

int main(void)
{
	os_multitasking_init();
	os_tasks_queue *queue = os_state_multitasking->queue;
	
	os_task *task1 = os_task_create(&task1_ep, OS_TASK_PRIORITY_LOW);
	os_tasks_queue_item *task1_item = os_task_queue_add(queue, task1);
	
	os_task *task2 = os_task_create(&task2_ep, OS_TASK_PRIORITY_LOW); 
	os_tasks_queue_item *task2_item = os_task_queue_add(queue, task2);
	
	os_task *task3 = os_task_create(&task3_ep, OS_TASK_PRIORITY_LOW);
	os_tasks_queue_item *task3_item = os_task_queue_add(queue, task3);
	
	os_multitasking_start();
	
    while(1)
    {
		
    }
}