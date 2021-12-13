#include <avr/io.h>
#include "adc.h"

ADC_self::ADC_self(void) {
  ADMUX  = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  m_capacity = true;
}

uint8_t ADC_self::getData(uint8_t pin) {
  pin &= 0x7;
  ADMUX = (ADMUX & 0xF8) | pin;
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  return ADC;
}

bool ADC_self::getCapacity(void) {
  return m_capacity;
}
