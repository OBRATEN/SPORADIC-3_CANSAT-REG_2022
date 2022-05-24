#include "timer.h"

volatile unsigned long timer1_millis;

ISR(TIMER1_COMPA_vect) {
  timer1_millis++;
}

void Timer_init(unsigned long f_cpu) {
  unsigned long ctc_match_overflow = ((f_cpu / 1000) / 8);
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  OCR1AH = (ctc_match_overflow >> 8);
  OCR1AL = ctc_match_overflow;
  TIMSK |= (1 << OCIE1A);
}

unsigned long millis (void) {
  unsigned long millis_return;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer1_millis;
  } return millis_return;
}