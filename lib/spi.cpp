#include "spi.hpp"

void SPI_interface::init(void) {
  SPI_DDR  |= (1 << MOSI) | (1 << CSN ) | (1 << SCK) | (0 << MISO);
  SPI_PORT |= (1 << MISO) | (1 << MOSI) | (1 << CSN) | (1 << SCK);
  SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (1 << SPR0);
  SPSR = (0 << SPI2X);
  _inited = 1;
}

void SPI_interface::writeByte(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
}

uint8_t SPI_interface::readByte(void) {
  SPDR = 0xff;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

uint8_t SPI_interface::transfer(uint8_t data) {
  SPDR = data;
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}
