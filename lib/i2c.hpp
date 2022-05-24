#ifndef I2C_H
#define I2C_H

#define F_CPU 8000000UL

/* Библиотека управления интерфейсом TWI (I2C)
 * Автор: Гарагуля Артур, "SPORADIC", г. Курск
 * Зависимости: avr
 */

#include <avr/io.h>
#include <util/delay.h>

class I2C_interface {
public:
  void init(uint32_t f_cpu, uint32_t f_scl);
  uint8_t inited(void);
  uint8_t start(void);
  uint8_t stop(void);
  uint8_t close(void);
  uint8_t writeByte(uint8_t data);
  uint8_t readByte(uint8_t *data, uint8_t opt);
  void writeReg(uint8_t addr, uint8_t reg, uint8_t val);
  void readReg(uint8_t devAddr, uint8_t regAddr, uint8_t num, uint8_t *buf);
  uint64_t readReg(uint8_t devAddr, uint8_t addr, uint8_t len);
  uint8_t read(uint8_t ack);
private:
  uint8_t _inited = 0;
  uint8_t _started = 0;
  uint32_t _f_cpu;
  uint32_t _f_scl;
};

#endif