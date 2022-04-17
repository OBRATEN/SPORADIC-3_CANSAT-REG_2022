#ifndef TIMER_H
#define TIMER_H

/* Библиотека управления таймером ATMega128a
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Зависимости: avr
 * Является реализацией millis() Arduino.
 * Настройка велась на счётчик со сбросом по переполнению на 1ms.
 * Тактовая частота тестирующего микроконтроллера (F_CPU) - 8MHz (8000000UL)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

ISR(TIMER1_COMA_vect);

void Timer_init(uint32_t f_cpu);
uint32_t millis(void);

#endif
