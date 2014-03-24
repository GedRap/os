/*
 * Implementation of Timer module
 * Uses AVR clock 1 which is available on all Arduinos
 * Please be aware that Arduino Servo library uses clock 1.
 * Some inspiration used from Arduino TimerOne library, for prescaling in particular,
 * Which is distributed under GNU General Public License and can be
 * Obtained at http://code.google.com/p/arduino-timerone/
 */ 

#include "timer.h"

//timer duration in microseconds, initial value of 0.1ms
unsigned int os_timer_microseconds = 100;
//bit mask for enabling timer
unsigned char os_timer_enabled_bits;

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	os_multitasking_isr();
}

void os_timer_init() {
	//Clear timer register and set appropriate mode
	TCCR1A = 0;
	TCCR1B = _BV(WGM13);
		  
	//Set timer period and enable prescaling
	long cycles = (F_CPU / 2000000) * os_timer_microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
	if(cycles < TIMER1_RESOLUTION)              os_timer_enabled_bits = _BV(CS10);              // no prescale, full xtal
	else if((cycles >>= 3) < TIMER1_RESOLUTION) os_timer_enabled_bits = _BV(CS11);              // prescale by /8
	else if((cycles >>= 3) < TIMER1_RESOLUTION) os_timer_enabled_bits = _BV(CS11) | _BV(CS10);  // prescale by /64
	else if((cycles >>= 2) < TIMER1_RESOLUTION) os_timer_enabled_bits = _BV(CS12);              // prescale by /256
	else if((cycles >>= 2) < TIMER1_RESOLUTION) os_timer_enabled_bits = _BV(CS12) | _BV(CS10);  // prescale by /1024
	else        cycles = TIMER1_RESOLUTION - 1, os_timer_enabled_bits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

	unsigned char oldSREG = SREG;
	// Disable interrupts for 16 bit register access
	cli();
	ICR1 = cycles;
	SREG = oldSREG;
	
	// Reset clock select registers and apply the correct bitmask
	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
	TCCR1B |= os_timer_enabled_bits;
	
	// enable global interrupts:
	sei();
}

// Start the timer and enable interrupts
void os_timer_start() {
	//Raise interrupt on overflow
	TIMSK1 = _BV(TOIE1);
}

// Stop the timer
// Clears all timer enable bits
void os_timer_stop() {
	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
}