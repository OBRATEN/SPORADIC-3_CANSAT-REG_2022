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
#include "bmp280.h"
#include "bmp280.c"
#include "timer.h"
#include "timer.c"

static uint32_t packageId = 0;
static uint32_t timeFromStart = 0;

FILE uart_std = FDEV_SETUP_STREAM(UART_writeChar, NULL, _FDEV_SETUP_WRITE);

static inline void blink(void) {
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
}

int main(void) {
  UART_init(51);
  I2C_init(F_CPU, F_SCL);
  stdout = &uart_std;
  float x, y, z;
  float press, alt, temp0;
  int32_t temp1;
  float zeroAlt = 0;
  DS_init();
  if (!(Axel_begin(0x1D))) Axel_begin(0x53);
  if (!(BMP_begin(0x77))) BMP_begin(0x76);
  for (uint8_t i = 0; i < 20; i++) {
    BMP_getData(&temp1, &press, &alt);
    zeroAlt += alt;
  } zeroAlt /= 20;
  while (1) {
    DS_readTemp(&temp0);
    Axel_readXYZ(&x, &y, &z);
    BMP_getData(&temp1, &press, &alt);
    printf("ATmega128a:%f|%ld.%.2ld|%f|%f|%f|%.4f|%.4f|%ld|%ld|%ld;\n",
           temp0, temp1 / 100, temp1 % 100, x, y, z,
           press, alt - zeroAlt, TIMER_millis(), packageId, timeFromStart);
    blink();
    packageId++;
  }
}
