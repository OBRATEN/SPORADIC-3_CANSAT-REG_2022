#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SS PINB0
#define SCK PINB1
#define MOSI PINB2
#define MISO PINB3

#define SS_low()  SPI_PORT &= ~(1 << SS)
#define SS_high() SPI_PORT |=  (1 << SS);

class SPI_interface {
public:
  void init(void);
  uint8_t inited(void);
  uint8_t writeByte(uint8_t data);
  uint8_t readByte(void);
  uint8_t transfer(uint8_t data);
private:
  uint8_t _data;
  uint8_t _inited = 0;
};

#endif
