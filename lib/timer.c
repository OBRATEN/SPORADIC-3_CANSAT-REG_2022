#include "timer.h"

ISR(TIMER1_COMPA_vect) {
  timer_millis++;
}

void TIMER_init(uint32_t f_cpu) {
  uint32_t ctc = ((f_cpu / 1000) / 8);
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  OCR1AH = (ctc >> 8);
  OCR1AL = ctc;
  TIMSK |= (1 << OCIE1A);
}

uint32_t TIMER_millis(void) {
  uint32_t result;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    result = timer_millis;
  } return result;
}
