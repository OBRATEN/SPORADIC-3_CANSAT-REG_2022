#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

class I2C_self {
public:
	I2C_self(void);
	void init(uint32_t f_cpu, uint32_t f_scl);
	void close(void);
	void start(void);
	void stop(void);
	uint8_t writeByte(uint8_t data);
	uint8_t readByte(uint8_t *data, uint8_t opt);
	void writeReg(uint8_t address, uint8_t reg, uint8_t val);
	void readBytes(uint8_t daddr, uint8_t raddr, uint8_t num, uint8_t *buffer);
	bool getCapacity(void);
private:
	bool m_capacity;
};

#endif