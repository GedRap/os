/*
 * core.h
 *
 * Core multitasking data structures and methods signatures
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
int os_task_kill(os_task *task);

//Either resume or start the task
void os_task_execute(os_task *task);

//Set priority for the given task
int os_task_set_priority(os_task *task, int priority);

//Return to the scheduler, meant to be called from the task itself
//Task passes it's own data structure so the scheduler knows who returned
void os_task_return_to_scheduler(os_task *task);


#endif /* TASK_H_ */

#ifndef CORE_H_
#define CORE_H_

//forward declarations
//typedef struct os_task_ os_task;

//OS multitasking related state
typedef struct os_multitasking_state_ os_multitasking_state;

typedef struct os_tasks_queue_ os_tasks_queue;
typedef struct os_tasks_queue_item_ os_tasks_queue_item;

os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state);



//Init the queue. Forward declaration.
// @TODO think more about dependencies. This declaration doesn't really
// belong here
os_tasks_queue *os_tasks_queue_init();

int os_task_queue_remove(os_tasks_queue *queue, os_tasks_queue_item *item);

struct os_multitasking_state_ {
	os_tasks_queue *queue;
	int last_pid;
	unsigned int os_sp;
};

//not sure for the naming here
//planning to leave os_multitasking namespace for multitasking related functions
//and os_state for the state of the operating system
extern os_multitasking_state *os_state_multitasking;

void os_multitasking_init();

int os_multitasking_get_next_pid(os_multitasking_state *state);

void os_multitasking_start();

 __attribute__ ( ( signal, naked ) ) void os_multitasking_isr();

//////////////////////////////////////////////////////////////////////////
// Context Management
//////////////////////////////////////////////////////////////////////////
//address of the context being used now
volatile extern int os_task_current_context_addr;
//pointer to the address of context being used now
//ideally this would point to the context itself but couldn't figure out
//yet how to do it in AVR assembly @TODO find it out?
volatile extern int *os_task_current_context;
//load context from the address os_task_current_context_addr
#define LOAD_CONTEXT() \
asm volatile(      \
"lds r26, os_task_current_context \n" \
"lds r27, os_task_current_context + 1 \n" \
"ld r28, X+ \n" \
"out __SP_L__, r28 \n" \
"ld r29, X+ \n" \
"out __SP_H__, r29 \n" \
"pop r31 \n" \
"pop r30 \n" \
"pop r29 \n" \
"pop r28 \n" \
"pop r27 \n" \
"pop r26 \n" \
"pop r25 \n" \
"pop r24 \n" \
"pop r23 \n" \
"pop r22 \n" \
"pop r21 \n" \
"pop r20 \n" \
"pop r19 \n" \
"pop r18 \n" \
"pop r17 \n" \
"pop r16 \n" \
"pop r15 \n" \
"pop r14 \n" \
"pop r13 \n" \
"pop r12 \n" \
"pop r11 \n" \
"pop r10 \n" \
"pop r9 \n" \
"pop r8 \n" \
"pop r7 \n" \
"pop r6 \n" \
"pop r5 \n" \
"pop r4 \n" \
"pop r3 \n" \
"pop r2 \n" \
"pop r1 \n" \
"pop r0 \n" \
"out __SREG__, r0 \n" \
"pop r0 \n" \
);

#define SAVE_CONTEXT()  \
asm volatile (      \
"push r0 \n"        \
"in r0, __SREG__ \n" \
"push r0         \n" \
"push r1         \n" \
"clr r1          \n" \
"push r2         \n" \
"push r3         \n" \
"push r4         \n" \
"push r5         \n" \
"push r6         \n" \
"push r7         \n" \
"push r8         \n" \
"push r9         \n" \
"push r10        \n" \
"push r11        \n" \
"push r12        \n" \
"push r13        \n" \
"push r14        \n" \
"push r15        \n" \
"push r16        \n" \
"push r17        \n" \
"push r18        \n" \
"push r19        \n" \
"push r20        \n" \
"push r21        \n" \
"push r22        \n" \
"push r23        \n" \
"push r24        \n" \
"push r25        \n" \
"push r26        \n" \
"push r27        \n" \
"push r28        \n" \
"push r29        \n" \
"push r30        \n" \
"push r31        \n" \
"in r0, __SP_L__ \n" \
"lds r26, os_task_current_context \n" \
"lds r27, os_task_current_context+1 \n" \
"st X+, r0 \n" \
"in r0, __SP_H__ \n" \
"st X+, r0 \n" \
);

#endif /* CORE_H_ */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

//typedef struct os_task_ os_task;

//Single item of the tasks queue
typedef struct os_tasks_queue_item_ os_tasks_queue_item;


//Get the next task to execute, based on the current state of the multitasking
//which involves what's the current task now, how much time has the task had,
//what's it's priority and etc.
//Current implementation is a round robin
os_tasks_queue_item *os_task_scheduler_next(os_multitasking_state *state);


struct os_tasks_queue_item_ {
	os_task *task;
	os_tasks_queue_item *prev;
	os_tasks_queue_item *next;
};



//Circular linked list with moving pointer current_task
// to store the queue of tasks
typedef struct os_tasks_queue_ os_tasks_queue;
struct os_tasks_queue_ {
	//points to the head of the queue
	os_tasks_queue_item *queue;
	
	//task being executed now
	os_tasks_queue_item *current_task;
	
	//points to the last inserted task
	os_tasks_queue_item *queue_tail;
	
	//total length
	int length;
} ;



//Init the queue
os_tasks_queue *os_tasks_queue_init();

//Add item to the tasks queue
os_tasks_queue_item *os_task_queue_add(os_tasks_queue *queue, os_task *task);

//Remove item from the queue
int os_task_queue_remove(os_tasks_queue *queue, os_tasks_queue_item *item);

//Find task in the queue. Return NULL if not found
os_tasks_queue_item *os_task_queue_find(os_tasks_queue *queue, os_task *task);


#endif /* SCHEDULER_H_ */