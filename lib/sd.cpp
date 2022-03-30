#include "sd.hpp"

inline void CS_assert(void) {
	PORTB &= ~0x04;
}

inline void CS_deassert(void) {
	PORTB |= 0x04;
}

uint8_t SD_card::init(void) {
	uint8_t response, cardVersion;
	uint16_t retry = 0;
	for (uint8_t i = 0; i < 10; i++) _spi.writeReadByte(0xFF);
	CS_assert();
	do {
		response = this->sendCmd(GO_IDLE_STATE, 0);
		retry++;
		if (retry > 32) return 0;
	} while (response != 0x01);
	CS_deassert();
	_spi.writeReadByte(0xFF);
	_spi.writeReadByte(0xFF);
	retry = 0;
	cardVersion = 2;
	do {
		response = this->sendCmd(SEND_IF_COND, 0x000001AA);
		retry++;
		if (retry > 254) {
			
		}
	}
}