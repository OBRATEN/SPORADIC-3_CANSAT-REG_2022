#include <avr/io.h>
#include <util/delay.h>
#include "ds18b20.h"

OW ow;

void Thermo::startConvert(void) {
	ow.reset();
	ow.writeByte(0xCC);
	ow.writeByte(0x44);
}

uint8_t Thermo::readSp(uint8_t *sp) {
	if (ow.reset() == 0) return 0;
	ow.writeByte(0xCC);
	ow.writeByte(0xBE);
	for (uint8_t i = 0; i < 9; i++) sp[i] = ow.readByte();
	return 1;
}

void Thermo::readTemp(int16_t *temp) {
	uint8_t sp[9];
	uint8_t ec = Thermo::readSp(sp);
	if (!(ec)) *temp = -60;
	else *temp = (int16_t)(sp[1] << 8) + sp[0];
}

bool Thermo::getCapacity(void) {
	return m_capacity;
}