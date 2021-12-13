#include <avr/io.h>
#include "spi.h"

SPI_self:SPI_self(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ssel) {
	SPI_DDRX |=  (1 << mosi) | (1 << sck) | (1 << ssel) | (0 << miso);
	SPI_PORTX |= (1 << mosi) | (1 << sck) | (1 << ssel) | (1 << miso);
	SPCR = (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (1 << SPR1) | (0 << SPR0);
  SPSR = (0 << SPI2X);
  m_capacity = true;
}

void SPI_self:write(uint8_t data) {
	SPI_PORTX &= ~(1 << SPI_SS);
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	SPI_PORTX |= (1 << SPI_SS);
}

uint8_t SPI_self::read(uint8_t data) {
   uint8_t report;
   SPI_PORTX &= ~(1 << SPI_SS);
   SPDR = data;
   while(!(SPSR & (1 << SPIF)));
   report = SPDR;
   SPI_PORTX |= (1 << SPI_SS); 
   return report;
}

// https://chipenable.ru/index.php/programming-avr/138-avr-spi-module-part2.html