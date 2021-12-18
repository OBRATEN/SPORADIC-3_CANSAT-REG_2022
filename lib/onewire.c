#include "onewire.h"

void OW_init(void) {
  OWDIR |= _BV(OWPINN);
}

static inline void OW_setLow(void) {
  OWPORT &= ~_BV(OWPINN);
}

static inline void OW_setHigh(void) {
  OWPORT |= _BV(OWPINN);
}

static inline void OW_input(void) {
  OWDIR &= ~_BV(OWPINN);
}

static inline void OW_output(void) {
  OWDIR |= _BV(OWPINN);
}

static inline uint8_t OW_level(void) {
  return OWPIN & _BV(OWPINN);
}

uint8_t OW_reset(void) {
  uint8_t res = 0;
  OW_setLow();
  OW_output();
  _delay_us(480);
  OW_input();
  _delay_us(60);
  res = OW_level();
  _delay_us(420);
  return res;
}

uint8_t OW_readBit(void) {
  uint8_t bit = 0;
  OW_setLow();
  OW_output();
  _delay_us(1);
  OW_input();
  _delay_us(14);
  if (OW_level()) bit = 1;
  _delay_us(45);
  return bit;
}

void OW_writeBit(uint8_t bit) {
  OW_setLow();
  OW_output();
  _delay_us(1);
  if (bit) OW_input();
  _delay_us(60);
  OW_input();
}

uint8_t OW_readByte(void) {
  uint8_t data = 0, i = 8;
  while (i--) {
    data = data >> 1;
    data |= (OW_readBit() << 7);
  } return data;
}

void OW_writeByte(uint8_t data) {
  uint8_t i = 8;
  while (i--) {
    OW_writeBit(data & 1);
    data = data >> 1;
  }
}

