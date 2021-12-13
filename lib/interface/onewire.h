#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <avr/io.h>
#include <util/delay.h>

#define OWPORT PORTC
#define OWDIR  DDRC
#define OWPIN  PINC
#define OWPINN PC1


class OW {
public:
	OW(void);
	bool reset(void);
	inline uint8_t level(void);
	void writeBit(uint8_t bit);
	void writeByte(uint8_t byte);
	uint8_t readBit(void);
	uint8_t readByte(void);
	inline void setLow(void);
	inline void setHigh(void);
	bool getCapacity(void);
private:
	bool m_capacity;
};

#endif