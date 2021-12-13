#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

OW::OW(void) {
	OWDIR |= (1 << OWPINN);
}

inline void OW::setLow(void) {
	OWDIR |= _BV(OWPINN);
}

inline void OW::setHigh(void) {
	OWDIR &= ~_BV(OWPINN);
}

inline uint8_t OW::level(void) {
	return OWPIN & _BV(OWPINN);
}

bool OW::reset(void) {
	OW::setLow();
	_delay_us(640);
	OW::setHigh();
	_delay_us(2);
	for (uint8_t i = 0; i < 80; i++) {
		if (!(OW::level())) {
			while (!(OW::level())) {}
			m_capacity = 1;
			return 1;
		}
		_delay_us(1);
	}
	return 0;
}

void OW::writeBit(uint8_t bit) {
	OW::setLow();
	if (bit) {
		_delay_us(5);
		OW::setHigh();
		_delay_us(90);
	} else {
		_delay_us(90);
		OW::setHigh();
		_delay_us(5);
	}
}

void OW::writeByte(uint8_t byte) {
	for (uint8_t i = 8; i; i--) {
		OW::writeBit(byte & 1);
		byte = byte >> 1;
	}
}

uint8_t OW::readBit(void) {
	uint8_t bit;
	OW::setLow();
	_delay_us(2);
	OW::setHigh();
	_delay_us(8);
	bit = OW::level();
	_delay_us(80);
	return bit;
}

uint8_t OW::readByte(void) {
	uint8_t *byte = (uint8_t*)malloc(sizeof(uint8_t));
	for (uint8_t i = 8; i; i--) {
		*byte >>= 1;
		if (OW::readBit()) *byte |= 0x80;
	} return *byte;
	delete byte;
}

bool OW::getCapacity(void) {
	return m_capacity;
}