#define F_CPU 8000000UL
#define F_SCL 400000UL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"
#include "onewire.hpp"
#include "uart.h"
#include "i2c.hpp"
#include "ds18b20.hpp"
#include "adxl345.hpp"
#include "bmp280.hpp"
#include "nrf24l01.hpp"

#define DS_TIME_INTERVAL 750
#define NRF_TIME_INTERVAL 80

inline void blink(uint8_t times) {
  cli();
  for (uint8_t i = 0; i < times; i++) {
    PORTB |= _BV(PINB6);
    _delay_ms(100);
    PORTB &= ~_BV(PINB6);
    _delay_ms(100);
  } sei();
}

static uint32_t globalTime = 0;
static uint32_t packageID = 0;

static uint32_t DS_time_mark = DS_TIME_INTERVAL;
static uint32_t NRF_time_mark = NRF_TIME_INTERVAL;

static float aX = 0;
static float aY = 0;
static float aZ = 0;

static float DStemp = 0;
static int32_t BMPtemp = 0;
static float press = 0;
static float alt = 0;
static float zeroAlt = 0;

char testMessage[32];

uint8_t rxDataPtr;

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

/*ISR(USART1_RXC_vect) {
  blink();
  printf("%s", UDR0);
}*/

int main(void) {
  cli();
  DDRB |= (1 << PINB6);
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
  for (uint8_t c = 0; c < 32; c++) testMessage[c] = 'a';
  blink(3);
  sei();
  for (;;) {
    PORTB |= _BV(PINB6);
    _delay_ms(100);
    PORTB &= ~_BV(PINB6);
    _delay_ms(100);
  }
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
      nrf.sendMessage(testMessage, 32);
      NRF_time_mark = millis() + NRF_TIME_INTERVAL;
    }
    packageID++;
    globalTime = millis();
    blink(1);
  }
  return 0;
}
