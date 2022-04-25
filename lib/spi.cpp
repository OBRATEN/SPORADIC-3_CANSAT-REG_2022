#include "spi.hpp"

void SPI_interface::init(void) {
    // set as master
    SPCR |= _BV(MSTR);
    // enable SPI
    SPCR |= _BV(SPE);
    // MISO pin automatically overrides to input
    SPI_DDR |= (1 << SCK);
    SPI_DDR |= (1 << MOSI);
    SPI_DDR &= ~(1 << MISO);
    // SPI mode 0: Clock Polarity CPOL = 0, Clock Phase CPHA = 0
    SPCR &= ~_BV(CPOL);
    SPCR &= ~_BV(CPHA);
    // Clock 2X speed
    SPCR &= ~_BV(SPR0);
    SPCR &= ~_BV(SPR1);
    SPSR |= _BV(SPI2X);
    // most significant first (MSB)
    SPCR &= ~_BV(DORD);
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

uint8_t SPI_interface::transfer(uint8_t data) {
  //if (!(_inited)) return 0;
  SPDR = data;
  asm volatile("nop");
  while(!(SPSR & (1 << SPIF)));
  return SPDR;
}
