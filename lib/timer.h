#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

extern volatile uint32_t timer1_millis;

ISR(TIMER1_COMPA_vect);
void Timer_init(unsigned long f_cpu);
unsigned long millis(void);

#endif // TIMER_H