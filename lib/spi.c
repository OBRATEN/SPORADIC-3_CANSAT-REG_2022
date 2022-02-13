#include "spi.h"

#define SPI_DDR DDRB
#define SS PINB0
#define SCK PINB1
#define MOSI PINB2
#define MISO PINB3

uint8_t datareg;

void SPI_init(void) {
  SPI_DDR |= (1 << MOSI) | (1 << SCK) | (1 << SS);
  SPI_DDR &= ~(1 << MISO);
  SPCR = (1 << SPE) | (1 << DORD) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA) | (1 << SPR1) | (1 << SPR0);
  SPSR = (0 << SPI2X);
}

uint8_t SPI_writeByte(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  datareg = SPDR;
  return data;
}

uint8_t SPI_readByte(void) {
  uint8_t data;
  SPDR = 0xff;
  while (!(SPSR & (1 << SPIF)));
  data = SPDR;
  return data;
}

uint8_t SPI_writeReadByte(uint8_t data) {
  SPDR = data;
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}
