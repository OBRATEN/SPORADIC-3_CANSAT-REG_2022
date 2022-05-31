#ifndef NRF_H
#define NRF_H

#include "config.h"
#include "pinout.h"
#include "spi.hpp"

#define NRF_READ_MASK 0x1F
#define NRF_WRITE_MASK 0x20

class nRF_radio {
public:
  nRF_radio();
  uint8_t init(void);
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