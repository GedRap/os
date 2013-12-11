/*
 * core.h
 *
 * Core multitasking data structures and methods signatures
 */ 


#ifndef CORE_H_
#define CORE_H_

//OS multitasking related state
typedef struct os_multitasking_state_ os_multitasking_state;

struct os_multitasking_state_ {
	os_tasks_queue *queue;
	int last_pid;
};

//not sure for the naming here
//planning to leave os_multitasking namespace for multitasking related functions
//and os_state for the state of the operating system
os_multitasking_state *os_state_multitasking;

void os_multitasking_init();

int os_multitasking_get_next_pid(os_multitasking_state *state);

void os_multitasking_isr();




#endif /* CORE_H_ */