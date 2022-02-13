#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

volatile uint32_t timer_millis;

ISR(TIMER1_COMPA_vect);
void TIMER_init(uint32_t f_cpu);
uint32_t TIMER_millis(void);

#endif
