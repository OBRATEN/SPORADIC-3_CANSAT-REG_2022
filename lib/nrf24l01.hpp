#ifndef NRF_H
#define NRF_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "spi.hpp"
#include <util/delay.h>

#define CSN PINB0
#define CE PINB4

#define RADIO_ADDRESS "sprd3"

class nRF_radio {
public:
  void begin(uint8_t SPI_inited);
  uint8_t start(void);
  void writeAddr(char *addr);
  uint8_t writeBuf(uint8_t addr, uint8_t *buf, uint8_t count);
  uint8_t writeReg(uint8_t addr, uint8_t val);
  uint8_t writeRegBuf(uint8_t addr, uint8_t *data, uint8_t count);
  uint8_t readReg(uint8_t reg);
  uint8_t sendMessage(char *buf, uint8_t size);
private:
	SPI_interface _spi;
};

#endif
