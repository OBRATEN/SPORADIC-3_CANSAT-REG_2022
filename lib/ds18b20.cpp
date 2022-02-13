#include "ds18b20.h"

DS18_term::DS18_term(void) {
  OW_init();
}

void DS18_term::readRawData(uint8_t *data) {
  OW_reset();
  OW_writeByte(0xCC);
  OW_writeByte(0x44);
  while (!(OW_readBit()));
  OW_reset();
  OW_writeByte(0xCC);
  OW_writeByte(0xBE);
  data[0] = OW_readByte();
  data[1] = OW_readByte();
  OW_reset();
}

void DS18_term::readTemp(float *res) {
  uint8_t temp[2];
  DS18_term::readRawData(temp);
  *res = (float)((int)temp[0] | (((int)temp[1]) << 8)) * 0.0625 + 0.03125;
}
