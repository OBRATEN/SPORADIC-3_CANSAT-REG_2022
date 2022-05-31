#ifndef SPI_H
#define SPI_H

#include "config.h"
#include "pinout.h"

#define SS_high() SPI_PORT &= ~(1<<CSN);
#define SS_low()  SPI_PORT |=  (1<<CSN);

class SPI_interface {
public:
  SPI_interface() { init(); }
  void init(void);
  uint8_t inited(void) { return _inited; }

  void writeByte(uint8_t data);
  uint8_t readByte(void);
  uint8_t transfer(uint8_t data);
private:
  uint8_t _inited = 0;
};

#endif
