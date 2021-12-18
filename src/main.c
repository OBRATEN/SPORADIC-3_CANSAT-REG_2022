#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"
#include "uart.c"
#include "ds18b20.h"
#include "ds18b20.c"
#include "adxl345.h"
#include "adxl345.c"

volatile uint32_t packageId = 0;

FILE uart_std = FDEV_SETUP_STREAM(UART_writeChar, NULL, _FDEV_SETUP_WRITE);

static inline void blink(void) {
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
}

int main(void) {
  UART_init(51);
  stdout = &uart_std;
  char buff[128];
  float x, y, z;
  DS_init();
  Axel_init();
  if (!(Axel_begin(0x1D))) Axel_begin(0x53);
  while (1) {
    Axel_readXYZ(&x, &y, &z);
    DS_readTemp(buff);
    printf("ATmega128a:%s|%f|%f|%f|%ld;\n", buff, x, y, z, packageId);
    blink();
    packageId++;
  }
}
