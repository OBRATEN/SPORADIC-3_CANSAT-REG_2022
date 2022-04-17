#include "spi.hpp"

void SPI_interface::init(void) {
	SPI_DDR |= (1 << MOSI) | (1 << SCK) | (1 << SS);
  SPI_DDR &= ~(1 << MISO);
  SPCR = (0 << SPIE) |
         (1 << SPE)  |
         (0 << DORD) |
         (1 << MSTR) |
         (0 << CPOL) |
         (1 << SPR1) | (1 << SPR0);
  SPSR = 0x00;
  _inited = 1;
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
  asm volatile("nop");
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}
