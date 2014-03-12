/*
 * memory.c
 *
 * Memory management related code
 */

#include "memory.h"

char os_task_stack0[OS_TASK_STACK_SIZE];
char os_task_stack1[OS_TASK_STACK_SIZE];
char os_task_stack2[OS_TASK_STACK_SIZE];

os_stack os_task_stacks[] = {&os_task_stack0, &os_task_stack1, &os_task_stack2};