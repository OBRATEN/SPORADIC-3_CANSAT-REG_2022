#include "onewire.hpp"

void OW_interface::init(void) {
  OWDIR |= (1 << OWPINN);
  _inited = 1;
}

inline void OW_interface::setLow(void) {
  OWPORT &= ~(1 << OWPINN);
}

inline void OW_interface::setHigh(void) {
  OWPORT |= (1 << OWPINN);
}

inline void OW_interface::input(void) {
  OWDIR &= ~(1 << OWPINN);
}

inline void OW_interface::output(void) {
  OWDIR |= (1 << OWPINN);
}

inline uint8_t OW_interface::level(void) {
  return (OWPIN & (1 << OWPINN));
}

uint8_t OW_interface::reset(void) {
  uint8_t res = 0;
  setLow();
  output();
  _delay_us(480);
  input();
  _delay_us(60);
  res = level();
  _delay_us(420);
  return res;
}

uint8_t OW_interface::readBit(void) {
  uint8_t bit = 0;
  setLow();
  output();
  _delay_us(1);
  input();
  _delay_us(14);
  if (level()) bit = 1;
  _delay_us(45);
  return bit;
}

void OW_interface::writeBit(uint8_t bit) {
  setLow();
  output();
  _delay_us(1);
  if (bit) input();
  _delay_us(60);
  input();
}

uint8_t OW_interface::readByte(void) {
  uint8_t data = 0, i = 8;
  while (i--) {
    data = data >> 1;
    data |= (readBit() << 7);
  } return data;
}

void OW_interface::writeByte(uint8_t data) {
  uint8_t i = 8;
  while (i--) {
    writeBit(data & 1);
    data = data >> 1;
  }
}

