#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

void SPI_init(void);
uint8_t SPI_writeByte(uint8_t data);
uint8_t SPI_readByte(void);
uint8_t SPI_writeReadByte(uint8_t data);

#endif
