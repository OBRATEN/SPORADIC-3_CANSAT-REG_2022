#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

class SPI_self {
public:
	SPI_self(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ssel);
	void write(uint8_t data);
	uint8_t read(void);
	bool getCapacity(void);

private:
	bool m_capacity;
};

#endif