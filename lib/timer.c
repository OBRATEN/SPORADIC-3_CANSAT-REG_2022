#include "timer.h"

volatile uint32_t _millis = 0;

ISR(TIMER1_COMPA_vect) {
  _millis++;
}

void Timer_init(uint32_t f_cpu) {
  TCCR1B = (1 << WGM12);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  uint32_t overflow = ((f_cpu / 64) / 1000);
  OCR1AH = (overflow >> 8);
  OCR1AL = overflow;
  TIMSK |= (1<<OCIE1A);
}

uint32_t millis(void) {
  uint32_t response;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    response = _millis;
  }
  return response;
}