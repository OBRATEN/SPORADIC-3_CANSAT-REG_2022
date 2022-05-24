#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define CSN 0
#define SCK 1
#define MOSI 2
#define MISO 3
#define CE 4
#define SS PORTB5

#define SS_high() SPI_PORT &= ~(1<<CSN);
#define SS_low()  SPI_PORT |=  (1<<CSN);

class SPI_interface {
public:
  SPI_interface();
  uint8_t inited(void);

  void writeByte(uint8_t data);
  uint8_t readByte(void);
  uint8_t transfer(uint8_t data);
private:
  uint8_t _inited = 0;
};

#endif
