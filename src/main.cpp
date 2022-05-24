#define F_CPU 8000000UL
#define F_SCL 400000UL
#define UART_BAUD 9600

#define TESTING_MODE 2

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "timer.h"
#include "uart.h"
#include "i2c.hpp"
#include "ds18b20.hpp"
#include "adxl345.hpp"
#include "nrf24l01.hpp"
#include "bmp280.hpp"
#include "sd.hpp"

// Макросы пинов управления светодиодами
#define ONCHIPLED_PIN  PING3
#define ONCHIPLED_DDR  DDRG
#define ONCHIPLED_PORT PORTG

#define RGB_DDR   DDRA
#define RGB_PORT  PORTA
#define RGB_RED   PINA4
#define RGB_GREEN PINA3
#define RGB_BLUE  PINA5

// Интервалы между выполнением задач в мс
#define DS_TIME_INTERVAL    770
#define NRF_TIME_INTERVAL   10
#define BMP_TIME_INTERVAL   50
#define ADXL_TIME_INTERVAL  25
#define SD_TIME_INTERVAL    100
#define SD_SECTORS          3903488 // Индивидуально для карты

// Зашиваем строку с названием команды ради прикола
const char __attribute__((__progmem__)) team[11] = "SPORADIC-3";

// Переменные меток времени
static uint32_t NRF_time_mark = NRF_TIME_INTERVAL;
static uint32_t DS_time_mark = DS_TIME_INTERVAL;
static uint32_t BMP_time_mark = BMP_TIME_INTERVAL;
static uint32_t ADXL_time_mark = ADXL_TIME_INTERVAL;
static uint32_t SD_time_mark = SD_TIME_INTERVAL;

// Объекты классов переферии
NRF nrf = NRF();
DS18_term ds = DS18_term();
ADXL_gyro adxl = ADXL_gyro();
BMP_press bmp = BMP_press();
SD_card sd = SD_card();

// Структура собранных данных
struct recData {
  float aX = 0;
  float aY = 0;
  float aZ = 0;

  float DStemp = 0;
  double BMPtemp = 0;
  double press = 0;
  double alt = 0;
} rec_data;

// Структура данных датчиков на передачу
struct txData {
  uint8_t prefix = 0;
  int16_t aX = 0;
  int16_t aY = 0;
  int16_t aZ = 0;
  int16_t DStemp = 0;
  int16_t BMPtemp = 0;
  int32_t press = 0;
  int16_t alt = 0;
  uint32_t packageID = 0;
  uint32_t gTime = 0;
  uint8_t status = 0;
} tx_data;

// Структура данных GPS
struct gpsData {
  uint8_t prefix = 1;
  char lat[12];
  char lon[12];
  char alt[6];
} gps_data;

// Переменные статуса
static uint8_t i2cW = 1;
static uint8_t spiW = 1;
static uint8_t adxlW = 0;
static uint8_t dsW = 0;
static uint8_t bmpW = 0;
static uint8_t bnW = 0;
static uint8_t sdW = 0;

// Вспомогательные переменные
const uint8_t tx_size = sizeof(tx_data);
static float zeroAlt = 0;
static char gpsTitleBuf[5];
static int8_t parserState = -1;
static uint8_t gpsDataReceived = 0;
static uint8_t commaPassed = 0;
static uint8_t latIt = 0;
static uint8_t lonIt = 0;
static uint8_t altIt = 0;
static uint32_t sd_curSec = 0x00000000;
static uint8_t sd_buff[512];
static uint16_t sd_bytesInSec = 0;

// Функции работы со светодиодами
inline void blink(uint8_t times) {
  cli();
  for (uint8_t i = 0; i < times; i++) {
    ONCHIPLED_PORT |= (1 << ONCHIPLED_PIN);
    _delay_ms(100);
    ONCHIPLED_PORT &= ~(1 << ONCHIPLED_PIN);
    _delay_ms(100);
  }
  sei();
}

inline void light(uint8_t time) {
  cli();
  ONCHIPLED_PORT |= (1 << ONCHIPLED_PIN);
  _delay_ms(100 * time);
  ONCHIPLED_PORT &= ~(1 << ONCHIPLED_PIN);
  sei();
}

__attribute__((always_inline)) inline void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  RGB_PORT = (r << RGB_RED) | (g << RGB_GREEN) | (b << RGB_BLUE);
}

void calibrateAltitude(uint8_t times) {
  for (uint8_t i = 0; i < 20; i++) {
    bmp.getData(&rec_data.BMPtemp, &rec_data.press, &rec_data.alt);
    zeroAlt += rec_data.alt;
  } zeroAlt /= times;
}

// Функция посимвольного вычленения данных из GPS
void parse(char data) {
  if (data == '$') {
    parserState = 0;
    gpsDataReceived = 0;
    latIt = 0;
    lonIt = 0;
    altIt = 0;
    commaPassed = 0;
    for (uint8_t i = 0; i < 12; i++) { gps_data.lat[i] = 0; gps_data.lon[i] = 0; }
    return;
  }
  if (parserState == -1) return;
  if (parserState == 0) {
    if (data == ',') {
      if (gpsTitleBuf[0] == 'G' && (gpsTitleBuf[1] == 'N' || gpsTitleBuf[1] == 'P') && gpsTitleBuf[2] == 'G' && gpsTitleBuf[3] == 'G' && gpsTitleBuf[4] == 'A') {
        parserState = 1;
        gpsDataReceived = 1;
        return;
      }
      parserState = -1;
      return;
    }
    gpsTitleBuf[0] = gpsTitleBuf[1];
    gpsTitleBuf[1] = gpsTitleBuf[2];
    gpsTitleBuf[2] = gpsTitleBuf[3];
    gpsTitleBuf[3] = gpsTitleBuf[4];
    gpsTitleBuf[4] = data;
    return;
  }
  if (parserState == 1) {
    if (data == ',') { commaPassed++; return; }
    switch (commaPassed) {
      case 1: gps_data.lat[latIt++] = data; break;
      case 3: gps_data.lon[lonIt++] = data; break;
      case 8: gps_data.alt[altIt++] = data; break;
      default: if (commaPassed > 8) gpsDataReceived = 2; break;
    }
  }
}

// Функция упаковки данных переферии для передачи
void packData(void) {
  tx_data.aX = (int16_t)(rec_data.aX * 100);
  tx_data.aY = (int16_t)(rec_data.aY * 100);
  tx_data.aZ = (int16_t)(rec_data.aZ * 100);
  tx_data.DStemp = (int16_t)(rec_data.DStemp * 100);
  tx_data.BMPtemp = (int16_t)(rec_data.BMPtemp * 100);
  tx_data.press = rec_data.press;
  tx_data.alt = (int16_t)(rec_data.alt * 100);
  tx_data.gTime = millis();
  tx_data.status = (i2cW << 7) | (spiW << 6) | (sdW << 5) | (((gpsDataReceived != 0) ? 1 : 0) << 4) | (adxlW << 3) | (bmpW << 2) | (dsW << 1) | (bnW << 0);
  tx_data.packageID++;
}

// Прерывание по получению данных UART1
ISR(USART1_RX_vect) {
  parse((unsigned char)UDR1);
  bnW = 1;
}

// Функции инициализации переферии
inline void initPins(void) {
  DDRA |= (1 << PINA3);
  RGB_DDR |= (1 << RGB_RED) | (1 << RGB_GREEN) | (1 << RGB_BLUE);
  DDRC |= (1 << PINC0);
}

inline void initSensors(void) {
  if (!(adxl.begin(0x1d))) if (!adxl.begin(0x53)) i2cW = 0;
  if (!(bmp.begin(0x77)))  if (!bmp.begin(0x76))  i2cW = 0;
  spiW = (sd.init() == 0) ? 1 : 0;
  ds.init();
  nrf.begin();
  calibrateAltitude(10);
}

inline void initInterfaces(void) {
  UART_init(F_CPU / 16 / UART_BAUD - 1, 1);
  I2C_interface i2c = I2C_interface();
  i2c.init(F_CPU, F_SCL);
}

int main(void) {
  cli();
  for (uint16_t i = 0; i < 512; i++) sd_buff[i] = 0x00;
  initPins();
  initInterfaces();
  initSensors();
  Timer_init(F_CPU);
  if (!i2cW || !spiW) setRGB(1, 0, 0);
  else setRGB(0, 1, 0);
  sei();
  light(8); // 800ms светодиода
  for (;;) {
    if (!i2cW || !spiW) { setRGB(1, 0, 0); break; }
    else setRGB(0, 1, 0);
    if (millis() - NRF_time_mark > NRF_TIME_INTERVAL) {
      if (gpsDataReceived != 2) {
        packData();
        nrf.send(&tx_data);
      } else nrf.send(&gps_data);
      NRF_time_mark = millis();
      adxlW = 0;
      dsW = 0;
      bmpW = 0;
      bnW = 0;
      sdW = 0;
    }
    if (millis() - DS_time_mark > DS_TIME_INTERVAL) {
      ds.readTemp(&rec_data.DStemp);
      DS_time_mark = millis();
      dsW = 1;
    }
    if (millis() - BMP_time_mark > BMP_TIME_INTERVAL) {
      bmp.getData(&rec_data.BMPtemp, &rec_data.press, &rec_data.alt);
      BMP_time_mark = millis();
      bmpW = 1;
    }
    if (millis() - ADXL_time_mark > ADXL_TIME_INTERVAL) {
      adxl.readXYZ(&rec_data.aX, &rec_data.aY, &rec_data.aZ);
      ADXL_time_mark = millis();
      adxlW = 1;
    }
    if (millis() - SD_time_mark > SD_TIME_INTERVAL) {
      uint8_t temp[tx_size];
      memcpy(temp, &tx_data, tx_size);
      if (sd_bytesInSec < 512) {
        for (uint16_t i = 0; i < tx_size; i++) sd_buff[i + sd_bytesInSec] = temp[i];
        sd_bytesInSec += tx_size;
      } else {
        uint8_t res = sd.writeSingleBlock(sd_curSec, sd_buff);
        sdW = 1;
        for (uint16_t i = 0; i < 512; i++) sd_buff[i] = 0x00;
        sd_bytesInSec = 0;
        sd_curSec += 1;
        if (!res) setRGB(0, 0, 1);
        else setRGB(1, 0, 0);
        _delay_ms(100);
      }
    }
  } return 0;
}