#include "spi.hpp"

void SPI_interface::init(void) {
	SPI_DDR |= (1 << MOSI) | (1 << SCK) | (1 << SS);
  SPI_DDR &= ~(1 << MISO);
  SPCR = (0 << SPIE) |
         (1 << SPE)  |
         (0 << DORD) |
         (1 << MSTR) |
         (0 << CPOL) |
         (0 << SPR1) | (0 << SPR0);
  _inited = 1;
  SPSR |= _BV(SPI2X);
}

uint8_t SPI_interface::inited(void) {
  return _inited;
}

uint8_t SPI_interface::writeByte(uint8_t data) {
  //if (!(_inited)) return 0;
  SPDR = data;
  while (!(SPSR & (1 << SPIF)));
  _data = SPDR;
  return data;
}

uint8_t SPI_interface::readByte(void) {
  //if (!(_inited)) return 0;
  SPDR = 0xff;
  while (!(SPSR & (1 << SPIF)));
  return SPDR;
}

uint8_t SPI_interface::writeReadByte(uint8_t data) {
  //if (!(_inited)) return 0;
  SPDR = data;
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}
