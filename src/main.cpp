#define F_CPU 8000000UL
#define F_SCL 400000000UL

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>

//#include "uart.h"
//#include "uart.c"

#include "ds18b20.cpp"
#include "adxl345.cpp"
#include "bmp280.cpp"
#include "nrf24l01.cpp"

//FILE uart_std = FDEV_SETUP_STREAM(UART_writeChar, NULL, _FDEV_SETUP_WRITE);

static uint32_t packageId {0};
static int32_t time;

static uint8_t testPackage[] = {0, 1, 2, 3, 4, 5};

static uint8_t ERROR {false};
static float DStemp;
static float aX, aY, aZ;
static float press, alt;
static float zeroAlt;
static int32_t BMPtemp;

static inline void blink(void) {
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
}

void calibrateAltitude(BMP_self* bmp, uint8_t times) {
  for (uint8_t i = 0; i < 20; i++) {
    bmp->getData(&BMPtemp, &press, &alt);
    zeroAlt += alt;
  } zeroAlt /= times;
}

int main(void) {
  UART_init(52);
  I2C_init(F_CPU, F_SCL);
  SPI_init();
  ADXL_self adxl;
  BMP_self bmp;
  DS18_term ds;
  nRF_self nrf;
  adxl.begin(0x1D);
  bmp.begin(0x77);
  nrf.begin();
  calibrateAltitude(&bmp, 20);
  while (!ERROR) {
    ds.readTemp(&DStemp);
    adxl.readXYZ(&aX, &aY, &aZ);
    bmp.getData(&BMPtemp, &press, &alt);
    packageId++;
    time++;
    nrf.sendMessage(testPackage, sizeof(testPackage) / sizeof(testPackage[0]));
  } return 0;
}
