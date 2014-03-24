/*
 * Header file for Timer module
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

//Clock 1 has resolution of 65536
#define TIMER1_RESOLUTION 65536
//Atmel 328P frequency is 16MHz
#define F_CPU 16000000

//Forward declaration of ISR
 __attribute__ ( ( signal ) ) void os_multitasking_isr();
 
 void os_timer_init();
 void os_timer_start();
 void os_timer_stop();

#endif /* TIMER_H_ */