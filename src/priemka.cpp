#define F_CPU 8000000UL
#define FOSC  8000000UL

#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD * 16UL))) - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "interf/uart.h"
#include "interf/adc.h"
#include "interf/i2c.h"
// #include "spi.h"
#include "modules/ds18b20.h"

#include "interf/uart.cpp"
#include "interf/adc.cpp"
#include "interf/i2c.cpp"
// #include "spi.cpp"
#include "modules/ds18b20.cpp"

#define LEDPIN PING3

void blink(void) {
  PORTG |= _BV(LEDPIN);
  _delay_ms(100);
  PORTG &= ~_BV(LEDPIN);
  _delay_ms(100);
}

int main(void) {
  DDRG |= _BV(PING3);
  UART_self uart(MYUBRR);
  Thermo term;
  int16_t temp = 0;
  while (1) {
    term.startConvert();
    term.readTemp(&temp);
    if (temp > 100) blink();
    uart.writeChar(temp);
    uart.writeChar('\n');
    _delay_ms(1000);
  }
}