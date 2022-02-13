#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#include "uart.h"
#include "uart.c"
/*
#include "ds18b20.h"
#include "ds18b20.c"
#include "adxl345.h"
#include "adxl345.c"
#include "bmp280.h"
#include "bmp280.c"
#include "timer.h"
#include "timer.c"
*/
#include "nrf2401.h"
#include "nrf2401.c"

//static uint32_t packageId = 0;
//static uint32_t timeFromStart = 0;

FILE uart_std = FDEV_SETUP_STREAM(UART_writeChar, NULL, _FDEV_SETUP_WRITE);

static inline void blink(void) {
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
}

int main(void) {
  //UART_init(51);
  //stdout = &uart_std;
  I2C_init(F_CPU, F_SCL);
  //TIMER_init(F_CPU);
  nRF_init();
  nRF_start();
  /*
  float x = 0, y = 0, z = 0;
  float press = 0, alt = 0, temp0 = 0;
  int32_t temp1 = 0;
  float zeroAlt = 0;
  DS_init();
  if (!(Axel_begin(0x1D))) Axel_begin(0x53);
  if (!(BMP_begin(0x77))) BMP_begin(0x76);
  for (uint8_t i = 0; i < 20; i++) {
    BMP_getData(&temp1, &press, &alt);
    zeroAlt += alt;
  } zeroAlt /= 20;*/
  uint8_t message[32];
  for (uint8_t i = 0; i < 32; i++) message[i] = i;
  while (1) {
    //printf("a");
    //if (UART_writeCharA('a')) blink();
    nRF_sendMessage(message, 32);
    blink();
    _delay_ms(5000);
    /*DS_readTemp(&temp0);
    Axel_readXYZ(&x, &y, &z);
    BMP_getData(&temp1, &press, &alt);
    printf("ATmega128a:%f|%ld.%.2ld|%f|%f|%f|%.4f|%.4f|%ld|%ld;\n",
           temp0, temp1 / 100, temp1 % 100, x, y, z,
           press, alt - zeroAlt, packageId, timeFromStart);
    packageId++;*/
  }
}
