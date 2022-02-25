#include "onewire.hpp"

void OW_interface::init(void) {
  OWDIR |= _BV(OWPINN);
}

inline void OW_interface::setLow(void) {
  OWPORT &= ~_BV(OWPINN);
}

inline void OW_interface::setHigh(void) {
  OWPORT |= _BV(OWPINN);
}

inline void OW_interface::input(void) {
  OWDIR &= ~_BV(OWPINN);
}

inline void OW_interface::output(void) {
  OWDIR |= _BV(OWPINN);
}

inline uint8_t OW_interface::level(void) {
  return OWPIN & _BV(OWPINN);
}

uint8_t OW_interface::reset(void) {
  uint8_t res = 0;
  this->setLow();
  this->output();
  _delay_us(480);
  this->input();
  _delay_us(60);
  res = this->level();
  _delay_us(420);
  return res;
}

uint8_t OW_interface::readBit(void) {
  uint8_t bit = 0;
  this->setLow();
  this->output();
  _delay_us(1);
  this->input();
  _delay_us(14);
  if (this->level()) bit = 1;
  _delay_us(45);
  return bit;
}

void OW_interface::writeBit(uint8_t bit) {
  this->setLow();
  this->output();
  _delay_us(1);
  if (bit) this->input();
  _delay_us(60);
  this->input();
}

uint8_t OW_interface::readByte(void) {
  uint8_t data = 0, i = 8;
  while (i--) {
    data = data >> 1;
    data |= (this->readBit() << 7);
  } return data;
}

void OW_interface::writeByte(uint8_t data) {
  uint8_t i = 8;
  while (i--) {
    this->writeBit(data & 1);
    data = data >> 1;
  }
}

