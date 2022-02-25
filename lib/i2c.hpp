#ifndef I2C_H
#define I2C_H

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

/*
void I2C_init(uint32_t f_cpu, uint32_t f_scl);
void I2C_start(void);
void I2C_stop(void);
void I2C_close(void);
uint8_t I2C_writeByte(uint8_t data);
uint8_t I2C_readByte(uint8_t *data, uint8_t opt);
void I2C_writeReg(uint8_t addr, uint8_t reg, uint8_t val);
void I2C_readReg(uint8_t devAddr, uint8_t regAddr, uint8_t num, uint8_t *buf);
*/

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
protected:
	uint8_t _inited = 0;
	uint8_t _started = 0;
	uint32_t _f_cpu;
	uint32_t _f_scl;
};

#endif
