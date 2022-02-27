#define F_CPU 8000000UL
#define F_SCL 400000UL

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"
#include "onewire.hpp"
#include "i2c.hpp"
#include "ds18b20.hpp"
#include "adxl345.hpp"
#include "bmp280.hpp"
#include "nrf24l01.hpp"

#define DS_TIME_INTERVAL 750

inline void blink(void) {
  cli();
  PORTG |= _BV(PING3);
  _delay_ms(100);
  PORTG &= ~_BV(PING3);
  _delay_ms(100);
  sei();
}

static uint32_t globalTime = 0;
static uint32_t packageID = 0;

static uint32_t DS_time_mark = DS_TIME_INTERVAL;

static float aX = 0;
static float aY = 0;
static float aZ = 0;

static float DStemp = 0;
static int32_t BMPtemp = 0;
static float press = 0;
static float alt = 0;
static float zeroAlt = 0;

static char testMessage[12] = "12345678901";

void calibrateAltitude(BMP_press* bmp, uint8_t times) {
  for (uint8_t i = 0; i < 20; i++) {
    bmp->getData(&BMPtemp, &press, &alt);
    zeroAlt += alt;
  } zeroAlt /= times;
}

void initSensors(ADXL_gyro* adxl, BMP_press* bmp) {
  if (!(adxl->begin(0x1d, 0))) adxl->begin(0x53, 0);
  if (!(bmp->begin(0x77, 0))) bmp->begin(0x76, 0);
}

int main(void) {
  cli();
  DS18_term ds;
  ADXL_gyro adxl;
  BMP_press bmp;
  Timer_init(F_CPU);
  initSensors(&adxl, &bmp);
  //nRF_radio nrf;
  //nrf.begin(0);
  sei();
  for (;;) {
    float oldDStemp = DStemp;
    float oldAX = aX;
    float oldPress = press;

    if (millis() > DS_time_mark) {
      ds.readTemp(&DStemp);
      DS_time_mark = millis() + DS_TIME_INTERVAL;
    }
    adxl.readXYZ(&aX, &aY, &aZ);
    bmp.getData(&BMPtemp, &press, &alt);

    if (DStemp != oldDStemp) blink();
    if (aX != oldAX) blink();
    if (press != oldPress) blink();
    packageID++;
    globalTime = millis();
    //nrf.sendMessage(testMessage, 12);
  }
  return 0;
}
