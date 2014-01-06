/*
 * memory.h
 *
 * Memory management related code
 */ 


#ifndef MEMORY_H_
#define MEMORY_H_


//SP address
//Used by get_sp and set_sp methods. Has to be global so that I could use a label
//inside inline assembly
extern unsigned int os_global_sp_addr;
extern unsigned int *os_global_sp_ptr;

//get SP and save it's address to os_global_sp_addr
extern volatile inline unsigned int os_get_sp();
//overwrite existing SP address with a value found in os_global_sp_ptr
extern volatile inline char os_set_sp();


#endif /* MEMORY_H_ */