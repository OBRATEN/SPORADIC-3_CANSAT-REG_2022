#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <util/delay.h>

void I2C_init(uint32_t f_cpu, uint32_t f_scl);
void I2C_close(void);
void I2C_start(void);
void I2C_stop(void);
uint8_t I2C_writeByte(uint8_t data);
uint8_t I2C_readByte(uint8_t *data, uint8_t opt);
void I2C_writeReg(uint8_t address, uint8_t reg, uint8_t val);
void I2C_readBytes(uint8_t daddr, uint8_t raddr, uint8_t num, uint8_t *buff);

#endif
