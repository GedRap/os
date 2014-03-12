/*
 * memory.h
 *
 * Memory management related code
 */ 


#ifndef MEMORY_H_
#define MEMORY_H_

#define OS_TASK_STACK_SIZE 128
#define OS_TASK_STACKS 3

//SP address
//Used by get_sp and set_sp methods. Has to be global so that I could use a label
//inside inline assembly
extern unsigned int os_global_sp_addr;
extern unsigned int *os_global_sp_ptr;

extern unsigned int os_task_sp_addr;

//get SP and save it's address to os_global_sp_addr
#define os_get_sp() \
	asm volatile( \
	"push r0 \n" \
	"in r0, __SP_L__ \n" \
	"push r26 \n" \
	"push r27 \n" \
	"lds r26, os_global_sp_ptr \n" \
	"lds r27, os_global_sp_ptr+1 \n" \
	"st X+, r0 \n" \
	"in r0, __SP_H__ \n" \
	"st X+, r0 \n" \
	"pop r27 \n" \
	"pop r26 \n" \
	"pop r0 \n" \
	);
//overwrite existing SP address with a value found in os_global_sp_ptr
#define os_set_sp()  \
	asm volatile( \
	"lds r26, os_global_sp_ptr \n" \
	"lds r27, os_global_sp_ptr + 1 \n" \
	"ld r28, X+ \n" \
	"out __SP_L__, r28 \n" \
	"ld r29, X+ \n" \
	"out __SP_H__, r29 \n" \
	);

typedef char* os_stack;

extern os_stack os_task_stacks[OS_TASK_STACKS];
extern char os_task_stack0[OS_TASK_STACK_SIZE];
extern char os_task_stack1[OS_TASK_STACK_SIZE];

#endif /* MEMORY_H_ */