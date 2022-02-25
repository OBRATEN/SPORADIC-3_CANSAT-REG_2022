#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

ISR(TIMER1_COMA_vect);

void Timer_init(uint32_t f_cpu);
uint32_t millis(void);

#endif
