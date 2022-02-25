#include "ds18b20.hpp"

DS18_term::DS18_term(void) {
  this->init();
}

void DS18_term::readRawData(uint8_t *data) {
  this->reset();
  this->writeByte(0xCC);
  this->writeByte(0x44);
  while (!(this->readBit()));
  this->reset();
  this->writeByte(0xCC);
  this->writeByte(0xBE);
  data[0] = this->readByte();
  data[1] = this->readByte();
  this->reset();
}

void DS18_term::readTemp(float *res) {
  uint8_t temp[2];
  this->readRawData(temp);
  *res = (float)((int)temp[0] | (((int)temp[1]) << 8)) * 0.0625 + 0.03125;
}
