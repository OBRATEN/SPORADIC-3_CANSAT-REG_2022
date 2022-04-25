#define F_CPU 8000000UL
#define F_SCL 400000UL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"
#include "uart.h"
#include "ds18b20.hpp"
#include "adxl345.hpp"
#include "bmp280.hpp"
#include "nrf24l01.hpp"
#include "fat.hpp"


#define DS_TIME_INTERVAL 750
#define NRF_TIME_INTERVAL 1

inline void blink(uint8_t times) {
  cli();
  for (uint8_t i = 0; i < times; i++) {
    PORTG |= (1 << PING3);
    _delay_ms(100);
    PORTG &= ~(1 << PING3);
    _delay_ms(100);
  }
  sei();
}

inline void light(uint8_t time) {
  cli();
  PORTG |= (1 << PING3);
  _delay_ms(100 * time);
  PORTG &= ~(1 << PING3);
  sei();
}

static uint32_t globalTime = 0;
static uint32_t packageID = 0;
static uint8_t mode = 0;

//static uint32_t DS_time_mark = DS_TIME_INTERVAL;
static uint32_t NRF_time_mark = NRF_TIME_INTERVAL;

/*
static float aX = 0;
static float aY = 0;
static float aZ = 0;

static float DStemp = 0;
static int32_t BMPtemp = 0;
static float press = 0;
static float alt = 0;
static float zeroAlt = 0;
uint8_t rxDataPtr;
*/

static uint8_t testMessage[32];

/*
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
*/

/*ISR(USART1_RXC_vect) {
  blink();
  printf("%s", UDR0);
}*/

int main(void) {
  cli();
  DDRG |= (1 << PING3);
  //UART_init(53, 1);
  //uart_str = fdevopen(UART_writeChar1, UART_getChar1);
  //stdout = uart_str;
  //DS18_term ds;
  //ADXL_gyro adxl;
  //BMP_press bmp;
  Timer_init(F_CPU);
  //initSensors(&adxl, &bmp);
  nRF_radio nrf;
  nrf.begin();
  memset(testMessage, 't', 32);
  blink(3);
  sei();
  if (mode == 0) {
    for (;;) {
      /*
      if (millis() > DS_time_mark) {
        ds.readTemp(&DStemp);
        DS_time_mark = millis() + DS_TIME_INTERVAL;
      }
      adxl.readXYZ(&aX, &aY, &aZ);
      bmp.getData(&BMPtemp, &press, &alt);
      //printf("asdasd");
      */
      if (millis() > NRF_time_mark) {
        if (nrf.send(testMessage, 32)) blink(1);
        NRF_time_mark = millis() + NRF_TIME_INTERVAL;
      }
      packageID++;
      globalTime = millis();
    }
  } else if (mode == 1) {
    for (;;) {
      if (nrf.checkW()) blink(1);
      _delay_ms(500);
      if (nrf.checkR()) blink(1);
      _delay_ms(500);
      if (nrf.checkRW()) blink(1);
      _delay_ms(500);
      light(10);
    }
  }
  return 0;
}
