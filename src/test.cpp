#define F_CPU 8000000UL
#define F_SCL 400000UL
#define UART_BAUD 9600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "nrf24l01.hpp"

int main(void) {
  DDRG |= (1 << PING3);
  DDRB |= (1 << PINB4);
  NRF nrf;
  nrf.init();
  char data[32];
  memset(data, 1, 32);
  while (1) {
    nrf.send(data);
    PORTG |= (1 << PING3);
    _delay_ms(100);
    PORTG &= ~(1 << PING3);
    _delay_ms(100); 
  }
}