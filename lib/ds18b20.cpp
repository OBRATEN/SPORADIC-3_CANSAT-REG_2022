#include "ds18b20.hpp"

DS18_term::DS18_term(void) {
  init();
}

void DS18_term::startMeasure(void) {
  reset();
  writeByte(0xCC);
  writeByte(0x44);
}

void DS18_term::readRawData(uint8_t *data) {
  while (!(readBit()));
  reset();
  writeByte(0xCC);
  writeByte(0xBE);
  data[0] = readByte();
  data[1] = readByte();
  reset();
}

void DS18_term::readTemp(float *res) {
  uint8_t temp[2];
  readRawData(temp);
  *res = (float)((int)temp[0] | (((int)temp[1]) << 8)) * 0.0625 + 0.03125;
  startMeasure();
}
