#define F_CPU 8000000
#define FOSC  8000000UL

#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD * 16UL))) - 1)

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

//#include "interface/uart.h"
//#include "interface/adc.h"
// #include "spi.h"
//#include "modules/ds18b20.h"
#include "modules/adxl345.h"
//#include "modules/bmp280.h"

//#include "interface/uart.cpp"
//#include "interface/adc.cpp"
//#include "spi.cpp"
//#include "modules/ds18b20.cpp"
#include "modules/adxl345.cpp"
//#include "modules/bmp280.cpp"

#define LEDPIN PING3

bool blink(int16_t temp) {
  if (temp <= 0) return 0;
  /*
  PORTG |= _BV(LEDPIN);
  _delay_ms(1000);
  PORTG &= ~_BV(LEDPIN);
  _delay_ms(1000);
  */
  for (int i = 0; i < temp; i++) {
    PORTG |= _BV(LEDPIN);
    _delay_ms(200);
    PORTG &= ~_BV(LEDPIN);
    _delay_ms(200);
  } return 1;
}

int main(void) {
  float x =0; float y = 0; float z = 0;
  float lx = 0; float ly = 0; float lz = 0;
  DDRG |= _BV(PING3);
  ADXL345_self adxl;
  if (adxl.begin(0x1d)) blink(1);
  else if (adxl.begin(0x53)) blink(1);
  else blink(3);
  while (1) {
    adxl.readXYZ(&x, &y, &z);
    if (abs(x - lx) > 1 || abs(y - ly) > 1 || abs(z - lz) > 1) blink(1);
    lx = x; ly = y; lz = z;
  }
}
