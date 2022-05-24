#ifndef NRF_H
#define NRF_H

#define F_CPU 8000000UL

#define NRF24L01__READ_MASK 0x1F
#define NRF24L01__WRITE_MASK 0x20

#include <util/delay.h>
#include "spi.hpp"



class NRF {
public:
  NRF();
  void begin(void);

  void writeReg(uint8_t reg, uint8_t data );
  void writeReg(uint8_t reg, const uint8_t* buf, uint8_t len);

  uint8_t readReg(uint8_t reg);
  void flush_tx();
  void set_address(uint64_t c);

  void send(const void* buf);
private:
  SPI_interface _spi;
};

#endif // NRF_H