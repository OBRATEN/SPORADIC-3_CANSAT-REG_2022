#ifndef BMP_H
#define BMP_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#ifndef F_SCL
#define F_SCL 100000
#endif

/* Библиотека связи с баротермометром BMP280
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Интерфейс: I2C
 * Зависимости: avr, i2c
 */

#include <avr/io.h>
#include <util/delay.h>
#include "i2c.hpp"

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
#define BMP_MEAS (BMP_OVSAMP_T2 << 5) | (BMP_OVSAMP_T16 << 2) | (BMP_FORCE)

class BMP_press {
public:
  uint8_t begin(uint8_t addr, uint8_t I2C_inited);
  void writeReg1B(uint8_t addr, uint8_t data);
  uint8_t readReg1B(uint8_t addr);
  uint16_t readReg2B(uint8_t addr);
  uint16_t readReg2BLE(uint8_t addr);
  int16_t readRegS2BLE(uint8_t addr);
  uint32_t readReg3B(uint8_t addr);
  void readCalData(void);
  int32_t readTemp(void);
  void readFloatPress(int32_t *temp, float *press);
  float readAlt(float *press);
  uint8_t getData(int32_t *temp, float *press, float *alt);
private:
  uint8_t devAddr;
  uint8_t t1, p1;
  int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9;
  int32_t t_fine;
  I2C_interface _i2c;
};

#endif
