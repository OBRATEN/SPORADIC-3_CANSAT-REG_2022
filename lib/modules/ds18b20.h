#ifndef DS18B20_H
#define DS18B20_H

#include <avr/io.h>
#include <util/delay.h>
#include "interface/onewire.h"
#include "interface/onewire.cpp"

class Thermo {
public:
	void startConvert(void);
	uint8_t readSp(uint8_t *sp);
	void readTemp(int16_t *temp);
	bool getCapacity(void);
private:
	bool m_capacity;
};

#endif