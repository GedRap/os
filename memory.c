/*
 * memory.c
 *
 * Memory management related code
 */ 

#include "memory.h"

//SP manipulation
volatile inline unsigned int os_get_sp() {
	asm volatile(
	"push r0 \n"
	"in r0, __SP_L__ \n"
	"push r26 \n"
	"push r27 \n"
	"lds r26, os_global_sp_ptr \n"
	"lds r27, os_global_sp_ptr+1 \n"
	"st X+, r0 \n"
	"in r0, __SP_H__ \n"
	"st X+, r0 \n"
	"pop r27 \n"
	"pop r26 \n"
	"pop r0 \n"
	);
	
	return os_global_sp_addr;
}

volatile inline char os_set_sp() {
	asm volatile(
	"lds r26, os_global_sp_ptr \n"
	"lds r27, os_global_sp_ptr + 1 \n"
	"ld r28, X+ \n"
	"out __SP_L__, r28 \n"
	"ld r29, X+ \n"
	"out __SP_H__, r29 \n"
	);
	
	return 1;
}