#ifndef BMP_H
#define BMP_H

/* Библиотека связи с баротермометром BMP280
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: I2C
 * Зависимости: avr, i2c
 */

#include "config.h"
#include "i2c.hpp"

#include <util/delay.h>

#define BMP_T1REG 0x88
#define BMP_T2REG 0x8A
#define BMP_T3REG 0x8C
#define BMP_P1REG 0x8E
#define BMP_P2REG 0x90
#define BMP_P3REG 0x92
#define BMP_P4REG 0x94
#define BMP_P5REG 0x96
#define BMP_P6REG 0x98
#define BMP_P7REG 0x9A
#define BMP_P8REG 0x9C
#define BMP_P9REG 0x9E

#define BMP_CHIPIDREG 0xD0
#define BMP_CHIPIDVAL 0x58

#define BMP_TEMPDATAREG 0xFA
#define BMP_PRESDATAREG 0xF7
#define BMP_CFGREG 0xF5
#define BMP_CTRLREG 0xF4

#define BMP_OVSAMP_T2 0b10
#define BMP_OVSAMP_T16 0b101
#define BMP_FORCE 0b01

#define BMP_TSB250 0b11
#define BMP_FILTC4 0b10
#define BMP_NOSPI 0b00

#define BMP_CFG  (BMP_TSB250 << 5)    | (BMP_FILTC4 << 2)     | (BMP_NOSPI)
#define BMP_MEAS 0xFD

class BMP_press {
public:
  BMP_press(void);
  uint8_t init(uint8_t addr);
  void writeReg(uint8_t addr, uint8_t data);
  uint8_t readReg(uint8_t addr);
  uint16_t read16_LE(uint8_t reg);
  int32_t readReg24b(uint8_t addr);
  void readCalData(void);
  double readTemp(void);
  void readFloatPress(double *temp, double *press);
  void readAlt(double *press, double *alt);
  uint8_t getData(double *temp, double *press, double *alt);
private:
  uint8_t devAddr;
  uint16_t t1, p1 = 0;
  int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9 = 0;
  int32_t t_fine;
  I2C_interface _i2c;
};

#endif
