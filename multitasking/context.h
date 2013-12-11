/*
 * context.h
 *
 * My Little Operating System https://github.com/GedRap/os
 *  - Multitasking module 
 * Macros for storing and loading the context
 */ 


#ifndef CONTEXT_H_
#define CONTEXT_H_

//address of the context being used now
volatile int os_task_current_context_addr;
//pointer to the address of context being used now
//ideally this would point to the context itself but couldn't figure out 
//yet how to do it in AVR assembly @TODO find it out?
volatile int *os_task_current_context = &os_task_current_context_addr;

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


#endif /* CONTEXT_H_ */