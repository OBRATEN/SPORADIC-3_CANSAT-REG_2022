#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <util/delay.h>

void I2C_init(uint32_t f_cpu, uint32_t f_scl);
void I2C_start(void);
void I2C_stop(void);
void I2C_close(void);

class I2C_interface {
public:
  void start(void);
  void stop(void);
  void close(void);
  uint8_t writeByte(uint8_t data);
  uint8_t readByte(uint8_t *data, uint8_t opt);
  void writeReg(uint8_t addr, uint8_t reg, uint8_t val);
  void readReg(uint8_t devAddr, uint8_t regAddr, uint8_t num, uint8_t *buf);
private:
  uint8_t _devAddr;
};

#endif
