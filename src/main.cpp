#include "config.h"
#include "pinout.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"
#include "uart.h"
#include "i2c.hpp"
#include "ds18b20.hpp"
#include "adxl345.hpp"
#include "nrf24l01.hpp"
#include "bmp280.hpp"
#include "sd.hpp"

// Зашиваем строку с названием команды ради прикола
const char __attribute__((__progmem__)) team[11] = "SPORADIC-3";

// Переменные меток времени
static uint32_t NRF_time_mark  = NRF_TIME_INTERVAL;
static uint32_t DS_time_mark   = DS_TIME_INTERVAL;
static uint32_t BMP_time_mark  = BMP_TIME_INTERVAL;
static uint32_t ADXL_time_mark = ADXL_TIME_INTERVAL;
static uint32_t SD_time_mark   = SD_TIME_INTERVAL;

// Объекты классов переферии
nRF_radio nrf  = nRF_radio();
DS18_term ds   = DS18_term();
ADXL_gyro adxl = ADXL_gyro();
BMP_press bmp  = BMP_press();
SD_card sd     = SD_card();

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
  char lat[12];
  char lon[12];
  char alt[6];
  uint8_t status = 0;
} gps_data;

// Вспомогательные переменные
static const uint8_t tx_size = sizeof(tx_data);
static uint8_t globalStatus = 0;
static float zeroAlt = 0;
static char gpsTitleBuf[5];
static int8_t parserState = -1;
static uint8_t gpsDataReceived = 0;
static uint8_t commaPassed = 0;
static uint8_t latIt = 0;
static uint8_t lonIt = 0;
static uint8_t altIt = 0;

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
  _delay_ms(100 * (time / 2));
  ONCHIPLED_PORT &= ~(1 << ONCHIPLED_PIN);
  _delay_ms(100 * (time / 2));
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
    memset(gps_data.lat, 0, 12);
    memset(gps_data.lon, 0, 12);
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
  tx_data.status = globalStatus;
  tx_data.packageID++;
}

// Прерывание по получению данных UART1
ISR(USART1_RX_vect) {
  parse((unsigned char)UDR1);
  globalStatus |= (1 << STATUS_BN);
}

// Функции инициализации переферии
inline void initPins(void) {
  ONCHIPLED_DDR |= (1 << ONCHIPLED_PIN);
  RGB_DDR |= (1 << RGB_RED) | (1 << RGB_GREEN) | (1 << RGB_BLUE);
}

inline void initSensors(void) {
  if (!(adxl.init(AXEL_DEF_ADDR))) if (!adxl.init(AXEL_ALT_ADDR)) globalStatus &= ~(1 << STATUS_I2C);
  if (!(bmp.init(BMP_DEF_ADDR)))   if (!bmp.init(BMP_ALT_ADDR))   globalStatus &= ~(1 << STATUS_I2C);
  if (!(sd.init()))  globalStatus &= ~(1 << STATUS_SPI);
  if (!(nrf.init())) globalStatus &= ~(1 << STATUS_SPI); 
  ds.init();
  calibrateAltitude(10);
}

inline void initInterfaces(void) {
  UART_init(F_CPU / 16 / UART_BAUD - 1, 1);
  I2C_interface i2c = I2C_interface();
  SPI_interface spi = SPI_interface();
  spi.init();
  i2c.init(F_CPU, F_SCL);
}

int main(void) {
  cli();
  initPins();
  initInterfaces();
  initSensors();
  Timer_init(F_CPU);
  sei();
  for (;;) {
    if ((globalStatus & 0b11111111) != STATUS_CHECK) {
      setRGB(1, 0, 0);
      initPins();
      initInterfaces();
      initSensors();
      continue;
    } else setRGB(0, 1, 0);
    if (millis() - NRF_time_mark > NRF_TIME_INTERVAL) {
      if (gpsDataReceived != 2) {
        packData();
        nrf.send(&tx_data);
      } else nrf.send(&gps_data);
      NRF_time_mark = millis();
      globalStatus = STATUS_CHECK;
    }
    if (millis() - DS_time_mark > DS_TIME_INTERVAL) {
      ds.readTemp(&rec_data.DStemp);
      DS_time_mark = millis();
      globalStatus |= (1 << STATUS_DS);
    }
    if (millis() - BMP_time_mark > BMP_TIME_INTERVAL) {
      bmp.getData(&rec_data.BMPtemp, &rec_data.press, &rec_data.alt);
      BMP_time_mark = millis();
      globalStatus |= (1 << STATUS_BMP);
    }
    if (millis() - ADXL_time_mark > ADXL_TIME_INTERVAL) {
      adxl.readXYZ(&rec_data.aX, &rec_data.aY, &rec_data.aZ);
      ADXL_time_mark = millis();
      globalStatus |= (1 << STATUS_ADXL);
    }
    if (millis() - SD_time_mark > SD_TIME_INTERVAL) {
      sd.writeData(&tx_data, tx_size);
      globalStatus |= (1 << STATUS_SD);
    }
  } return 0;
}